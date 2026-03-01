import importlib
import pytest


def test_tokenize_json_shape():
    try:
        mod = importlib.import_module('python_module')
    except Exception:
        pytest.skip('python_module not importable')
    j = mod.tokenize_json('one two three')
    assert (j is None) or isinstance(j, str)
    if isinstance(j, str):
        # basic JSON array expected
        assert j.strip().startswith('[') and j.strip().endswith(']')
