use std::ffi::{CString, CStr};
use candid::Principal;
use crate::{AnyErr, RetPtr, ResultCode};
use cty::{c_char, c_int};

/// Construct a Principal of the IC management canister
#[no_mangle]
pub extern "C" fn principal_management_canister(principal : RetPtr<u8>) {
    let principal_tmp = Principal::management_canister();
    let arr = principal_tmp.as_ref();
    let len = arr.len() as c_int;

    principal(arr.as_ptr(), len);
}

/// Construct a self-authenticating ID from public key
#[no_mangle]
pub extern "C" fn principal_self_authenticating(
    public_key: *const u8,
    public_key_len: c_int,
    principal : RetPtr<u8>) {
    let public_key = unsafe { std::slice::from_raw_parts(public_key, public_key_len as usize) };
    let principal_tmp = Principal::self_authenticating(public_key);
    let arr = principal_tmp.as_ref();
    let len = arr.len() as c_int;

    principal(arr.as_ptr(), len);
}

/// Construct an anonymous ID
#[no_mangle]
pub extern "C" fn principal_anonymous(principal : RetPtr<u8>){
    let principal_tmp = Principal::anonymous();
    let arr = principal_tmp.as_ref();
    let len = arr.len() as c_int;

    principal(arr.as_ptr(), len);
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
    principal : RetPtr<u8>
) {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal_tmp = Principal::from_slice(slice);
    let arr = principal_tmp.as_ref();
    let len = arr.len() as c_int;

    principal(arr.as_ptr(), len);
}

/// Construct a Principal from a slice of bytes.
#[no_mangle]
pub extern "C" fn principal_try_from_slice(
    bytes: *const u8,
    bytes_len: c_int,
    principal_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>
) -> ResultCode {
    //Compute Slice of bytes
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };

    let principal_tmp = Principal::try_from_slice(slice);
    
    match principal_tmp {
        Ok(principal) => {
            let arr = principal.as_ref();
            let len = arr.len() as c_int;
            principal_ret(arr.as_ptr(), len);
            ResultCode::Ok
        }

        Err(e) => {
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }
}

/// Parse a Principal from text representation.
#[no_mangle]
pub extern "C" fn principal_from_text(
    text: *const c_char,
    principal_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>,
) -> ResultCode {

    let text_cstr = unsafe {
        assert!(!text.is_null());
        CStr::from_ptr(text)
    };
    let text_str = text_cstr.to_str().unwrap();

    let principal_tmp = Principal::from_text(text_str);
    match principal_tmp {
        Ok(principal) => {
            let arr = principal.as_ref();
            let len = arr.len() as c_int;
            principal_ret(arr.as_ptr(), len);
            ResultCode::Ok
        }

        Err(e) => {
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }
}

/// Return the textual representation of Principal.
#[no_mangle]
pub extern "C" fn principal_to_text(
    bytes: *const u8,
    bytes_len: c_int,
    principal_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>
) -> ResultCode {

    // In C Principal will be just a chunk of memory so compute principal struture here in rust
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };
    let principal = Principal::try_from_slice(slice).map_err(AnyErr::from);

    let text = principal
        .map(|principal| principal.to_text())
        .and_then(|text| CString::new(text).map_err(AnyErr::from))
        .map(|text| text.into_bytes_with_nul());

    match text {
        Ok(principal) => {
            let arr = principal.as_slice();
            let len = arr.len() as c_int;
            principal_ret(arr.as_ptr(), len);
            ResultCode::Ok
        }

        Err(e) => {
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }

}

// TODO : TESTING
mod tests{
    #[allow(unused)]
    use super::*;

    #[test]
    fn test_principal_management_canister() {
        extern "C" fn principal_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            // principal management 
            assert_eq!(slice, &[0u8; 0]);
            assert_eq!(len, 0);
        }

        principal_management_canister(principal_ret);
    }

        #[test]
    fn test_principal_self_authenticating() {
        const PK: [u8; 32] = [
            0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 
            0xaa, 0x11, 0xaa, 0x11, 0xaa, 0xaa, 0x11, 0xaa, 0x11, 
            0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 
            0x11, 0x11, 0xaa, 0x11, 0xaa,
        ];
        const PRINCIPAL: [u8; 29] = [
            0x9e, 0x3a, 0xde, 0x5f, 0xe2 ,0x5a, 0x80, 0x89, 0x4d,
            0x27, 0x04, 0xe8, 0x44, 0xff, 0xf8, 0x80, 0x30, 0x75,
            0x06, 0x93, 0x09, 0x86, 0xed, 0xf5, 0x4c, 0xc4, 0xfb,
            0xad, 0x02,
        ];

        extern "C" fn principal_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            // principal management 
            assert_eq!(slice, &PRINCIPAL);
            assert_eq!(len as usize, PRINCIPAL.len());
        }

        principal_self_authenticating(PK.as_ptr(), PK.len() as c_int, principal_ret);
    }

    #[test]
    fn test_principal_anonymous() {
        extern "C" fn principal_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            // should equal anonymous Tag
            assert_eq!(slice, &[4u8; 1]);
            assert_eq!(len, 1);
        }

        principal_anonymous(principal_ret);
    }

    #[test]
    fn test_principal_try_from_slice() {
        const SLICE_BYTES: [u8; 3] = [0x01, 0x23, 0x45];

        extern "C" fn principal_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            assert_eq!(slice, &SLICE_BYTES);
            assert_eq!(len as usize, SLICE_BYTES.len());
        }

        extern "C" fn error_ret(_data: *const u8, _len: c_int) {}

        assert_eq!(
            principal_try_from_slice(SLICE_BYTES.as_ptr(), SLICE_BYTES.len() as c_int, principal_ret, error_ret),
            ResultCode::Ok
        );
    }
}
