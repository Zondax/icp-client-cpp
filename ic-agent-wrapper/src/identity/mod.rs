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
#![allow(non_snake_case)]

use std::{ffi::{c_char, CStr}};
use cty::{c_void, c_int};
use ic_agent::{identity::{AnonymousIdentity, BasicIdentity, Secp256k1Identity}, Identity, Signature};
use k256::SecretKey;
use ring::signature::Ed25519KeyPair;
use crate::{AnyErr, ResultCode, RetPtr};


#[allow(dead_code)]
#[repr(C)]
#[derive(Debug, Eq, PartialEq, Copy, Clone, Hash)]
pub enum IdentityType {
    /// anonym
    Anonym = 0,
    /// basic
    Basic = 1,
    /// secp256k1
    Secp256k1 = 2,
}

/// Dummy
#[no_mangle]
pub extern "C" fn identity_type(id_type: IdentityType) -> IdentityType{
    id_type
}


/// The anonymous identity.
#[no_mangle]
pub extern "C" fn identity_anonymous_wrap(identity_ret: *mut *const c_void){
    let anonymous_id = Box::new(AnonymousIdentity {});
    let identity: *const c_void = Box::into_raw(anonymous_id) as *const c_void;
    unsafe {
        *identity_ret = identity;
    }
}

/// Create a BasicIdentity from reading a PEM Content
#[no_mangle]
pub extern "C" fn identity_basic_from_pem_wrap(
    pem_data: *const c_char, 
    identity_ret: *mut *const c_void, 
    error_ret: RetPtr<u8>) -> ResultCode {
    
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id =
        pem_str.and_then(|pem_str| BasicIdentity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));

    match basic_id {
        Ok(identity) => {
            let identity_tmp: *const c_void = Box::into_raw(Box::new(identity)) as *const c_void;
            unsafe {
                *identity_ret = identity_tmp;
            }
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


/// Create a BasicIdentity from a KeyPair from the ring crate.
#[no_mangle]
pub extern "C" fn identity_basic_from_key_pair_wrap(
    public_key: *const u8,
    private_key_seed: *const u8,
    identity_ret: *mut *const c_void, 
    error_ret: RetPtr<u8>) -> ResultCode {

    let public_key_slice =  unsafe {std::slice::from_raw_parts(public_key as *const u8, 32)};
    let private_key_seed_slice =  unsafe {std::slice::from_raw_parts(private_key_seed as *const u8, 32)};

    match Ed25519KeyPair::from_seed_and_public_key(private_key_seed_slice, public_key_slice) {
        Ok(key_pair) => {
            let basic_id = BasicIdentity::from_key_pair(key_pair);
            let identity_tmp: *const c_void = Box::into_raw(Box::new(basic_id)) as *const c_void;
            unsafe {
                *identity_ret = identity_tmp;
            }
            ResultCode::Ok
        }
        Err(e) => {
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    };
    ResultCode::Ok
}

/// Creates an identity from a PEM certificate.
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_pem_wrap(
    pem_data: *const c_char,
    identity_ret: *mut *const c_void, 
    error_ret: RetPtr<u8>) -> ResultCode {
    
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id =
        pem_str.and_then(|pem_str| Secp256k1Identity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));

    match basic_id {
        Ok(identity) => {
            let identity_tmp: *const c_void = Box::into_raw(Box::new(identity)) as *const c_void;
            unsafe {
                *identity_ret = identity_tmp;
            }
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

/// Creates an identity from a private key.
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_private_key_wrap(
    private_key: *const c_char,
    pk_len: usize,
    identity_ret: *mut *const c_void){
    
    let pk = unsafe { std::slice::from_raw_parts(private_key as *const u8, pk_len) };
    let pk = SecretKey::from_be_bytes(pk).unwrap();

    let anonymous_id = Box::new(Secp256k1Identity::from_private_key(pk));
    let identity: *const c_void = Box::into_raw(anonymous_id) as *const c_void;
    unsafe {
        *identity_ret = identity;
    }
}

/// Returns a sender, ie. the Principal ID that is used to sign a request.
/// Only one sender can be used per request.
#[no_mangle]
pub extern "C" fn identity_sender_wrap(
    id_ptr: *mut *const c_void,
    idType: IdentityType,
    principal_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>,
) -> ResultCode {

    unsafe {
        match idType {
            IdentityType::Anonym => {
                let boxed = Box::from_raw(id_ptr as *mut AnonymousIdentity);
                let principal = boxed.sender();
                    match principal {
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
            IdentityType::Basic => {
                let boxed = Box::from_raw(id_ptr as *mut BasicIdentity);
                let principal = boxed.sender();
                    match principal {
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
            IdentityType::Secp256k1 => {
                let boxed = Box::from_raw(id_ptr as *mut Secp256k1Identity);
                let principal = boxed.sender();
                    match principal {
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
        }
    }
}

/// Sign a blob, the concatenation of the domain separator & request ID,
/// creating the sender signature.>
#[no_mangle]
pub extern "C" fn identity_sign_wrap(
    bytes: *const u8,
    bytes_len: c_int,
    id_ptr: *mut *const c_void,
    idType: IdentityType,
    pubkey_ret: RetPtr<u8>,
    sig_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>,
) -> ResultCode {

    unsafe {
        match idType {
            IdentityType::Anonym => {
                let boxed = Box::from_raw(id_ptr as *mut AnonymousIdentity);
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                let signature = boxed.sign(blob);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();

                        let arr = public_key.as_slice();
                        let len = arr.len() as c_int;
                        pubkey_ret(arr.as_ptr(), len);

                        let arr = signature.as_slice();
                        let len = arr.len() as c_int;
                        sig_ret(arr.as_ptr(), len);
                        ResultCode:: Ok
                    }
                    Err(err) => {
                        let err_str = err.to_string() + "\0";
                        let arr = err_str.as_bytes();
                        let len = arr.len() as c_int;
                        error_ret(arr.as_ptr(), len);
                        ResultCode::Err
                    }
                }
            }
            IdentityType::Basic => {
                let boxed = Box::from_raw(id_ptr as *mut BasicIdentity);
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                let signature = boxed.sign(blob);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();

                        let arr = public_key.as_slice();
                        let len = arr.len() as c_int;
                        pubkey_ret(arr.as_ptr(), len);

                        let arr = signature.as_slice();
                        let len = arr.len() as c_int;
                        sig_ret(arr.as_ptr(), len);
                        ResultCode:: Ok
                    }
                    Err(err) => {
                        let err_str = err.to_string() + "\0";
                        let arr = err_str.as_bytes();
                        let len = arr.len() as c_int;
                        error_ret(arr.as_ptr(), len);
                        ResultCode::Err
                    }
                }
            }
            IdentityType::Secp256k1 => {
                let boxed = Box::from_raw(id_ptr as *mut Secp256k1Identity);
                let blob = std::slice::from_raw_parts(bytes, bytes_len as usize);
                let signature = boxed.sign(blob);

                match signature {
                    Ok(Signature {
                        public_key,
                        signature,
                    }) => {
                        let public_key = public_key.unwrap_or_default();
                        let signature = signature.unwrap_or_default();

                        let arr = public_key.as_slice();
                        let len = arr.len() as c_int;
                        pubkey_ret(arr.as_ptr(), len);

                        let arr = signature.as_slice();
                        let len = arr.len() as c_int;
                        sig_ret(arr.as_ptr(), len);
                        ResultCode:: Ok
                    }
                    Err(err) => {
                        let err_str = err.to_string() + "\0";
                        let arr = err_str.as_bytes();
                        let len = arr.len() as c_int;
                        error_ret(arr.as_ptr(), len);
                        ResultCode::Err
                    }
                } 
            }
        }
    }
}


#[cfg(test)]
mod tests {
    use candid::Principal;
    use ic_agent::{Identity};

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
        let mut identity: *const c_void = std::ptr::null();

        identity_anonymous_wrap(&mut identity);
        assert!(!identity.is_null());

        unsafe {
             let boxed = Box::from_raw(identity as *mut AnonymousIdentity);
             assert_eq!(boxed.sender(), Ok(Principal::anonymous()));
        }
    }

    #[test]
    fn test_identity_sender() {
        const ANONYM_ID: [u8; 1] = [4u8];

        let mut identity: *const c_void = std::ptr::null();

        identity_anonymous_wrap(&mut identity);

        extern "C" fn principal_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            assert_eq!(slice, ANONYM_ID);
        }

        extern "C" fn error_ret(_data: *const u8, _len: c_int) {}


        assert_eq!(identity_sender_wrap(&mut identity, IdentityType::Anonym, principal_ret, error_ret), ResultCode::Ok);
    }

    #[test]
    fn test_identity_basic_from_pem() {
        let mut identity: *const c_void = std::ptr::null();
        
        extern "C" fn error_ret(_data: *const u8, _len: c_int) {}
        
        assert_eq!(
            identity_basic_from_pem_wrap(
                BASIC_ID_FILE.as_ptr() as *const c_char,
                &mut identity,
                error_ret
            ),
            ResultCode::Ok,
        );

        unsafe {
            let boxed = Box::from_raw(identity as *mut BasicIdentity);
            let basic = BasicIdentity::from_pem(BASIC_ID_FILE.as_bytes()).unwrap();
            assert_eq!(boxed.sender(), basic.sender());
        }
    }

        #[test]
    fn test_identity_secp256k1_from_pem() {
        let mut identity: *const c_void = std::ptr::null();
        
        extern "C" fn error_ret(_data: *const u8, _len: c_int) {}
        
        assert_eq!(
            identity_secp256k1_from_pem_wrap(
                SECP256K1_ID_FILE.as_ptr() as *const c_char,
                &mut identity,
                error_ret
            ),
            ResultCode::Ok,
        );

        unsafe {
            let boxed = Box::from_raw(identity as *mut Secp256k1Identity);
            let secp = Secp256k1Identity::from_pem(SECP256K1_ID_FILE.as_bytes()).unwrap();
            assert_eq!(boxed.sender(), secp.sender());
        }
    }
}
