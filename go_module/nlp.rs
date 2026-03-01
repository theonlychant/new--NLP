// Simple Rust tokenizer and C ABI wrapper for testing
// This file is a textual Rust example placed inside `go_module` for test/demo purposes.

// Count words in a UTF-8 byte slice. Exposed as C ABI for interop tests.
#[no_mangle]
pub extern "C" fn rust_tokenize_word_count(ptr: *const u8, len: usize) -> usize {
    if ptr.is_null() || len == 0 {
        return 0;
    }
    let slice = unsafe { std::slice::from_raw_parts(ptr, len) };
    match std::str::from_utf8(slice) {
        Ok(s) => s.split_whitespace().count(),
        Err(_) => 0,
    }
}

// Local test entry point (not used by build, present for reference).
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn count_words() {
        let s = "hello from rust tokenizer".as_bytes();
        let n = rust_tokenize_word_count(s.as_ptr(), s.len());
        assert_eq!(n, 4);
    }
}
