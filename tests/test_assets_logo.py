from pathlib import Path


def test_logo_exists():
    p = Path(__file__).resolve().parents[1] / 'assets' / 'logo_chant.svg'
    assert p.exists(), f"logo not found at {p}"
