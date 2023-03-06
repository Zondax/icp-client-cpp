/*******************************************************************************
*   (c) 2018 - 2023 Zondax AG
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
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
