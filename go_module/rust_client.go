//go:build rust
// +build rust

package main

// #cgo LDFLAGS: -L${SRCDIR}/../rust_module/target/release -lrustlib
// #include <stdint.h>
// #include <stdlib.h>
// int32_t compute_from_rust(const char* s);
import "C"

import (
    "unsafe"
)

func callRustCompute(s string) int {
    cs := C.CString(s)
    defer C.free(unsafe.Pointer(cs))
    res := C.compute_from_rust(cs)
    return int(res)
}
