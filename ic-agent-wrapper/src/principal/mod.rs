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
use crate::RetError;
use candid::Principal;
use std::ffi::{c_char, c_int, CStr, CString};

#[repr(C)]
pub struct CPrincipal {
    pub ptr: *mut u8,
    pub len: usize,
}

/// @brief Construct a Principal of the IC management canister
///
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn principal_management_canister() -> Option<Box<CPrincipal>> {
    let principal = Principal::management_canister();
    let arr = principal.as_ref();
    let len = arr.len();
    let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
    let c_principal = Box::new(CPrincipal { ptr, len });
    Some(c_principal)
}

/// @brief Construct a self-authenticating ID from public key
///
/// @param public_key Pointer to array that holds public key
/// @param public_key_len Length of public_key
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn principal_self_authenticating(
    public_key: *const u8,
    public_key_len: c_int,
) -> Option<Box<CPrincipal>> {
    let public_key = unsafe { std::slice::from_raw_parts(public_key, public_key_len as usize) };
    let principal = Principal::self_authenticating(public_key);

    let arr = principal.as_ref();
    let len = arr.len();
    let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
    let c_principal = Box::new(CPrincipal { ptr, len });
    Some(c_principal)
}

/// @brief Construct an anonymous ID
///
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn principal_anonymous() -> Option<Box<CPrincipal>> {
    // Create an anonymous principal
    let principal = Principal::anonymous();
    let arr = principal.as_ref();
    let len = arr.len();
    let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
    let c_principal = Box::new(CPrincipal { ptr, len });
    Some(c_principal)
}

/// @brief Construct a Principal from a array of bytes.
///
/// @param bytes Pointer to array of bytes
/// @param bytes_len Length of array of bytes
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn principal_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
) -> Option<Box<CPrincipal>> {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal = Principal::from_slice(slice);
    let arr = principal.as_ref();
    let len = arr.len();
    let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
    let c_principal = Box::new(CPrincipal { ptr, len });
    Some(c_principal)
}

/// @brief Try to Construct a Principal from a array of bytes.
///
/// @param bytes Pointer to array of bytes
/// @param bytes_len Length of array of bytes
/// @param error_ret CallBack to get error
/// @return Pointer to CPrincipal structure
/// If the function returns a NULL CPrincipal the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn principal_try_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CPrincipal>> {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal_tmp = Principal::try_from_slice(slice);

    match principal_tmp {
        Ok(principal) => {
            let arr = principal.as_ref();
            let len = arr.len();
            let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
            let c_principal = Box::new(CPrincipal { ptr, len });
            Some(c_principal)
        }

        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(c_string.as_ptr() as _, c_string.as_bytes().len() as _, error_ret.user_data);
            }
            None
        }
    }
}

/// @brief Construct a Principal from text representation.
///
/// @param text Pointer to text representation
/// @param error_ret CallBack to get error
/// @return Pointer to CPrincipal structure
/// If the function returns a NULL CPrincipal the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn principal_from_text(
    text: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CPrincipal>> {
    let text_cstr = unsafe {
        assert!(!text.is_null());
        CStr::from_ptr(text)
    };
    let text_str = text_cstr.to_str().unwrap();

    let principal_tmp = Principal::from_text(text_str);
    match principal_tmp {
        Ok(principal) => {
            let arr = principal.as_ref();
            let len = arr.len();
            let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
            let c_principal = Box::new(CPrincipal { ptr, len });
            Some(c_principal)
        }

        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(c_string.as_ptr() as _, c_string.as_bytes().len() as _, error_ret.user_data);
            }
            None
        }
    }
}

/// @brief Return the textual representation of Principal.
///
/// @param bytes Principal in bytes
/// @param bytes_len Length of array of bytes
/// @param error_ret CallBack to get error
/// @return Pointer to CPrincipal structure will hold the text
/// If the function returns a NULL CPrincipal the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn principal_to_text(
    bytes: *const u8,
    bytes_len: c_int,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CPrincipal>> {
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal_tmp = Principal::try_from_slice(slice);

    match principal_tmp {
        Ok(principal) => {
            let arr = CString::new(principal.to_text()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            let len = arr.as_bytes().len();
            let ptr = arr.into_raw() as *mut u8;
            let c_principal = Box::new(CPrincipal { ptr, len });
            Some(c_principal)
        }
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(c_string.as_ptr() as _, c_string.as_bytes().len() as _, error_ret.user_data);
            }
            None
        }
    }
}

/// @brief Free allocated Memory
///
/// @param bytes CPrincipal structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn principal_destroy(ptr: Option<Box<CPrincipal>>) {
    if ptr.is_some() {
        let p = ptr.unwrap();
        let _: Vec<u8> = unsafe { Vec::from_raw_parts(p.ptr, p.len, 0) };
    }
}

mod tests {
    #[allow(unused)]
    use core::slice;
    #[allow(unused)]
    use std::ffi::CStr;

    #[allow(unused)]
    use super::*;

    #[test]
    fn test_principal_anonymous() {
        let principal = principal_anonymous();
        assert!(principal.is_some());

        let principal = principal.unwrap();
        let slice = unsafe { std::slice::from_raw_parts(principal.ptr, principal.len as usize) };
        assert_eq!(slice.len(), 1);
        assert_eq!(slice[0], 4);

        principal_destroy(Some(principal));
    }

    #[test]
    fn test_principal_management_canister() {
        let principal = principal_management_canister();
        assert!(principal.is_some());
        let principal = principal.unwrap();
        assert_eq!(principal.len, 0);

        principal_destroy(Some(principal));
    }

    #[test]
    fn test_principal_self_authenticating() {
        const PK: [u8; 32] = [
            0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa,
            0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11,
            0x11, 0xaa, 0x11, 0xaa,
        ];
        const PRINCIPAL: [u8; 29] = [
            0x9e, 0x3a, 0xde, 0x5f, 0xe2, 0x5a, 0x80, 0x89, 0x4d, 0x27, 0x04, 0xe8, 0x44, 0xff,
            0xf8, 0x80, 0x30, 0x75, 0x06, 0x93, 0x09, 0x86, 0xed, 0xf5, 0x4c, 0xc4, 0xfb, 0xad,
            0x02,
        ];

        let principal = principal_self_authenticating(PK.as_ptr(), PK.len() as c_int);
        assert!(principal.is_some());
        let principal = principal.unwrap();
        let slice = unsafe { std::slice::from_raw_parts(principal.ptr, principal.len as usize) };

        assert_eq!(slice, &PRINCIPAL[..]);

        principal_destroy(Some(principal));
    }

    #[test]
    fn test_principal_to_text() {
        const TEXT: &[u8; 8] = b"aaaaa-aa";

        let principal = principal_to_text([0u8; 0].as_ptr(), 0, None);
        assert!(principal.is_some());
        let principal = principal.unwrap();
        let slice = unsafe { std::slice::from_raw_parts(principal.ptr, principal.len as usize) };

        assert_eq!(slice, TEXT);

        principal_destroy(Some(principal));
    }

    #[test]
    fn test_principal_from_text() {
        const ANONYMOUS_TEXT: &[u8; 28] = b"rrkah-fqaaa-aaaaa-aaaaq-cai\0";
        const BYTES: &[u8] = &[0, 0, 0, 0, 0, 0, 0, 1, 1, 1];

        let principal = principal_from_text(ANONYMOUS_TEXT.as_ptr() as *const c_char, None);
        assert!(principal.is_some());
        let principal = principal.unwrap();
        let slice = unsafe { std::slice::from_raw_parts(principal.ptr, principal.len as usize) };

        assert_eq!(slice, BYTES);

        principal_destroy(Some(principal));
    }
}
