
use std::{fmt::Display};
use libc::c_int;
use anyhow::Error as AnyErr;

mod request_id;
mod principal;
//mod identity;
//mod hash_tree;

/************************************************************************/
/*UTILS : Based on AGENT_UNITY*/
/************************************************************************/

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

fn ret_unsized_ptr<T>(unsized_cb: PtrCallBack<T>, s: impl AsRef<[T]>) {
    let arr = s.as_ref();
    let len = arr.len() as c_int;

    unsized_cb(arr.as_ptr(), len);
}

// Get pointer to result and error
fn return_combined_result<T, E, A>(
    ret_cb: PtrCallBack<A>,
    err_cb: PtrCallBack<u8>,
    r: Result<T, E>,
) -> ResultCode
where
    T: AsRef<[A]>,
    E: Display,
{
    match r {
        Ok(v) => {
            ret_unsized_ptr(ret_cb, v);

            ResultCode::Ok
        }
        Err(e) => {
            ret_unsized_ptr(err_cb, e.to_string() + "\0");

            ResultCode::Err
        }
    }
}
