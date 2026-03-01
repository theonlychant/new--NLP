package projecttests

import (
    "os"
    "testing"
)

func TestRustLibPath(t *testing.T) {
    if _, err := os.Stat("../rust_module/target/librustlib.so"); os.IsNotExist(err) {
        t.Skip("rust shared lib not present")
    }
}
