use std::ffi::{CStr, CString};
use candid::Principal;
use libc::{c_char, c_int};
use crate::{PtrCallBack, AnyErr, ResultCode, return_combined_result, ret_unsized_ptr};

/// Construct a Principal of the IC management canister
#[no_mangle]
pub extern "C" fn principal_management_canister() -> *mut Principal {
    Box::into_raw(Box::new(Principal::management_canister()))
}

/// Construct a self-authenticating ID from public key
#[no_mangle]
pub extern "C" fn principal_self_authenticating(
    public_key: *const u8,
    public_key_len: c_int,
    principal_ret: PtrCallBack<u8>,
){
    let public_key = unsafe { std::slice::from_raw_parts(public_key, public_key_len as usize) };
    ret_unsized_ptr(principal_ret, Principal::self_authenticating(public_key));
}

/// Construct an anonymous ID
#[no_mangle]
pub extern "C" fn principal_anonymous(principal_ret: PtrCallBack<u8>){
    ret_unsized_ptr(principal_ret, Principal::anonymous());
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
    principal_ret: PtrCallBack<u8>,
) {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    ret_unsized_ptr(principal_ret, Principal::from_slice(slice));
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_try_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
    ret_cb: PtrCallBack<u8>,
    err_cb: PtrCallBack<u8>,
) -> ResultCode {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    return_combined_result(ret_cb, err_cb, Principal::try_from_slice(slice))
}

/// Parse a Principal from text representation.
#[no_mangle]
pub extern "C" fn principal_from_text(
    text: *const c_char,
    ret_cb: PtrCallBack<u8>,
    err_cb: PtrCallBack<u8>,
) -> ResultCode {
    let text = unsafe { CStr::from_ptr(text).to_str().map_err(AnyErr::from) };

    let principal = text.and_then(|text| Principal::from_text(text).map_err(AnyErr::from));

    return_combined_result(ret_cb, err_cb, principal)
}

/// Return the textual representation of Principal.
#[no_mangle]
pub extern "C" fn principal_to_text(
    ptr: *mut Principal,
    ret_cb: PtrCallBack<u8>,
    err_cb: PtrCallBack<u8>,
) -> ResultCode {

    let mpc = unsafe {
        assert!(!ptr.is_null());
        &mut *ptr
    };

    // In C Principal will be just a chunk of memory so compute principal struture here in rust
    //let slice = unsafe { std::slice::from_raw_parts(mpc, bytes_len as usize) };
    let principal = Principal::try_from_slice(mpc.as_slice()).map_err(AnyErr::from);
    
    let text = principal
        .map(|principal| principal.to_text())
        .and_then(|text| CString::new(text).map_err(AnyErr::from))
        .map(|text| text.into_bytes_with_nul());

    return_combined_result(ret_cb, err_cb, text)
}

// TODO : TESTING
