// Improved Go integration for a lightweight NLP client
package main

import (
    "bytes"
    "encoding/json"
    "flag"
    "fmt"
    "io"
    "log"
    "net/http"
    "os"
    "time"
)

// NLPRequest is sent to an external NLP service (or used locally).
type NLPRequest struct {
    Text string `json:"text"`
    Mode string `json:"mode,omitempty"`
}

// NLPResponse is the minimal expected response shape.
type NLPResponse struct {
    Result string `json:"result"`
}

func main() {
    // CLI flags to keep the example simple and useful during development.
    text := flag.String("text", "", "Text to analyze (use -text or pipe input)")
    host := flag.String("host", "localhost", "NLP service host")
    port := flag.Int("port", 8000, "NLP service port")
    mode := flag.String("mode", "default", "Analysis mode")
    timeout := flag.Int("timeout", 5, "HTTP timeout seconds")
    stub := flag.Bool("stub", false, "Use local stub instead of HTTP call")
    useRust := flag.Bool("rust", false, "Use Rust native compute (requires built lib)")
    flag.Parse()

    // If no -text provided, try reading from stdin for piping workflows.
    if *text == "" {
        stat, _ := os.Stdin.Stat()
        if (stat.Mode() & os.ModeCharDevice) == 0 {
            b, err := io.ReadAll(os.Stdin)
            if err == nil && len(b) > 0 {
                *text = string(b)
            }
        }
    }

    if *text == "" {
        log.Fatal("no text provided: use -text or pipe input")
    }

    // Optionally use a local, fast stub for development/testing.
    if *stub {
        res := localStubAnalyze(*text, *mode)
        fmt.Println(res)
        return
    }

    // Optionally call into the Rust native library for a numeric compute.
    if *useRust {
        // callRustCompute is provided by rust_client.go using cgo.
        n := callRustCompute(*text)
        fmt.Printf("rust.words=%d\n", n)
        return
    }

    url := fmt.Sprintf("http://%s:%d/analyze", *host, *port)
    client := &http.Client{Timeout: time.Duration(*timeout) * time.Second}

    resp, err := sendToNLP(client, url, NLPRequest{Text: *text, Mode: *mode})
    if err != nil {
        log.Printf("HTTP error: %v - falling back to local stub", err)
        fmt.Println(localStubAnalyze(*text, *mode))
        return
    }

    fmt.Println(resp.Result)
}

// sendToNLP posts the request JSON to the external service and decodes the response.
func sendToNLP(client *http.Client, url string, req NLPRequest) (*NLPResponse, error) {
    b, err := json.Marshal(req)
    if err != nil {
        return nil, err
    }
    r, err := client.Post(url, "application/json", bytes.NewReader(b))
    if err != nil {
        return nil, err
    }
    defer r.Body.Close()

    if r.StatusCode != http.StatusOK {
        body, _ := io.ReadAll(r.Body)
        return nil, fmt.Errorf("status %d: %s", r.StatusCode, string(body))
    }

    var nr NLPResponse
    dec := json.NewDecoder(r.Body)
    if err := dec.Decode(&nr); err != nil {
        return nil, err
    }
    return &nr, nil
}

// localStubAnalyze provides a simple built-in analyzer so the module
// can run without an external service. Keep this deterministic and small.
func localStubAnalyze(text, mode string) string {
    // Very simple "analysis": count words and echo mode.
    words := 0
    inWord := false
    for _, r := range text {
        if r == ' ' || r == '\n' || r == '\t' || r == '\r' {
            if inWord {
                words++
                inWord = false
            }
            continue
        }
        inWord = true
    }
    if inWord {
        words++
    }
    return fmt.Sprintf("mode=%s words=%d preview=%q", mode, words, preview(text, 80))
}

func preview(s string, n int) string {
    if len(s) <= n {
        return s
    }
    return s[:n] + "..."
}
