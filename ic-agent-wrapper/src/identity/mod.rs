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
#![allow(non_snake_case)]

use crate::{principal::CPrincipal, AnyErr, CIdentitySign, RetError};
use cty::c_int;
use ic_agent::{
    identity::{AnonymousIdentity, BasicIdentity, Secp256k1Identity},
    Identity, Signature,
};
use k256::SecretKey;
use libc::c_void;
use ring::signature::Ed25519KeyPair;
use std::ffi::{c_char, CStr, CString};
use sha2::{Digest, Sha256};

/// Enum for Identity Types
#[allow(dead_code)]
#[repr(C)]
#[derive(Debug, Eq, PartialEq, Copy, Clone, Hash)]
pub enum IdentityType {
    Anonym = 0,
    Basic = 1,
    Secp256k1 = 2,
}

/// @brief Create Anonymous Identity
///
/// @return Void pointer to Anonymous Identity
/// This function can be used by the user but its intent to be used in a C
/// friendly function that returns a CIdentity Structure
#[no_mangle]
pub extern "C" fn identity_anonymous() -> *mut c_void {
    let anonymous_id = Box::new(AnonymousIdentity {});
    let identity: *mut c_void = Box::into_raw(anonymous_id) as *mut c_void;
    identity
}

/// @brief Create a BasicIdentity from reading a PEM Content
///
/// @param pem_data Pointer to Pem file data
/// @param error_ret CallBack to get error
/// @return Void pointer to Basic Identity
/// This function can be used by the user but its intent to be used in a C
/// friendly function that returns a CIdentity Structure
/// If the function returns a NULL pointer the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn identity_basic_from_pem(
    pem_data: *const c_char,
    error_ret: Option<&mut RetError>,
) -> *mut c_void {
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id = pem_str
        .and_then(|pem_str| BasicIdentity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));

    match basic_id {
        Ok(identity) => Box::into_raw(Box::new(identity)) as *mut c_void,
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }

            std::ptr::null_mut()
        }
    }
}

/// @brief Create a BasicIdentity from a KeyPair from the ring crate
///
/// @param public_key Pointer to public key
/// @param private_key_seed Pointer to a private key seed
/// @param error_ret CallBack to get error
/// @return Void pointer to Basic Identity
/// This function can be used by the user but its intent to be used in a C
/// friendly function that returns a CIdentity Structure
/// If the function returns a NULL pointer the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn identity_basic_from_key_pair(
    public_key: *const u8,
    private_key_seed: *const u8,
    error_ret: Option<&mut RetError>,
) -> *mut c_void {
    let public_key_slice = unsafe { std::slice::from_raw_parts(public_key as *const u8, 32) };
    let private_key_seed_slice =
        unsafe { std::slice::from_raw_parts(private_key_seed as *const u8, 32) };

    match Ed25519KeyPair::from_seed_and_public_key(private_key_seed_slice, public_key_slice) {
        Ok(key_pair) => {
            let basic_id = BasicIdentity::from_key_pair(key_pair);
            Box::into_raw(Box::new(basic_id)) as *mut c_void
        }
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }

            std::ptr::null_mut()
        }
    }
}

/// @brief Creates an Secp256k1 identity from a PEM file
///
/// @param pem_data Pointer to Pem file data
/// @param error_ret CallBack to get error
/// @return Void pointer to Secp256k1 Identity
/// This function can be used by the user but its intent to be used in a C
/// friendly function that returns a CIdentity Structure
/// If the function returns a NULL pointer the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_pem(
    pem_data: *const c_char,
    error_ret: Option<&mut RetError>,
) -> *mut c_void {
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id = pem_str
        .and_then(|pem_str| Secp256k1Identity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));

    match basic_id {
        Ok(identity) => Box::into_raw(Box::new(identity)) as *mut c_void,
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }

            std::ptr::null_mut()
        }
    }
}

/// @brief Create a Secp256k1 from a KeyPair from the ring crate
///
/// @param private_key Pointer to a private key
/// @param pk_len Private key length
/// @return Void pointer to Secp256k1 Identity
/// This function can be used by the user but its intent to be used in a C
/// friendly function that returns a CIdentity Structure
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_private_key(
    private_key: *const c_char,
    pk_len: usize,
) -> *mut c_void {
    let pk = unsafe { std::slice::from_raw_parts(private_key as *const u8, pk_len) };
    let pk = SecretKey::from_bytes(pk.into()).unwrap();

    let anonymous_id = Box::new(Secp256k1Identity::from_private_key(pk));
    Box::into_raw(anonymous_id) as *mut c_void
}

/// @brief Returns a sender, ie. the Principal ID that is used to sign a request.
/// Only one sender can be used per request.
///
/// @param id_ptr Pointer to identity. This function does not take ownership
/// of the passed identity.
/// @param idType Identity Type
/// @param error_ret CallBack to get error
/// @return Void pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn identity_sender(
    id_ptr: *const c_void,
    idType: IdentityType,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CPrincipal>> {
    unsafe {
        match idType {
            IdentityType::Anonym => {
                let id = id_ptr as *const AnonymousIdentity;
                let principal = (&*id).sender();
                match principal {
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
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }
                        None
                    }
                }
            }
            IdentityType::Basic => {
                let id = id_ptr as *const BasicIdentity;
                let principal = (&*id).sender();
                match principal {
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
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }
                        None
                    }
                }
            }
            IdentityType::Secp256k1 => {
                let id = id_ptr as *const Secp256k1Identity;
                let principal = (&*id).sender();
                match principal {
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
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }
                        None
                    }
                }
            }
        }
    }
}

/// @brief Sign a blob, the concatenation of the domain separator & request ID,
/// creating the sender signature
///
/// @param bytes Pointer to blob content
/// @param bytes_len Length of blob
/// @param id_ptr Pointer to identity. This function does not take ownership of the passed
/// identity.
/// @param idType Identity Type
/// @param error_ret CallBack to get error
/// @return Pointer to the signature
#[no_mangle]
pub extern "C" fn identity_sign(
    bytes: *const u8,
    bytes_len: c_int,
    id_ptr: *const c_void,
    idType: IdentityType,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CIdentitySign>> {
    unsafe {
        match idType {
            IdentityType::Anonym => {
                let id = id_ptr as *const AnonymousIdentity;
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                let signature = (&*id).sign_arbitrary(blob);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                        delegations: _,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();
                        Some(Box::new(CIdentitySign {
                            pubkey: public_key,
                            signature: signature,
                        }))
                    }
                    Err(err) => {
                        let err_str = err.to_string();
                        let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                            let fallback_error = "Failed to convert error message to CString";
                            CString::new(fallback_error).expect("Fallback error message is invalid")
                        });
                        if let Some(error_ret) = error_ret {
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }

                        None
                    }
                }
            }
            IdentityType::Basic => {
                let id = id_ptr as *const BasicIdentity;
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                // Hash the blob using SHA256
                let mut hasher = Sha256::new();
                hasher.update(blob);
                let hash = hasher.finalize();

                let signature = (&*id).sign_arbitrary(&hash);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                        delegations: _,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();
                        Some(Box::new(CIdentitySign {
                            pubkey: public_key,
                            signature: signature,
                        }))
                    }
                    Err(err) => {
                        let err_str = err.to_string();
                        let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                            let fallback_error = "Failed to convert error message to CString";
                            CString::new(fallback_error).expect("Fallback error message is invalid")
                        });
                        if let Some(error_ret) = error_ret {
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }

                        None
                    }
                }
            }
            IdentityType::Secp256k1 => {
                let id = id_ptr as *const Secp256k1Identity;
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                // Hash the blob using SHA256
                let mut hasher = Sha256::new();
                hasher.update(blob);
                let hash = hasher.finalize();

                let signature = (&*id).sign_arbitrary(&hash);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                        delegations: _,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();
                        Some(Box::new(CIdentitySign {
                            pubkey: public_key,
                            signature: signature,
                        }))
                    }
                    Err(err) => {
                        let err_str = err.to_string();
                        let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                            let fallback_error = "Failed to convert error message to CString";
                            CString::new(fallback_error).expect("Fallback error message is invalid")
                        });
                        if let Some(error_ret) = error_ret {
                            (error_ret.call)(
                                c_string.as_ptr() as _,
                                c_string.as_bytes().len() as _,
                                error_ret.user_data,
                            );
                        }

                        None
                    }
                }
            }
        }
    }
}

/// @brief Free allocated Memory
///
/// @param identity Identity pointer
/// @param idType Identity Type
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn identity_destroy(identity: *mut c_void, idType: IdentityType) {
    if !identity.is_null() {
        match idType {
            IdentityType::Anonym => {
                let _id = unsafe { Box::from_raw(identity as *mut AnonymousIdentity) };
            }
            IdentityType::Basic => {
                let _id = unsafe { Box::from_raw(identity as *mut BasicIdentity) };
            }
            IdentityType::Secp256k1 => {
                let _id = unsafe { Box::from_raw(identity as *mut Secp256k1Identity) };
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use ic_agent::export::Principal;
    use ic_agent::Identity;

    #[allow(unused)]
    use super::*;
    const BASIC_ID_FILE: &'static str = "-----BEGIN PRIVATE KEY-----
MFMCAQEwBQYDK2VwBCIEIL9r4XBKsg4pquYBHY6rgfzuBsvCy89tgqDfDpofXRBP
oSMDIQBCkE1NL4X43clXS1LFauiceiiKW9NhjVTEpU6LpH9Qcw==
-----END PRIVATE KEY-----\0";

    const SECP256K1_ID_FILE: &str = "-----BEGIN EC PARAMETERS-----
BgUrgQQACg==
-----END EC PARAMETERS-----
-----BEGIN EC PRIVATE KEY-----
MHQCAQEEIAgy7nZEcVHkQ4Z1Kdqby8SwyAiyKDQmtbEHTIM+WNeBoAcGBSuBBAAK
oUQDQgAEgO87rJ1ozzdMvJyZQ+GABDqUxGLvgnAnTlcInV3NuhuPv4O3VGzMGzeB
N3d26cRxD99TPtm8uo2OuzKhSiq6EQ==
-----END EC PRIVATE KEY-----\0";

    #[test]
    fn test_identity_anonymous() {
        let identity = identity_anonymous();
        assert!(!identity.is_null());

        unsafe {
            let boxed = Box::from_raw(identity as *mut AnonymousIdentity);
            assert_eq!(boxed.sender(), Ok(Principal::anonymous()));
        }
    }

    #[test]
    fn test_identity_sender() {
        const ANONYMOUS_BYTES: [u8; 1] = [4u8];
        let identity = identity_anonymous();
        unsafe {
            let boxed = Box::from_raw(identity as *mut AnonymousIdentity);
            assert_eq!(boxed.sender(), Ok(Principal::anonymous()));
        }

        let principal = identity_sender(identity, IdentityType::Anonym, None);
        let principal = principal.unwrap();
        let slice = unsafe { std::slice::from_raw_parts(principal.ptr, principal.len as usize) };

        assert_eq!(ANONYMOUS_BYTES, slice);
    }

    #[test]
    fn test_identity_basic_from_pem() {
        let id = identity_basic_from_pem(BASIC_ID_FILE.as_ptr() as *const c_char, None);
        assert!(!id.is_null());

        unsafe {
            let boxed = Box::from_raw(id as *mut BasicIdentity);
            let basic = BasicIdentity::from_pem(BASIC_ID_FILE.as_bytes()).unwrap();
            assert_eq!(boxed.sender(), basic.sender());
        }
    }

    #[test]
    fn test_identity_secp256k1_from_pem() {
        let id = identity_secp256k1_from_pem(SECP256K1_ID_FILE.as_ptr() as *const c_char, None);

        unsafe {
            let boxed = Box::from_raw(id as *mut Secp256k1Identity);
            let secp = Secp256k1Identity::from_pem(SECP256K1_ID_FILE.as_bytes()).unwrap();
            assert_eq!(boxed.sender(), secp.sender());
        }
    }

    #[test]
    fn identity_sign_should_work() {
        const EMPTY_BYTES: [u8; 0] = [];
        const PUB_KEY_EXPECTED: [u8; 44] = [
            0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00, 0x42, 0x90,
            0x4d, 0x4d, 0x2f, 0x85, 0xf8, 0xdd, 0xc9, 0x57, 0x4b, 0x52, 0xc5, 0x6a, 0xe8, 0x9c,
            0x7a, 0x28, 0x8a, 0x5b, 0xd3, 0x61, 0x8d, 0x54, 0xc4, 0xa5, 0x4e, 0x8b, 0xa4, 0x7f,
            0x50, 0x73,
        ];
        const SIGNATURE_EXPECTED: [u8; 64] = [
            0x6d, 0x7a, 0x2f, 0x85, 0xeb, 0x6c, 0xc2, 0x18, 0x80, 0xc8, 0x3d, 0x9b, 0xb1, 0x70,
            0xe2, 0x4b, 0xf5, 0xd8, 0x9a, 0xa9, 0x96, 0x92, 0xb6, 0x89, 0xac, 0x9d, 0xe9, 0x5c,
            0x1e, 0x3e, 0x50, 0xdc, 0x98, 0x12, 0x2f, 0x94, 0x11, 0x2f, 0x6c, 0xc6, 0x6a, 0x0b,
            0xbf, 0xc0, 0x56, 0x5b, 0xdb, 0x87, 0xa9, 0xe2, 0x2c, 0x8e, 0x56, 0x94, 0x56, 0x12,
            0xde, 0xbf, 0x22, 0x4a, 0x3f, 0xdb, 0xf1, 0x03,
        ];

        let basic = BasicIdentity::from_pem(BASIC_ID_FILE.as_bytes()).unwrap();
        let fptr = Box::into_raw(Box::new(basic)) as *const dyn Identity;

        let result = identity_sign(
            EMPTY_BYTES.as_ptr(),
            EMPTY_BYTES.len() as c_int,
            fptr as *mut c_void,
            IdentityType::Basic,
            None,
        );

        let result = result.unwrap();
        assert_eq!(result.pubkey, PUB_KEY_EXPECTED);
        assert_eq!(result.signature, SIGNATURE_EXPECTED);
    }
}
