
use std::fmt::Display;
use libc::c_int;
use anyhow::Error as AnyErr;

mod request_id;
mod principal;

/************************************************************************/
/*UTILS : FROM AGENT_UNITY*/
/************************************************************************/

// Return for FFI functions
#[repr(i32)]
pub enum ResultCode {
    /// Ok
    Ok = 0,
    /// Error
    Err = -1,
}

/// A callback used to give the unsized value to caller.
type UnsizedCallBack<T> = extern "C" fn(*const T, c_int);

/// TODO: Use macro to abstract the same parts from the different functions for reducing duplicated code.
fn ret_unsized<T>(unsized_cb: UnsizedCallBack<T>, s: impl AsRef<[T]>) {
    let arr = s.as_ref();
    let len = arr.len() as c_int;

    unsized_cb(arr.as_ptr(), len);
}

// Return combined result
fn return_combined_result<T, E, A>(
    ret_cb: UnsizedCallBack<A>,
    err_cb: UnsizedCallBack<u8>,
    r: Result<T, E>,
) -> ResultCode
where
    T: AsRef<[A]>,
    E: Display,
{
    match r {
        Ok(v) => {
            ret_unsized(ret_cb, v);

            ResultCode::Ok
        }
        Err(e) => {
            ret_unsized(err_cb, e.to_string() + "\0");

            ResultCode::Err
        }
    }
}
