/*
 * rust_driver.h
 * Compatibility helpers for using the rustlib from Python (ctypes) and Go (cgo).
 * Provides additional typedefs and convenience macros that make binding generation
 * and dynamic loading simpler.
 */

#ifndef RUST_DRIVER_H
#define RUST_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "rustlib.h"

/* Python ctypes helpers:
 * - Functions that return heap-allocated strings use `rust_free_cstring` to free.
 * - _py_retchar allocates via libc-compatible allocator so ctypes or Python can free it.
 */
RUSTLIB_EXPORT char* _py_retchar(const char* s);

/* Go cgo helpers:
 * - Provide simple wrappers with C linkage and plain types to make cgo declarations
 *   smaller and easier. Example: `int32_t compute_from_rust(const char*)` is already
 *   cgo-friendly; these wrappers are optional.
 */
RUSTLIB_EXPORT int32_t go_compute_from_rust(const char* s);

/* Utility: return version string (owned by library) */
RUSTLIB_EXPORT const char* rustlib_version(void);

#ifdef __cplusplus
}
#endif

#endif // RUST_DRIVER_H