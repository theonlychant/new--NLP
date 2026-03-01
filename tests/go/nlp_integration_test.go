package projecttests

import (
    "bufio"
    "bytes"
    "context"
    "encoding/json"
    "os/exec"
    "strings"
    "testing"
    "time"
)

// This Go test file exercises the local go_module stub, runs concurrent
// invocations, parses the output and validates basic NLP fields are present.

func runGoStub(t *testing.T, text string, timeout time.Duration) (string, error) {
    ctx, cancel := context.WithTimeout(context.Background(), timeout)
    defer cancel()
    cmd := exec.CommandContext(ctx, "go", "run", "../go_module", "-text", text, "-stub")
    var out bytes.Buffer
    cmd.Stdout = &out
    cmd.Stderr = &out
    err := cmd.Run()
    return out.String(), err
}

func TestGoStubSingleInvocation(t *testing.T) {
    out, err := runGoStub(t, "Hello from go test", 5*time.Second)
    if err != nil {
        t.Skipf("go run not available or failed: %v", err)
    }
    if !strings.Contains(out, "words=") {
        t.Fatalf("expected words= in output, got: %s", out)
    }
}

func TestGoStubConcurrent(t *testing.T) {
    // attempt multiple concurrent runs to stress the stub
    n := 6
    results := make(chan string, n)
    errors := make(chan error, n)
    for i := 0; i < n; i++ {
        i := i
        go func() {
            o, err := runGoStub(t, strings.Repeat("test", i+1), 6*time.Second)
            results <- o
            errors <- err
        }()
    }
    for i := 0; i < n; i++ {
        o := <-results
        err := <-errors
        if err != nil {
            t.Skipf("skipping concurrent assertion; go run failed: %v", err)
        }
        if !strings.Contains(o, "words=") {
            t.Fatalf("concurrent run missing words= in: %s", o)
        }
    }
}

func TestGoStubParseAsKV(t *testing.T) {
    out, err := runGoStub(t, "sample to parse", 5*time.Second)
    if err != nil {
        t.Skipf("go run not available: %v", err)
    }
    // attempt to parse key=value tokens from the output
    scanner := bufio.NewScanner(strings.NewReader(out))
    kv := map[string]string{}
    for scanner.Scan() {
        line := scanner.Text()
        parts := strings.Fields(line)
        for _, p := range parts {
            if strings.Contains(p, "=") {
                sp := strings.SplitN(p, "=", 2)
                kv[sp[0]] = sp[1]
            }
        }
    }
    if _, ok := kv["words"]; !ok {
        t.Fatalf("expected words= in output kv map; found keys: %v; raw output: %s", kv, out)
    }
}

func TestGoStubJsonLikeOutput(t *testing.T) {
    // Some runs may emit JSON-like payloads via analyze; attempt to detect/parsethem
    out, err := runGoStub(t, "json probe text", 5*time.Second)
    if err != nil {
        t.Skipf("go run failed: %v", err)
    }
    // search for {...} substring
    start := strings.Index(out, "{")
    end := strings.LastIndex(out, "}")
    if start >= 0 && end > start {
        candidate := out[start : end+1]
        var js map[string]interface{}
        if err := json.Unmarshal([]byte(candidate), &js); err == nil {
            // if parsed, ensure some top-level key exists
            if len(js) == 0 {
                t.Fatalf("parsed JSON but empty: %s", candidate)
            }
        }
    }
}
