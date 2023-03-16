use std::ffi::CStr;
use ic_types::principal::Principal;
use libc::{c_char, c_int};
use crate::{UnsizedCallBack, AnyErr, ResultCode, return_combined_result};

#[no_mangle]
pub extern "C" fn principal_management_canister() -> *mut Principal {
    Box::into_raw(Box::new(Principal::management_canister()))
}

//ALTERNATIVE WITH HELP FUCNTION IS IT SAFER ?
// #[no_mangle]
// pub extern "C" fn principal_management_canister_1(ret_cb: UnsizedCallBack<u8>) {
//     ret_unsized(ret_cb, Principal::management_canister());
// }

#[no_mangle]
pub extern "C" fn principal_self_authenticating(
    public_key: *const u8,
    public_key_len: c_int
) -> *mut Principal {
    let public_key = unsafe { std::slice::from_raw_parts(public_key, public_key_len as usize) };

    Box::into_raw(Box::new(Principal::self_authenticating(public_key)))
}

#[no_mangle]
pub extern "C" fn principal_anonymous() -> *mut Principal {
    Box::into_raw(Box::new(Principal::anonymous()))
}

#[no_mangle]
pub extern "C" fn principal_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
) -> *mut Principal {
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };

    Box::into_raw(Box::new(Principal::from_slice(slice)))
}

#[no_mangle]
pub extern "C" fn principal_try_from_bytes(
    bytes: *const u8,
    bytes_len: c_int,
    ret_cb: UnsizedCallBack<u8>,
    err_cb: UnsizedCallBack<u8>,
) -> ResultCode {
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };

    let principal = Principal::try_from_slice(slice);

    return_combined_result(ret_cb, err_cb, principal)
}

#[no_mangle]
pub extern "C" fn principal_from_text(
    text: *const c_char,
    ret_cb: UnsizedCallBack<u8>,
    err_cb: UnsizedCallBack<u8>,
) -> ResultCode {
    let text = unsafe { CStr::from_ptr(text).to_str().map_err(AnyErr::from) };

    let principal = text.and_then(|text| Principal::from_text(text).map_err(AnyErr::from));

    return_combined_result(ret_cb, err_cb, principal)
}

// TODO : to_text example on agent-unity
