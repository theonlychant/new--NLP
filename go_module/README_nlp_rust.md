Rust test artifacts in go_module

These files are examples and placeholders placed directly in `go_module` for testing:

- `nlp.rs` — simple Rust tokenizer exposing a C ABI `rust_tokenize_word_count`.
- `model_blob.rs` — small Rust byte array (`MODEL_BLOB`) simulating an embedded model.
- `model.bin` — textual placeholder representing a compact binary payload.

Notes:
- These files are provided as source/text artifacts only. Use them as references or copy them
  into a proper `rust_module` Cargo project when you want to build a real shared library.
- To compile a real Rust library and use it from Go, create a Cargo project and link the
  produced shared object (`librustlib.so`) or use `cbindgen`/cgo wrappers.

Example (outside `go_module`):

```bash
# from repo root
cd rust_module
cargo build --release
cd ../go_module
export LD_LIBRARY_PATH=$PWD/../rust_module/target/release:$LD_LIBRARY_PATH
go run -tags=rust . -text "Hello from Go to Rust" -rust
```
