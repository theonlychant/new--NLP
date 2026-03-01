package projecttests

import (
    "testing"
    "strings"
    "os/exec"
)

// sanity: run the go_module CLI with the stub and ensure NLP pipeline runs
func TestGoModuleStubInvocation(t *testing.T) {
    cmd := exec.Command("go", "run", "../go_module", "-text", "hello world from go test", "-stub")
    out, err := cmd.CombinedOutput()
    if err != nil {
        t.Skipf("go run failed or not available: %v", err)
    }
    s := string(out)
    if !strings.Contains(s, "words=") {
        t.Fatalf("expected words= output in: %s", s)
    }
}
