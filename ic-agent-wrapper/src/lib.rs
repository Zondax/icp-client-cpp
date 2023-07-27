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
use ::candid::parser::value::IDLValue;
use ::candid::Principal;
use anyhow::Error as AnyErr;
use anyhow::Result as AnyResult;
use libc::c_void;
use principal::CPrincipal;
use std::ffi::c_char;
use std::ffi::c_int;

mod agent;
mod candid;
mod identity;
mod principal;
mod request_id;

/// CallBack Ptr creation with size and len
type RetPtr<T> = extern "C" fn(*const T, c_int, *mut c_void);

#[repr(C)]
pub struct RetError {
    user_data: *mut c_void,
    call: RetPtr<u8>,
}

#[repr(u8)]
#[derive(Debug)]
enum IdlValueType {
    Bool,
    Null,
    Text,
    Number,
    Float64,
    Opt,
    Vec,
    Record,
    Variant,
    Principal,
    Service,
    Func,
    None,
    Int,
    Nat,
    Nat8,
    Nat16,
    Nat32,
    Nat64,
    Int8,
    Int16,
    Int32,
    Int64,
    Float32,
    Reserved,
}

/// @brief Returns the type of the IdlValue as an u8 value.
///
/// @param _ptr Pointer to IDLValue
#[no_mangle]
pub extern "C" fn idl_value_type(value: &IDLValue) -> u8 {
    let ty = match value {
        IDLValue::Bool(_) => IdlValueType::Bool,
        IDLValue::Null => IdlValueType::Null,
        IDLValue::Text(_) => IdlValueType::Text,
        IDLValue::Number(_) => IdlValueType::Number,
        IDLValue::Float64(_) => IdlValueType::Float64,
        IDLValue::Opt(_) => IdlValueType::Opt,
        IDLValue::Vec(_) => IdlValueType::Vec,
        IDLValue::Record(_) => IdlValueType::Record,
        IDLValue::Variant(_) => IdlValueType::Variant,
        IDLValue::Principal(_) => IdlValueType::Principal,
        IDLValue::Service(_) => IdlValueType::Service,
        IDLValue::Func(..) => IdlValueType::Func,
        IDLValue::None => IdlValueType::None,
        IDLValue::Int(_) => IdlValueType::Int,
        IDLValue::Nat(_) => IdlValueType::Nat,
        IDLValue::Nat8(_) => IdlValueType::Nat8,
        IDLValue::Nat16(_) => IdlValueType::Nat16,
        IDLValue::Nat32(_) => IdlValueType::Nat32,
        IDLValue::Nat64(_) => IdlValueType::Nat64,
        IDLValue::Int8(_) => IdlValueType::Int8,
        IDLValue::Int16(_) => IdlValueType::Int16,
        IDLValue::Int32(_) => IdlValueType::Int32,
        IDLValue::Int64(_) => IdlValueType::Int64,
        IDLValue::Float32(_) => IdlValueType::Float32,
        IDLValue::Reserved => IdlValueType::Reserved,
    };

    ty as u8
}

/// @brief Free allocated memory
///
/// @param _ptr Pointer to IDLValue Array
#[no_mangle]
pub extern "C" fn idl_value_destroy(_ptr: Option<Box<IDLValue>>) {}

/// Structure that holds Rust string
#[derive(Debug, Clone)]
pub struct CText {
    data: Vec<u8>,
}

/// @brief Free allocated CText
///
/// @param ptr CText structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn ctext_destroy(_ptr: Option<Box<CText>>) {}

/// @brief Get pointer to Ctext content
///
/// @param ptr CText structure pointer
/// @return Pointer to the content allocated in rust
#[no_mangle]
pub extern "C" fn ctext_str(ptr: &CText) -> *const c_char {
    ptr.data.as_ptr() as _
}

/// @brief Get Ctext content length
///
/// @param bytes CText structure pointer
/// @return Ctext content length
#[no_mangle]
pub extern "C" fn ctext_len(ptr: &CText) -> usize {
    ptr.data.len()
}

/// Structure that holds Rust u8 Vector
pub struct CBytes {
    data: Vec<u8>,
}

/// @brief Free allocated CBytes
///
/// @param ptr CBytes structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn cbytes_destroy(_ptr: Option<Box<CBytes>>) {}

/// @brief Get pointer to CBytes content
///
/// @param ptr CBytes structure pointer
/// @return Pointer to the content allocated in rust
#[no_mangle]
pub extern "C" fn cbytes_ptr(ptr: &CBytes) -> *const u8 {
    ptr.data.as_ptr() as _
}

/// @brief Get CBytes content length
///
/// @param bytes CBytes structure pointer
/// @return CBytes content length
#[no_mangle]
pub extern "C" fn cbytes_len(ptr: &CBytes) -> usize {
    ptr.data.len()
}

/// Struture that holds a vector of IDLValues
pub struct CIDLValuesVec {
    data: Vec<*const IDLValue>,
}

/// @brief Free allocated CIDLValuesVec
///
/// @param ptr CIDLValuesVec structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub unsafe extern "C" fn cidlval_vec_destroy(ptr: Option<Box<CIDLValuesVec>>) {
    if let Some(cidlval_vec) = ptr {
        for ptr in cidlval_vec.data.iter() {
            // check if inner value is null,
            // that means it could have been move or changed owner
            let idlval = *ptr as *mut IDLValue;
            if idlval.is_null() {
                continue;
            }
            idl_value_destroy(Some(Box::from_raw(idlval)));
        }
    }
    // The memory for the CIDLValuesVec itself is freed here
}

/// @brief Get pointer to IDLValue vector
///
/// @param ptr CIDLValuesVec structure pointer
/// @return Pointer to the content allocated in rust
#[no_mangle]
pub extern "C" fn cidlval_vec(ptr: &CIDLValuesVec) -> *const IDLValue {
    ptr.data.as_ptr() as _
}

/// @brief Get pointer to IDLValue at specific index
///
/// @param ptr CIDLValuesVec structure pointer
/// @param index to specific index
/// @return Pointer to IDLValue
#[no_mangle]
pub extern "C" fn cidlval_vec_value(ptr: &CIDLValuesVec, index: usize) -> *const IDLValue {
    if index < ptr.data.len() {
        ptr.data[index]
    } else {
        std::ptr::null()
    }
}

/// @brief Get pointer to IDLValue at specific index
///
/// @param ptr CIDLValuesVec structure pointer
/// @param index to specific index
/// @return Pointer to IDLValue
/// @note This gives ownership of the requested value to the caller, who is in charge
/// of free the memory.
#[no_mangle]
pub extern "C" fn cidlval_vec_value_take(ptr: &mut CIDLValuesVec, index: usize) -> *const IDLValue {
    if index < ptr.data.len() {
        let owned_ptr = ptr.data[index];
        ptr.data[index] = std::ptr::null();
        owned_ptr
    } else {
        std::ptr::null()
    }
}

/// @brief Get CIDLValuesVec length
///
/// @param ptr CIDLValuesVec structure pointer
/// @return Number of IDLValues inside the vector
#[no_mangle]
pub extern "C" fn cidlval_vec_len(ptr: &CIDLValuesVec) -> usize {
    ptr.data.len()
}

/// Struture that holds a Record IDLValue with a Vector of keys (CText) and Vector of IDLValues
#[derive(Debug, Clone)]
pub struct CRecord {
    keys: Vec<CText>,
    vals: Vec<IDLValue>,
}

/// @brief Free allocated CRecord
///
/// @param ptr CRecord structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn crecord_destroy(_ptr: Option<Box<CRecord>>) {}

/// @brief Get pointer to CRecord keys vector
///
/// @param ptr CRecord structure pointer
/// @return Pointer to the content of Keys vector
#[no_mangle]
pub extern "C" fn crecord_keys(ptr: &CRecord) -> *const u8 {
    ptr.keys.as_ptr() as _
}

/// @brief Get pointer to a CRecord Key at specific index
///
/// @param ptr CRecord structure pointer
/// @param index to specific index
/// @return Pointer to CRecord Key
///
/// @note Ownership is transfered to the caller
#[no_mangle]
pub extern "C" fn crecord_take_key(ptr: *mut CRecord, index: usize) -> *mut CText {
    if ptr.is_null() {
        return std::ptr::null_mut();
    }

    let keys = unsafe { &mut (*ptr).keys };

    if index < keys.len() {
        let mut key = CText { data: vec![0] };
        std::mem::swap(&mut keys[index], &mut key);

        Box::into_raw(Box::new(key))
    } else {
        std::ptr::null_mut()
    }
}

/// @brief Get CRecord Key Vector length
///
/// @param ptr CRecord structure pointer
/// @return Number of Keys inside the CRecord Keys vector
#[no_mangle]
pub extern "C" fn crecord_keys_len(ptr: &CRecord) -> usize {
    ptr.keys.len()
}

/// @brief Get pointer to CRecord Values vector
///
/// @param ptr CRecord structure pointer
/// @return Pointer to the content of Values vector
#[no_mangle]
pub extern "C" fn crecord_vals(ptr: &CRecord) -> *const IDLValue {
    ptr.vals.as_ptr() as _
}

/// @brief Get pointer to a CRecord Value at specific index
///
/// @param ptr CRecord structure pointer
/// @param index to specific index
/// @return Pointer to CRecord Value
///
/// @note Ownership is trasfered to the caller
#[no_mangle]
pub extern "C" fn crecord_take_val(ptr: *mut CRecord, index: usize) -> *mut IDLValue {
    if ptr.is_null() {
        return std::ptr::null_mut();
    }

    let vals = unsafe { &mut (*ptr).vals };

    if index < vals.len() {
        let mut val = IDLValue::Null;
        std::mem::swap(&mut vals[index], &mut val);

        Box::into_raw(Box::new(val))
    } else {
        std::ptr::null_mut()
    }
}

/// @brief Get CRecord Values Vector length
///
/// @param ptr CRecord structure pointer
/// @return Number of Values inside the CRecord Values vector
#[no_mangle]
pub extern "C" fn crecord_vals_len(ptr: &CRecord) -> usize {
    ptr.vals.len()
}

/// Struture that holds Func IDLValue with CText String and a Principal id
pub struct CFunc {
    s: CText,
    p: Principal,
}

/// @brief Free allocated CFunc
///
/// @param ptr CFunc structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn cfunc_destroy(_ptr: Option<Box<CFunc>>) {}

/// @brief Get String from Func IDL Value
///
/// @param ptr CFunc structure pointer
/// @return Pointer to the String content of Func
#[no_mangle]
pub extern "C" fn cfunc_string(ptr: &CFunc) -> *const c_char {
    ptr.s.data.as_ptr() as _
}

/// @brief Get String length from Func IDL Value
///
/// @param ptr CFunc structure pointer
/// @return String length
#[no_mangle]
pub extern "C" fn cfunc_string_len(ptr: &CFunc) -> usize {
    ptr.s.data.len()
}

/// @brief Get Principal from Func IDL Value
///
/// @param ptr CFunc structure pointer
/// @return Pointer to the CPrincipal content of Func
#[no_mangle]
pub extern "C" fn cfunc_principal(ptr: &CFunc) -> Option<Box<CPrincipal>> {
    let arr = ptr.p.as_ref();
    let len = arr.len();
    let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
    let c_principal = Box::new(CPrincipal { ptr, len });
    Some(c_principal)
}

/// Struture that holds Variant IDLValue with id:(Vec(u8)), IDLValue and u64 code
pub struct CVariant {
    id: Vec<u8>,
    val: IDLValue,
    code: u64,
}

/// @brief Free allocated CVariant
///
/// @param ptr CVariant structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn cvariant_destroy(_ptr: Option<Box<CVariant>>) {}

/// @brief Get Id from Variant IDL Value
///
/// @param ptr CVariant structure pointer
/// @return Pointer to the ID content of Variant
#[no_mangle]
pub extern "C" fn cvariant_id(ptr: &CVariant) -> *const u8 {
    ptr.id.as_ptr() as _
}

/// @brief Get ID length from Variant IDL Value
///
/// @param ptr CVariant structure pointer
/// @return Id length
#[no_mangle]
pub extern "C" fn cvariant_id_len(ptr: &CVariant) -> usize {
    ptr.id.len()
}

/// @brief Get IDLValue from Variant IDL Value
///
/// @param ptr CVariant structure pointer
/// @return Pointer to the IDLValue content of Variant
#[no_mangle]
pub extern "C" fn cvariant_idlvalue(ptr: &CVariant) -> Option<Box<IDLValue>> {
    let val = &ptr.val;
    Some(Box::new(val.to_owned()))
}

/// @brief Get Code from Variant IDL Value
///
/// @param ptr CVariant structure pointer
/// @return Code of Variant
#[no_mangle]
pub extern "C" fn cvariant_code(ptr: &CVariant) -> u64 {
    ptr.code
}

/// Structure that holds the result of identity sign with pubkey and signature result
pub struct CIdentitySign {
    pubkey: Vec<u8>,
    signature: Vec<u8>,
}

/// @brief Free allocated CIdentitySign
///
/// @param ptr CIdentitySign structure pointer
/// Rust code will deal the memory allocation but the user should guarantee
/// The memory is free when isn't needed anymore
#[no_mangle]
pub extern "C" fn cidentitysign_destroy(_ptr: Option<Box<CIdentitySign>>) {}

/// @brief Get Pubkey from CIdentitySign
///
/// @param ptr CIdentitySign structure pointer
/// @return Pointer to the Public Key returned by the signing process
#[no_mangle]
pub extern "C" fn cidentitysign_pubkey(ptr: &CIdentitySign) -> *const u8 {
    ptr.pubkey.as_ptr() as _
}

/// @brief Get Pubkey Length
///
/// @param ptr CIdentitySign structure pointer
/// @return Size of the returned Pubkey
#[no_mangle]
pub extern "C" fn cidentitysign_pubkey_len(ptr: &CIdentitySign) -> usize {
    ptr.pubkey.len()
}

/// @brief Get Signature from CIdentitySign
///
/// @param ptr CIdentitySign structure pointer
/// @return Pointer to the Signature returned by the signing process
#[no_mangle]
pub extern "C" fn cidentitysign_sig(ptr: &CIdentitySign) -> *const u8 {
    ptr.signature.as_ptr() as _
}

/// @brief Get Signature Length
///
/// @param ptr CIdentitySign structure pointer
/// @return Size of the returned Signature
#[no_mangle]
pub extern "C" fn cidentitysign_sig_len(ptr: &CIdentitySign) -> usize {
    ptr.signature.len()
}
