"""ctypes wrapper for the rustlib C API.

Provides safe fallbacks for environments where the Rust shared library
is not in the standard release path (uses compiled C stub when present).

Functions exported:
- compute(text) -> int
- analyze(text) -> str
- tokenize_json(text) -> str
"""
from pathlib import Path
import ctypes
import ctypes.util
import sys
from typing import Optional


def _possible_lib_paths(root: Path):
    paths = []
    # typical cargo release
    paths.append(root / 'rust_module' / 'target' / 'release' / 'librustlib.so')
    # fallback build.sh / Makefile output
    paths.append(root / 'rust_module' / 'target' / 'librustlib.so')
    # debug build
    paths.append(root / 'rust_module' / 'target' / 'debug' / 'librustlib.so')
    # platform variations
    paths.append(root / 'rust_module' / 'target' / 'release' / 'librustlib.d')
    paths.append(root / 'rust_module' / 'target' / 'release' / 'librustlib.rlib')
    return paths


def _locate_lib():
    root = Path(__file__).resolve().parents[1]
    for p in _possible_lib_paths(root):
        if p.exists():
            return str(p)
    # try system lookup (if installed globally)
    name = ctypes.util.find_library('rustlib') or ctypes.util.find_library('librustlib')
    if name:
        return name
    raise OSError('librustlib shared library not found; build rust_module or run build.sh')


_lib_path = None
try:
    _lib_path = _locate_lib()
    _lib = ctypes.CDLL(_lib_path)
except OSError as e:
    # propagate to user when calling functions
    _lib = None


def _has(name: str) -> bool:
    return _lib is not None and hasattr(_lib, name)


def _ensure_lib():
    if _lib is None:
        raise OSError('librustlib not loaded; build rust_module/target or run rust_module/build.sh')


# Setup common function prototypes if present
if _lib is not None:
    if _has('go_compute_from_rust'):
        _lib.go_compute_from_rust.argtypes = [ctypes.c_char_p]
        _lib.go_compute_from_rust.restype = ctypes.c_int
    if _has('compute_from_rust'):
        _lib.compute_from_rust.argtypes = [ctypes.c_char_p]
        _lib.compute_from_rust.restype = ctypes.c_int
    if _has('rust_analyze_alloc'):
        _lib.rust_analyze_alloc.argtypes = [ctypes.c_char_p]
        _lib.rust_analyze_alloc.restype = ctypes.c_char_p
    if _has('rust_tokenize_json'):
        _lib.rust_tokenize_json.argtypes = [ctypes.c_char_p, ctypes.c_void_p]
        _lib.rust_tokenize_json.restype = ctypes.c_char_p
    if _has('rust_free_cstring'):
        _lib.rust_free_cstring.argtypes = [ctypes.c_char_p]
        _lib.rust_free_cstring.restype = None
    if _has('rust_model_infer_json'):
        _lib.rust_model_infer_json.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        _lib.rust_model_infer_json.restype = ctypes.c_char_p
    if _has('rust_embed_alloc'):
        # rust_embed_alloc(const char* s, rust_vec_t** out)
        _lib.rust_embed_alloc.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
        _lib.rust_embed_alloc.restype = ctypes.c_int
    if _has('rust_vec_free'):
        _lib.rust_vec_free.argtypes = [ctypes.c_void_p]
        _lib.rust_vec_free.restype = None
    if _has('rust_batch_embed_alloc'):
        # rust_batch_embed_alloc(const char** inputs, size_t n, float** out_buf, size_t* out_dim)
        _lib.rust_batch_embed_alloc.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.c_size_t, ctypes.POINTER(ctypes.POINTER(ctypes.c_float)), ctypes.POINTER(ctypes.c_size_t)]
        _lib.rust_batch_embed_alloc.restype = ctypes.c_int
    if _has('rust_free_float_array'):
        _lib.rust_free_float_array.argtypes = [ctypes.POINTER(ctypes.c_float)]
        _lib.rust_free_float_array.restype = None


def compute(text: str) -> int:
    """Return an integer compute result (word count by default)."""
    if _has('go_compute_from_rust'):
        return _lib.go_compute_from_rust(text.encode('utf-8'))
    if _has('compute_from_rust'):
        return _lib.compute_from_rust(text.encode('utf-8'))
    raise OSError('compute function not available in loaded library')


def analyze(text: str) -> Optional[str]:
    """Return human-readable analysis string allocated by the library.

    Caller does not need to free; wrapper will free the returned C string.
    """
    _ensure_lib()
    if not _has('rust_analyze_alloc'):
        return None
    p = _lib.rust_analyze_alloc(text.encode('utf-8'))
    if not p:
        return None
    try:
        s = ctypes.cast(p, ctypes.c_char_p).value.decode('utf-8')
        return s
    finally:
        if _has('rust_free_cstring'):
            _lib.rust_free_cstring(p)


def tokenize_json(text: str) -> Optional[str]:
    _ensure_lib()
    if not _has('rust_tokenize_json'):
        return None
    p = _lib.rust_tokenize_json(text.encode('utf-8'), None)
    if not p:
        return None
    try:
        return ctypes.cast(p, ctypes.c_char_p).value.decode('utf-8')
    finally:
        if _has('rust_free_cstring'):
            _lib.rust_free_cstring(p)


def embed(text: str):
    """Return a simple embedding vector (list of floats) for `text`.

    Uses `rust_embed_alloc` if available, otherwise falls back to a toy vector.
    """
    if _has('rust_embed_alloc'):
        out_ptr = ctypes.c_void_p()
        rc = _lib.rust_embed_alloc(text.encode('utf-8'), ctypes.byref(out_ptr))
        if rc != 0 or not out_ptr:
            # fallback
            pass
        else:
            # rust_vec_t { size_t dim; float* data; }
            # read dim from memory: treat pointer as struct (two pointers/size)
            # We'll assume layout: size_t then pointer
            ptr = out_ptr.value
            if ptr:
                # read dim (first size_t)
                dim = ctypes.c_size_t.from_address(ptr).value
                data_ptr_addr = ptr + ctypes.sizeof(ctypes.c_size_t)
                data_ptr = ctypes.POINTER(ctypes.c_float).from_address(data_ptr_addr).value
                # construct array
                arr_type = ctypes.c_float * dim
                arr = ctypes.cast(data_ptr, ctypes.POINTER(arr_type)).contents
                res = [float(x) for x in arr]
                if _has('rust_vec_free'):
                    _lib.rust_vec_free(out_ptr)
                return res
    # fallback simple vector: [word_count, length, sum]
    wc = len([w for w in text.split() if w])
    l = len(text)
    return [float(wc), float(l), float(wc + l)]


def batch_embed(texts):
    """Return list of embedding lists for inputs. Uses rust_batch_embed_alloc when available."""
    if not isinstance(texts, (list, tuple)):
        texts = [texts]
    n = len(texts)
    if n == 0:
        return []
    if _has('rust_batch_embed_alloc'):
        c_inputs = (ctypes.c_char_p * n)(*([t.encode('utf-8') for t in texts]))
        out_buf = ctypes.POINTER(ctypes.c_float)()
        out_dim = ctypes.c_size_t()
        rc = _lib.rust_batch_embed_alloc(c_inputs, ctypes.c_size_t(n), ctypes.byref(out_buf), ctypes.byref(out_dim))
        if rc == 0 and out_buf and out_dim.value > 0:
            dim = out_dim.value
            total = n * dim
            arr_type = ctypes.c_float * total
            arr = ctypes.cast(out_buf, ctypes.POINTER(arr_type)).contents
            res = []
            for i in range(n):
                start = i * dim
                res.append([float(x) for x in arr[start:start+dim]])
            if _has('rust_free_float_array'):
                _lib.rust_free_float_array(out_buf)
            return res
    # fallback
    return [embed(t) for t in texts]


if __name__ == '__main__':
    txt = ' '.join(sys.argv[1:]) if len(sys.argv) > 1 else 'hello from python'
    try:
        print('compute ->', compute(txt))
    except OSError as e:
        print('compute not available:', e)
    try:
        a = analyze(txt)
        print('analyze ->', a)
    except OSError:
        print('analyze not available')
