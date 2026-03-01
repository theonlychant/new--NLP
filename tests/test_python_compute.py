import importlib
import pytest


def test_compute_returns_int():
    try:
        mod = importlib.import_module('python_module')
    except Exception:
        pytest.skip('python_module not importable')
    res = mod.compute('one two three')
    assert isinstance(res, int)
    assert res == 3
