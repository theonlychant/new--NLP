package projecttests

import (
    "io/ioutil"
    "testing"
)

func TestReadRootReadme(t *testing.T) {
    b, err := ioutil.ReadFile("../README.md")
    if err != nil {
        t.Skipf("README.md not readable: %v", err)
    }
    if len(b) == 0 {
        t.Fatalf("README.md appears empty")
    }
}
