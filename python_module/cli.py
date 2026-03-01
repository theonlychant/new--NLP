"""Simple CLI demo for `python_module` to exercise the wrapper functions."""
import argparse
from . import compute, analyze, tokenize_json


def main():
    p = argparse.ArgumentParser(description="python_module demo")
    p.add_argument("text", nargs="*", default=["hello from python"])
    args = p.parse_args()
    txt = " ".join(args.text)
    try:
        n = compute(txt)
        print(f"compute -> {n}")
    except Exception as e:
        print("compute -> (error)", e)
    try:
        a = analyze(txt)
        print("analyze ->", a)
    except Exception as e:
        print("analyze -> (error)", e)
    try:
        j = tokenize_json(txt)
        print("tokenize_json ->", j)
    except Exception as e:
        print("tokenize_json -> (error)", e)


if __name__ == "__main__":
    main()
