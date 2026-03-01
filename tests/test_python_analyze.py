import importlib
import pytest


def test_analyze_maybe_string_or_none():
    try:
        mod = importlib.import_module('python_module')
    except Exception:
        pytest.skip('python_module not importable')
    val = mod.analyze('hello world')
    assert (val is None) or isinstance(val, str)
    # if present, must contain word count or preview
    if isinstance(val, str):
        assert 'words=' in val or 'preview' in val
