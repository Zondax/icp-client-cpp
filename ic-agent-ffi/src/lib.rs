
use std::{fmt::Display};
use libc::c_int;
use anyhow::Error as AnyErr;

mod request_id;
mod principal;
mod identity;
//mod hash_tree;

/// Return for FFI functions
#[repr(i32)]
pub enum ResultCode {
    /// Ok
    Ok = 0,
    /// Error
    Err = -1,
}

/// A callback used to give the unsized value to caller.
type PtrCallBack<T> = extern "C" fn(*const T, c_int);

fn return_unsized_ptr<T>(fn_callback: PtrCallBack<T>, s: impl AsRef<[T]>) {
    let arr = s.as_ref();
    let len = arr.len() as c_int;

    fn_callback(arr.as_ptr(), len);
}

// Get pointer to result and error
fn return_result<T, E, A>(
    return_callback: PtrCallBack<A>,
    error_callback: PtrCallBack<u8>,
    r: Result<T, E>,
) -> ResultCode
where
    T: AsRef<[A]>,
    E: Display,
{
    match r {
        Ok(v) => {
            return_unsized_ptr(return_callback, v);

            ResultCode::Ok
        }
        Err(e) => {
            return_unsized_ptr(error_callback, e.to_string() + "\0");

            ResultCode::Err
        }
    }
}
