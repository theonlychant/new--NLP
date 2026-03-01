"""python_module
===============
Lightweight package wrapper exposing the small Rust-backed NLP helpers.

This package provides:
- `compute(text)` -> int
- `analyze(text)` -> Optional[str]
- `tokenize_json(text)` -> Optional[str]

It delegates to `rust_wrapper` which locates the shared library (or falls
back to C stubs when available).
"""
from .rust_wrapper import compute, analyze, tokenize_json

__all__ = ["compute", "analyze", "tokenize_json"]
