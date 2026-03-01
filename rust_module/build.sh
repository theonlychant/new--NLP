#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
TARGET_DIR="$ROOT_DIR/target"
mkdir -p "$TARGET_DIR"

if command -v cargo >/dev/null 2>&1; then
  echo "Building rust crate via cargo..."
  (cd "$ROOT_DIR" && cargo build --release)
  # copy any produced shared lib into target for consumers
  cp -v "$ROOT_DIR"/target/release/librustlib.* "$TARGET_DIR/" 2>/dev/null || true
  echo "Rust build finished. Shared libs (if any) copied to $TARGET_DIR"
else
  echo "cargo not found; building fallback C stub into shared library"
  gcc -shared -fPIC "$ROOT_DIR/c_stub.c" -I"$ROOT_DIR/include" -o "$TARGET_DIR/librustlib.so"
  echo "Built $TARGET_DIR/librustlib.so"
fi

echo "Done. Place $TARGET_DIR in LD_LIBRARY_PATH or copy the .so into your runtime path."
