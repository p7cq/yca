// Command yca-ocsp is the delegated OCSP responder (RFC 6960) for a yca
// CA store. It reads revocation status from the store's cert_index table and
// signs responses with the short-lived responder credentials issued by
// `yca create ocsp`.
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"time"
)

// Injected at build time (-ldflags).
var version = "dev"

// tsWriter prefixes each log line with a timestamp matching the CA's
// yca.log format (2006-01-02 15:04:05.000); the standard log flags cannot
// produce it. Safe per line: log writes each line in a single Write call.
type tsWriter struct{ w io.Writer }

func (t tsWriter) Write(p []byte) (int, error) {
	if _, err := fmt.Fprintf(t.w, "%s ", time.Now().Format("2006-01-02 15:04:05.000")); err != nil {
		return 0, err
	}
	return t.w.Write(p)
}

func main() {
	log.SetFlags(0)
	log.SetOutput(tsWriter{os.Stderr})

	var (
		storeDir = flag.String("store", "./store", "yca store directory")
		listen   = flag.String("listen", "127.0.0.1:23509", "listen address")
		basePath = flag.String("path", "/ocsp", "base path (the AIA URL path)")
		validity = flag.Duration("validity", time.Hour,
			"response window (thisUpdate..nextUpdate); also drives max-age")
		showVer = flag.Bool("version", false, "print version and exit")
	)
	flag.Parse()
	if *showVer {
		fmt.Printf("yca-ocsp version %s\n", version)
		return
	}

	store, err := OpenStore(*storeDir)
	if err != nil {
		log.Fatalf("open store %s: %v", *storeDir, err)
	}
	defer store.Close()
	log.Printf("store %s: issuer %q, responder %q (notAfter %s)", *storeDir,
		store.Issuer.Subject.CommonName,
		store.Creds().Cert.Subject.CommonName,
		store.Creds().Cert.NotAfter.Format(time.RFC3339))

	h, err := newOCSPHandler(store, *validity)
	if err != nil {
		log.Fatalf("handler: %v", err)
	}
	mux := http.NewServeMux()
	// RFC 6960 A.1: POST with the DER request as body, or GET with the
	// base64 request in the path ({b64...} tolerates unencoded '/').
	mux.Handle("POST "+*basePath, h)
	mux.Handle("GET "+*basePath+"/{b64...}", h)
	mux.HandleFunc("GET /healthz", func(w http.ResponseWriter, _ *http.Request) {
		if err := store.Healthy(); err != nil {
			log.Printf("healthz: %v", err)
			http.Error(w, "unhealthy", http.StatusServiceUnavailable)
			return
		}
		w.WriteHeader(http.StatusOK)
	})

	log.Printf("listening on %s, path %s", *listen, *basePath)
	log.Fatal(http.ListenAndServe(*listen, mux))
}
