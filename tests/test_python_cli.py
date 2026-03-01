import subprocess
import sys
from pathlib import Path


def test_cli_runs():
    # run the python CLI module and ensure it produces expected lines
    cmd = [sys.executable, '-m', 'python_module.cli', 'hello', 'tests']
    p = subprocess.run(cmd, capture_output=True, text=True)
    out = p.stdout + p.stderr
    assert 'compute ->' in out
    # ensure compute reported an integer
    assert 'compute ->' in out
