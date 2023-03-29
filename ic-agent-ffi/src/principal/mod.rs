use std::ffi::{CString, CStr};
use candid::Principal;
use libc::{c_char, c_int};
use crate::{AnyErr};

#[repr(C)]
pub struct PrincipalWrapper {
    bytes: [u8; 32],
}

/// Principal wrapper
#[repr(C)]
pub struct PrincipalResult {
    principal: *mut PrincipalWrapper,
    error: *mut c_char,
}

impl PrincipalResult {
    fn new(principal: *mut PrincipalWrapper, error: *mut c_char) -> Self {
        Self { principal, error }
    }
}

/// Construct a Principal of the IC management canister
#[no_mangle]
pub extern "C" fn principal_management_canister() -> *mut PrincipalWrapper {
    let principal = Principal::management_canister();
    let mut bytes = [0u8; 32];
    bytes.copy_from_slice(principal.as_slice());
    let wrapper = PrincipalWrapper { bytes };
    let boxed_wrapper = Box::new(wrapper);
    Box::into_raw(boxed_wrapper)
}

/// Construct a self-authenticating ID from public key
#[no_mangle]
pub extern "C" fn principal_self_authenticating(
    public_key: *const u8,
    public_key_len: c_int,
) -> *mut PrincipalWrapper {
    let public_key = unsafe { std::slice::from_raw_parts(public_key, public_key_len as usize) };
    let principal = Principal::self_authenticating(public_key);
    let mut bytes = [0u8; 32];
    bytes.copy_from_slice(principal.as_slice());
    let wrapper = PrincipalWrapper { bytes };
    let boxed_wrapper = Box::new(wrapper);
    Box::into_raw(boxed_wrapper)
}

/// Construct an anonymous ID
#[no_mangle]
pub extern "C" fn principal_anonymous() -> *mut PrincipalWrapper {
    let principal = Principal::anonymous();
    let mut bytes = [0u8; 32];
    bytes.copy_from_slice(principal.as_slice());
    let wrapper = PrincipalWrapper { bytes };
    let boxed_wrapper = Box::new(wrapper);
    Box::into_raw(boxed_wrapper)
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
) -> *mut PrincipalWrapper{
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal = Principal::from_slice(slice);
    let mut bytes = [0u8; 32];
    bytes.copy_from_slice(principal.as_slice());
    let wrapper = PrincipalWrapper { bytes };
    let boxed_wrapper = Box::new(wrapper);
    Box::into_raw(boxed_wrapper)
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_try_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
) -> PrincipalResult {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    match Principal::try_from_slice(slice) {
        Ok(principal) => {
            let mut bytes = [0u8; 32];
            bytes.copy_from_slice(principal.as_slice());
            let wrapper = PrincipalWrapper { bytes };
            PrincipalResult::new(Box::into_raw(Box::new(wrapper)), std::ptr::null_mut())
        }
        Err(e) => {
            let error = CString::new(e.to_string() + "\0").unwrap();
            PrincipalResult::new(std::ptr::null_mut(), error.into_raw())
        }
    }
}

/// Parse a Principal from text representation.
#[no_mangle]
pub extern "C" fn principal_from_text(
    text: *const c_char,
) -> PrincipalResult {

    let text_cstr = unsafe {
        assert!(!text.is_null());
        CStr::from_ptr(text)
    };
    let text_str = text_cstr.to_str().unwrap();

    match Principal::from_text(text_str) {
        Ok(principal) => {
            let mut bytes = [0u8; 32];
            bytes.copy_from_slice(principal.as_slice());
            let wrapper = PrincipalWrapper { bytes };
            PrincipalResult::new(Box::into_raw(Box::new(wrapper)), std::ptr::null_mut())
        }
        Err(e) => {
            let error = CString::new(e.to_string() + "\0").unwrap();
            PrincipalResult::new(std::ptr::null_mut(), error.into_raw())
        }
    }
}

/// Return the textual representation of Principal.
#[no_mangle]
pub extern "C" fn principal_to_text(
    bytes: *const u8,
    bytes_len: c_int,
) -> PrincipalResult {

    // In C Principal will be just a chunk of memory so compute principal struture here in rust
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal = Principal::try_from_slice(slice).map_err(AnyErr::from);

    let text = principal
        .map(|principal| principal.to_text())
        .and_then(|text| CString::new(text).map_err(AnyErr::from))
        .map(|text| text.into_bytes_with_nul());

    match text {
        Ok(principal) => {
            let mut bytes = [0u8; 32];
            bytes.copy_from_slice(principal.as_slice());
            let wrapper = PrincipalWrapper { bytes };
            PrincipalResult::new(Box::into_raw(Box::new(wrapper)), std::ptr::null_mut())
        }
        Err(e) => {
            let error = CString::new(e.to_string() + "\0").unwrap();
            PrincipalResult::new(std::ptr::null_mut(), error.into_raw())
        }
    }

}

#[no_mangle]
pub extern "C" fn principal_free(wrapper: *mut PrincipalWrapper) {
    if !wrapper.is_null() {
        unsafe {
            drop(Box::from_raw(wrapper));
        }
    }
}

#[no_mangle]
pub extern "C" fn principal_result_free(result: PrincipalResult) {
    unsafe {
        if !result.principal.is_null() {
            drop(Box::from_raw(result.principal));
        }
        if !result.error.is_null() {
            drop(CString::from_raw(result.error));
        }
    }
}

// TODO : TESTING
