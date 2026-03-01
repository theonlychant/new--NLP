use std::ffi::CString;

// Integration tests for the cdylib API exposed by the crate.
// These call the public `compute_from_rust` function using CStrings
// to exercise the FFI entrypoint with various inputs.

#[test]
fn test_ascii_word_count() {
    let s = CString::new("hello from rust").unwrap();
    let n = unsafe { rustlib::compute_from_rust(s.as_ptr()) };
    assert_eq!(n, 3);
}

#[test]
fn test_unicode_word_count() {
    // Japanese + English separated by space -> 2
    let s = CString::new("こんにちは 世界").unwrap();
    let n = unsafe { rustlib::compute_from_rust(s.as_ptr()) };
    assert_eq!(n, 2);
}

#[test]
fn test_empty_input() {
    let s = CString::new("").unwrap();
    let n = unsafe { rustlib::compute_from_rust(s.as_ptr()) };
    assert_eq!(n, 0);
}
