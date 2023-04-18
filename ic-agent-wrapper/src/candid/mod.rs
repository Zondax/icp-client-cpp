/*******************************************************************************
*   (c) 2018 - 2022 Zondax AG
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
