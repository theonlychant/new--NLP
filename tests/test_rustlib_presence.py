import importlib
import pytest
from pathlib import Path


def test_rustlib_shared_exists_or_skip():
    p = Path(__file__).resolve().parents[1] / 'rust_module' / 'target' / 'librustlib.so'
    if not p.exists():
        pytest.skip('librustlib.so not built')
    assert p.stat().st_size > 0


