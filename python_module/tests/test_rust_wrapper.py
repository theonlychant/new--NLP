import importlib
from pathlib import Path
import pytest


def _lib_exists():
    lib = Path(__file__).resolve().parents[2] / "rust_module" / "target" / "release" / "librustlib.so"
    return lib.exists()


def test_compute_basic():
    try:
        rw = importlib.import_module("python_module.rust_wrapper")
    except OSError:
        pytest.skip("rust shared lib not built")
    assert isinstance(rw.compute("hello world"), int)


def test_compute_unicode():
    try:
        rw = importlib.import_module("python_module.rust_wrapper")
    except OSError:
        pytest.skip("rust shared lib not built")
    # Should return a numeric word count
    assert rw.compute("こんにちは 世界") == 2


def test_model_blob_present():
    blob = Path(__file__).resolve().parents[2] / "go_module" / "model.bin"
    assert blob.exists()
    assert blob.stat().st_size > 0
