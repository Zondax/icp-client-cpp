use std::{ffi::c_int};
use anyhow::Error as AnyErr;
use anyhow::Result as AnyResult;

mod request_id;
mod principal;
mod identity;
mod agent;
mod candid;

/// Ptr creation with size and len
type RetPtr<T> = extern "C" fn(*const T, c_int);

/// Return for FFI functions
#[repr(i32)]
#[derive(Debug, Eq, PartialEq, Copy, Clone, Hash)]
pub enum ResultCode {
    /// Ok
    Ok = 0,
    /// Error
    Err = -1,
}
