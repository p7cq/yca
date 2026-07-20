// Command yca-acme is an ACME (RFC 8555) issuance frontend for yca CA.
// It owns the protocol state (accounts, orders, challenges) in its own
// database and never opens the CA store: issuance execs the yca CLI, the
// single owner of the store schema.
//
//	yca-acme [flags]                     run the daemon
//	yca-acme eab new [--allow patterns]  provision an EAB credential
//	yca-acme eab list                    list provisioned credentials
//	yca-acme eab delete <kid>            revoke a credential
//	yca-acme ari accelerate --issuer cn  have that issuer's certificates
//	                                     replaced at once (CA compromise)
//	yca-acme ari list                    list active accelerations
//	yca-acme ari clear --issuer cn       back to the policy window
package main

import (
	"context"
	"crypto/rand"
	"encoding/base64"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"os"
	"strings"
	"time"
)

// Injected at build time (-ldflags).
var version = "dev"

// tsWriter prefixes each log line with a timestamp matching the CA's
// yca.log format (see responder/main.go).
type tsWriter struct{ w io.Writer }

func (t tsWriter) Write(p []byte) (int, error) {
	if _, err := fmt.Fprintf(t.w, "%s ", time.Now().Format("2006-01-02 15:04:05.000")); err != nil {
		return 0, err
	}
	return t.w.Write(p)
}

type server struct {
	db      *DB
	baseURL string // external base URL (clients sign it into every JWS)
	yca     *ycaRunner
	http01  *http01Validator
	dns01   *dns01Validator
}

func main() {
	log.SetFlags(0)
	log.SetOutput(tsWriter{os.Stderr})

	if len(os.Args) > 1 && os.Args[1] == "eab" {
		eabMain(os.Args[2:])
		return
	}
	if len(os.Args) > 1 && os.Args[1] == "ari" {
		ariMain(os.Args[2:])
		return
	}

	var (
		state   = flag.String("state", "./acme.db", "protocol state database")
		listen  = flag.String("listen", "127.0.0.1:8555", "listen address")
		baseURL = flag.String("url", "http://127.0.0.1:8555",
			"external base URL (what clients see, e.g. https://pki.example.ca)")
		ycaBin  = flag.String("yca", "yca", "path to the yca binary")
		config  = flag.String("config", "", "yca --config (passed through)")
		store   = flag.String("store", "", "yca --store (passed through)")
		id      = flag.String("id", "acme", "enrolled identity used for signing")
		leafVal = flag.String("valid", "",
			"validity requested per issuance, passed to 'yca sign --valid' "+
				"(<N><s|m|h|d>; empty: the CA's ee_valid_days policy)")
		vaPort  = flag.Int("http01-port", 80, "port http-01 validation connects to")
		dnsAddr = flag.String("dns", "",
			"resolver for dns-01 TXT lookups, host[:53] (default: system)")
		tlsCrt  = flag.String("tls-cert", "", "serve TLS with this certificate")
		tlsKey  = flag.String("tls-key", "", "TLS private key")
		showVer = flag.Bool("version", false, "print version and exit")
	)
	flag.Parse()
	if *showVer {
		fmt.Printf("yca-acme version %s\n", version)
		return
	}

	db, err := OpenDB(*state)
	if err != nil {
		log.Fatalf("open state %s: %v", *state, err)
	}
	defer db.Close()

	s := &server{
		db:      db,
		baseURL: strings.TrimSuffix(*baseURL, "/"),
		yca:     newYcaRunner(*ycaBin, *config, *store, *id, *leafVal),
		http01:  &http01Validator{port: *vaPort},
		dns01:   &dns01Validator{lookupTXT: resolverFor(*dnsAddr).LookupTXT},
	}
	if err := s.yca.selfCheck(); err != nil {
		log.Fatalf("yca: %v", err)
	}
	go gcLoop(db)

	mux := http.NewServeMux()
	mux.HandleFunc("GET /acme/directory", s.handleDirectory)
	mux.HandleFunc("HEAD /acme/new-nonce", s.handleNewNonce)
	mux.HandleFunc("GET /acme/new-nonce", s.handleNewNonce)
	mux.HandleFunc("POST /acme/new-account", s.handleNewAccount)
	mux.HandleFunc("POST /acme/acct/{id}", s.handleAccount)
	mux.HandleFunc("POST /acme/new-order", s.handleNewOrder)
	mux.HandleFunc("POST /acme/order/{id}", s.handleOrder)
	mux.HandleFunc("POST /acme/order/{id}/finalize", s.handleFinalize)
	mux.HandleFunc("POST /acme/authz/{id}", s.handleAuthz)
	mux.HandleFunc("POST /acme/chall/{id}", s.handleChallenge)
	mux.HandleFunc("POST /acme/cert/{id}", s.handleCert)
	mux.HandleFunc("POST /acme/revoke-cert", s.handleRevokeCert)
	mux.HandleFunc("POST /acme/key-change", s.handleKeyChange)
	mux.HandleFunc("GET /acme/renewal-info/{id}", s.handleRenewalInfo)
	mux.HandleFunc("GET /healthz", func(w http.ResponseWriter, _ *http.Request) {
		w.WriteHeader(http.StatusOK)
	})

	log.Printf("listening on %s, directory %s/acme/directory", *listen, s.baseURL)
	if *tlsCrt != "" {
		log.Fatal(http.ListenAndServeTLS(*listen, *tlsCrt, *tlsKey, mux))
	}
	log.Fatal(http.ListenAndServe(*listen, mux))
}

func (s *server) handleDirectory(w http.ResponseWriter, _ *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	_ = writeIndentedJSON(w, map[string]any{
		"newNonce":    s.url("/acme/new-nonce"),
		"newAccount":  s.url("/acme/new-account"),
		"newOrder":    s.url("/acme/new-order"),
		"revokeCert":  s.url("/acme/revoke-cert"),
		"keyChange":   s.url("/acme/key-change"),
		"renewalInfo": s.url("/acme/renewal-info"),
		"meta": map[string]any{
			"externalAccountRequired": true,
		},
	})
}

func (s *server) handleNewNonce(w http.ResponseWriter, r *http.Request) {
	nonce, err := s.db.IssueNonce()
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	w.Header().Set("Replay-Nonce", nonce)
	w.Header().Set("Cache-Control", "no-store")
	if r.Method == http.MethodGet { // RFC 8555 §7.2: GET gets 204
		w.WriteHeader(http.StatusNoContent)
		return
	}
	w.WriteHeader(http.StatusOK)
}

// Protocol-state garbage collection: orders/authzs expire 24 h after
// creation (RFC-visible in the resource); the grace period keeps
// recently-expired objects readable a while longer so clients can still
// fetch the error off a failed order.
const (
	gcInterval = time.Hour
	gcGrace    = 24 * time.Hour
)

func gcLoop(db *DB) {
	for {
		n, err := db.GC(gcGrace)
		switch {
		case err != nil:
			log.Printf("gc: %v", err)
		case n > 0:
			log.Printf("gc: removed %d expired protocol objects", n)
		}
		time.Sleep(gcInterval)
	}
}

// resolverFor returns the system resolver, or one pinned to `addr`
// (host[:port], :53 assumed) - dns-01 records live on the operator's own
// nameserver, which is not necessarily the host's resolv.conf.
func resolverFor(addr string) *net.Resolver {
	if addr == "" {
		return net.DefaultResolver
	}
	if _, _, err := net.SplitHostPort(addr); err != nil {
		addr = net.JoinHostPort(addr, "53")
	}
	target := addr
	return &net.Resolver{
		PreferGo: true,
		Dial: func(ctx context.Context, network, _ string) (net.Conn, error) {
			var d net.Dialer
			return d.DialContext(ctx, network, target)
		},
	}
}

// --- eab subcommand ---

// splitAllow parses the comma-separated --allow patterns ("" = allow all).
func splitAllow(s string) []string {
	if s == "" {
		return nil
	}
	parts := strings.Split(s, ",")
	for i := range parts {
		parts[i] = strings.TrimSpace(parts[i])
	}
	return parts
}

func eabMain(args []string) {
	fs := flag.NewFlagSet("eab", flag.ExitOnError)
	state := fs.String("state", "./acme.db", "protocol state database")
	allow := fs.String("allow", "",
		"comma-separated identifier patterns this credential may order "+
			"(e.g. '*.example.ca,host.example.ca'; empty = any)")
	if len(args) < 1 {
		fmt.Fprintln(os.Stderr, "usage: yca-acme eab <new|list|delete> [--state db] [--allow patterns] [kid]")
		os.Exit(2)
	}
	verb := args[0]
	_ = fs.Parse(args[1:])

	db, err := OpenDB(*state)
	if err != nil {
		log.Fatalf("open state %s: %v", *state, err)
	}
	defer db.Close()

	switch verb {
	case "new":
		kid := newID()
		hmac := make([]byte, 32)
		if _, err := rand.Read(hmac); err != nil {
			log.Fatalf("rng: %v", err)
		}
		if err := db.InsertEAB(kid, hmac, *allow); err != nil {
			log.Fatalf("insert: %v", err)
		}
		// Shown once, like the CA passphrase banner: the HMAC key is not
		// recoverable from the (hashed-nothing, but private) state db by
		// design of the workflow - hand it to the client operator now.
		fmt.Printf("\n=== EAB CREDENTIAL (shown once) ===\n"+
			"kid : %s\nhmac: %s\nallow: %s\n"+
			"(e.g. acme.sh --register-account --eab-kid <kid> --eab-hmac-key <hmac>)\n"+
			"===================================\n\n",
			kid, base64.RawURLEncoding.EncodeToString(hmac), orAny(*allow))
	case "list":
		creds, err := db.ListEAB()
		if err != nil {
			log.Fatalf("list: %v", err)
		}
		for _, c := range creds {
			fmt.Printf("%s\tallow: %s\n", c[0], orAny(c[1]))
		}
	case "delete":
		kid := fs.Arg(0)
		if kid == "" {
			fmt.Fprintln(os.Stderr, "usage: yca-acme eab delete [--state db] <kid>")
			os.Exit(2)
		}
		// The bound accounts survive, but allowed() denies every order
		// once the credential is gone - this IS the containment step.
		bound, err := db.AccountsByEAB(kid)
		if err != nil {
			log.Fatalf("accounts: %v", err)
		}
		ok, err := db.DeleteEAB(kid)
		if err != nil {
			log.Fatalf("delete: %v", err)
		}
		if !ok {
			log.Fatalf("no EAB credential with kid %s", kid)
		}
		fmt.Printf("deleted EAB credential %s\n", kid)
		if len(bound) > 0 {
			fmt.Printf("%d account(s) were registered with it and can no "+
				"longer order: %s\n", len(bound), strings.Join(bound, " "))
		}
	default:
		fmt.Fprintln(os.Stderr, "usage: yca-acme eab <new|list|delete>")
		os.Exit(2)
	}
}

func orAny(s string) string {
	if s == "" {
		return "(any)"
	}
	return s
}

// ariMain drives the renewal-acceleration directives: after a CA
// compromise the operator revokes the generation (`yca revoke ca`) and
// tells the frontend to have everything that generation signed replaced
// at once, instead of at its policy age. Keying on the issuer makes the
// directive self-limiting: a renewed certificate is signed by the
// successor, so it stops matching.
func ariMain(args []string) {
	fs := flag.NewFlagSet("ari", flag.ExitOnError)
	state := fs.String("state", "./acme.db", "protocol state database")
	issuer := fs.String("issuer", "",
		"common name of the issuing CA generation (e.g. 'CA E1')")
	window := fs.Duration("window", 2*time.Hour,
		"how long the fleet has to replace; clients pick a random moment "+
			"inside it, so size it for the number of certificates (issuance "+
			"is serialized)")
	if len(args) < 1 {
		fmt.Fprintln(os.Stderr,
			"usage: yca-acme ari <accelerate|list|clear> [--state db] "+
				"[--issuer cn] [--window dur]")
		os.Exit(2)
	}
	verb := args[0]
	_ = fs.Parse(args[1:])

	db, err := OpenDB(*state)
	if err != nil {
		log.Fatalf("open state %s: %v", *state, err)
	}
	defer db.Close()

	switch verb {
	case "accelerate":
		if *issuer == "" {
			log.Fatal("--issuer is required")
		}
		if *window <= 0 {
			log.Fatal("--window must be positive")
		}
		if err := db.SetAccel(*issuer, *window); err != nil {
			log.Fatalf("set: %v", err)
		}
		fmt.Printf("certificates issued by %q are now suggested for "+
			"replacement within %s\n", *issuer, *window)
	case "list":
		rows, err := db.ListAccel()
		if err != nil {
			log.Fatalf("list: %v", err)
		}
		for _, a := range rows {
			fmt.Printf("%s\twindow %s\tsince %s\n", a.IssuerCN, a.Window,
				a.Created.UTC().Format(time.RFC3339))
		}
	case "clear":
		if *issuer == "" {
			log.Fatal("--issuer is required")
		}
		gone, err := db.ClearAccel(*issuer)
		if err != nil {
			log.Fatalf("clear: %v", err)
		}
		if !gone {
			log.Fatalf("no acceleration set for %q", *issuer)
		}
		fmt.Printf("cleared the acceleration for %q\n", *issuer)
	default:
		log.Fatalf("unknown ari verb %q (accelerate|list|clear)", verb)
	}
}
