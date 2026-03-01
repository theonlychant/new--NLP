rust_module — build & test
=================================

This folder contains a small Rust crate (`rustlib`) exposing a C ABI for
NLP helper routines and a C fallback stub (`c_stub.c`) so examples can run
without a Rust toolchain.

Quick actions
-------------

- Build using Cargo (recommended when Rust is available):

```bash
cd rust_module
cargo build --release
```

- Build fallback C stub (no Rust required):

```bash
cd rust_module
make build-cstub
# or
./build.sh
```

The shared library will be placed in `rust_module/target/` as
`librustlib.so` (or platform equivalent on Windows).

Using from Go or Python
------------------------
- For Go (cgo), pass `-tags=rust` at build time to link the real library,
  or use the `-stub` flag in `go_module` which calls the C stub.
- For Python, set `LD_LIBRARY_PATH` to include `rust_module/target` or copy
  `librustlib.so` into the Python module folder. The `python_module` has a
  ctypes wrapper that looks for `librustlib.so` in `rust_module/target`.

CI and automation
------------------
A simple GitHub Actions workflow is provided at `.github/workflows/ci.yml`
to build and test the Rust crate and run the Go/Python smoke tests.

Notes & next steps
------------------
- The C stub provides practical fallbacks but isn't a full implementation
  of every function declared in `include/rustlib.h` — fill in the functions
  you need most or implement them in Rust.
- The `Makefile` and `build.sh` provide convenient entry points for local
  development.
