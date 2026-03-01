use std::ffi::CStr;
use std::os::raw::{c_char, c_int};

#[no_mangle]
pub extern "C" fn compute_from_rust(s: *const c_char) -> c_int {
    if s.is_null() {
        return 0;
    }
    let c_str = unsafe { CStr::from_ptr(s) };
    match c_str.to_str() {
        Ok(text) => {
            let words = text.split_whitespace().count() as c_int;
            words
        }
        Err(_) => 0,
    }
}
