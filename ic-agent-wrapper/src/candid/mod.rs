use std::ffi::c_int;
use candid::IDLArgs;
use libc::c_void;
use crate::RetPtr;

#[no_mangle]
pub extern "C" fn idl_args_to_text(idl_args: *const c_void, ret_cb: RetPtr<u8>) {

    let boxed = unsafe { Box::from_raw(idl_args as *mut IDLArgs) };
    let idl_str = boxed.to_string() + "\0";

    let arr = idl_str.as_bytes();
    let len = arr.len() as c_int;

    ret_cb(arr.as_ptr(), len);
}
