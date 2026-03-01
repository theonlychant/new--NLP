import importlib
import pytest


def test_import_wrapper_or_skip():
    try:
        importlib.import_module("python_module.rust_wrapper")
    except OSError:
        pytest.skip("rust shared lib not built; skipping import test")
