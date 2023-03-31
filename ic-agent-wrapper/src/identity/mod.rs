use std::{ffi::{c_char, CStr, CString}};
use ic_agent::{identity::{AnonymousIdentity, BasicIdentity, Secp256k1Identity}};
use k256::SecretKey;
use crate::{AnyErr};

/// AnonymousId wrapper structure
#[repr(C)]
pub struct AnonymousIdentityWrapper {
    identity: *mut AnonymousIdentity,
}

/// BasicId wrapper structure
#[repr(C)]
pub struct BasicIdentityWrapper {
    identity: *mut BasicIdentity,
}

#[repr(C)]
pub struct BasicIdentityResult {
    identity: *mut BasicIdentityWrapper,
    error: *mut c_char,
}

impl BasicIdentityResult {
    fn new(identity: *mut BasicIdentityWrapper, error: *mut c_char) -> Self {
        Self { identity, error }
    }
}

/// Secp256k1 wrapper structure
#[repr(C)]
pub struct Secp256k1IdentityWrapper {
    identity: *mut Secp256k1Identity,
}

#[repr(C)]
pub struct Secp256k1IdentityResult {
    identity: *mut Secp256k1IdentityWrapper,
    error: *mut c_char,
}

impl Secp256k1IdentityResult {
    fn new(identity: *mut Secp256k1IdentityWrapper, error: *mut c_char) -> Self {
        Self { identity, error }
    }
}

/// The anonymous identity.
#[no_mangle]
pub extern "C" fn identity_anonymous() -> *mut AnonymousIdentityWrapper {
    let wrapper = Box::new(AnonymousIdentityWrapper {
        identity: Box::into_raw(Box::new(AnonymousIdentity {})),
    });

    Box::into_raw(wrapper)
}

/// Create a BasicIdentity from reading a PEM Content
#[no_mangle]
pub extern "C" fn identity_basic_from_pem(pem_data: *const c_char) -> BasicIdentityResult {
    
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id =
        pem_str.and_then(|pem_str| BasicIdentity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));


    match basic_id {
        Ok(identity) => {
            let identity_wrapper = BasicIdentityWrapper {
                identity: Box::into_raw(Box::new(identity))};
            BasicIdentityResult::new(Box::into_raw(Box::new(identity_wrapper)), std::ptr::null_mut())
        }
        Err(e) => {
            let error = CString::new(e.to_string() + "\0" ).unwrap();
            BasicIdentityResult::new(std::ptr::null_mut(), error.into_raw())
        }
    }
}

/// Create a BasicIdentity from a KeyPair from the ring crate.
// #[no_mangle]
// pub extern "C" fn identity_basic_from_key_pair(
//     public_key: *const u8,
//     private_key_seed: *const u8,) -> BasicIdentityResult {

//     let public_key_slice =  unsafe {std::slice::from_raw_parts(public_key as *const u8, 32)};
//     let private_key_seed_slice =  unsafe {std::slice::from_raw_parts(private_key_seed as *const u8, 32)};

//     let ed25519_key_pair = match Ed25519KeyPair::from_seed_and_public_key(private_key_seed_slice, public_key_slice) {
//         Ok(key_pair) => key_pair,
//         Err(e) => {
//             let error = CString::new(e.to_string() + "\0" ).unwrap();
//                 BasicIdentityResult::new(std::ptr::null_mut(), error.into_raw())
//         }
//     };

//     let basic_id = BasicIdentity::from_key_pair(ed25519_key_pair);

//     match basic_id {
//         Ok(identity) => {
//             let identity_wrapper = BasicIdentityWrapper {
//                 identity: Box::into_raw(Box::new(identity))};
//             BasicIdentityResult::new(Box::into_raw(Box::new(identity_wrapper)), std::ptr::null_mut())
//         }
//         Err(e) => {
//             let error = CString::new(e.to_string()).unwrap();
//             BasicIdentityResult::new(std::ptr::null_mut(), error.into_raw())
//         }
//     }
// }

/// Creates an identity from a PEM certificate.
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_pem(pem_data: *const c_char) -> Secp256k1IdentityResult {
    
    let pem_cstr = unsafe {
        assert!(!pem_data.is_null());
        CStr::from_ptr(pem_data)
    };
    let pem_str = pem_cstr.to_str().map_err(AnyErr::from);
    let basic_id =
        pem_str.and_then(|pem_str| Secp256k1Identity::from_pem(pem_str.as_bytes()).map_err(AnyErr::from));

    match basic_id {
        Ok(identity) => {
            let identity_wrapper = Secp256k1IdentityWrapper {
                identity: Box::into_raw(Box::new(identity))};
            Secp256k1IdentityResult::new(Box::into_raw(Box::new(identity_wrapper)), std::ptr::null_mut())
        }
        Err(e) => {
            let error = CString::new(e.to_string() + "\0").unwrap();
            Secp256k1IdentityResult::new(std::ptr::null_mut(), error.into_raw())
        }
    }
}

/// Creates an identity from a private key.
#[no_mangle]
pub extern "C" fn identity_secp256k1_from_private_key(private_key: *const c_char, pk_len: usize,) -> *mut Secp256k1IdentityWrapper {
    
    let pk = unsafe { std::slice::from_raw_parts(private_key as *const u8, pk_len) };
    let pk = SecretKey::from_be_bytes(pk).unwrap();

    let wrapper = Box::new(Secp256k1IdentityWrapper {
        identity: Box::into_raw(Box::new(Secp256k1Identity::from_private_key(pk))),
    });

    Box::into_raw(wrapper)
}

// Trait implementation has sender and sign for all identities 
// shoudl we define this functions all returning an generic Identity 
// see agent unity
// TODO identity_basic_from_key_pair
// TODO TESTING
