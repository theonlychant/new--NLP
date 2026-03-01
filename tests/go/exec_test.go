package projecttests

import (
    "os/exec"
    "testing"
)

func TestGoToolchainAvailable(t *testing.T) {
    if _, err := exec.LookPath("go"); err != nil {
        t.Skip("go toolchain not available")
    }
    if _, err := exec.Command("go", "version").Output(); err != nil {
        t.Fatalf("go version failed: %v", err)
    }
}
