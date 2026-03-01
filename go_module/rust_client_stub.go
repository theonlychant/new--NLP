//go:build !rust
// +build !rust

package main

// callRustCompute is a stub used when the real Rust library isn't built.
func callRustCompute(s string) int {
    return 0
}
