// Copyright 2026 p7cq <707c71@gmail.com>
// SPDX-License-Identifier: Apache-2.0

package main

import (
	"fmt"
	"net"
	"net/http"
	"testing"
	"time"
)

// A client that never finishes its request headers is dropped after
// ReadHeaderTimeout instead of holding a connection forever (slowloris).
func TestSlowHeadersDropped(t *testing.T) {
	srv := newHTTPServer("127.0.0.1:0", http.NotFoundHandler())
	if srv.ReadHeaderTimeout == 0 || srv.ReadTimeout == 0 ||
		srv.IdleTimeout == 0 {
		t.Fatal("read/idle timeouts unset")
	}
	srv.ReadHeaderTimeout = 200 * time.Millisecond // keep the test fast
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	go srv.Serve(ln)
	t.Cleanup(func() { srv.Close() })

	c, err := net.Dial("tcp", ln.Addr().String())
	if err != nil {
		t.Fatal(err)
	}
	defer c.Close()
	// Headers never terminated by the blank line.
	fmt.Fprint(c, "GET /acme/directory HTTP/1.1\r\nHost: x\r\n")
	c.SetReadDeadline(time.Now().Add(5 * time.Second))
	_, err = c.Read(make([]byte, 1))
	if ne, ok := err.(net.Error); ok && ne.Timeout() {
		t.Fatal("connection still open after the header timeout")
	}
}
