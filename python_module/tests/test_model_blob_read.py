from pathlib import Path
import pytest


def test_model_blob_readable():
    blob = Path(__file__).resolve().parents[2] / "go_module" / "model.bin"
    assert blob.exists(), "model.bin should exist in go_module"
    content = blob.read_bytes()
    assert len(content) > 0
