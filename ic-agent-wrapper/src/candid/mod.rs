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
use anyhow::anyhow;
use candid::IDLArgs;
use candid::{
    parser::value::{IDLField, IDLValue, VariantValue},
    types::Label,
    Nat, Principal,
};
use std::ops::Deref;
use std::{
    ffi::{c_char, c_int, CStr, CString},
    str::{FromStr, Utf8Error},
};

use crate::{
    principal::CPrincipal, AnyErr, CBytes, CFunc, CIDLValuesVec, CRecord, CText, CVariant, RetError,
};

/*******************************************************************************
* Candid Args
********************************************************************************/

/// @brief Translate IDLArgs to text
///
/// @param idl_args Pointer to IdlArgs
/// @return pointer to a CText structure that can be used to access the result from rust
/// If the function returns a NULL IDLArgs the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_args_to_text(idl_args: &IDLArgs) -> Option<Box<CText>> {
    let data = CString::new(idl_args.to_string()).ok()?.into_bytes();
    let c_text = Box::new(CText { data });
    Some(c_text)
}

/// @brief Get IDLArgs from text
///
/// @param text Pointer to text content
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLArgs Structure
/// If the function returns a NULL IDLArgs the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_args_from_text(
    text: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLArgs>> {
    let text = unsafe { CStr::from_ptr(text).to_str().map_err(AnyErr::from) };

    let idl_value = text.and_then(|text| IDLArgs::from_str(text).map_err(AnyErr::from));

    match idl_value {
        Ok(t) => Some(Box::new(t)),
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

/// @brief Translate IDLArgs to bytes array
///
/// @param idl_args Pointer to IdlArgs
/// @return pointer to a CBytes structure that can be used to access the result from rust
/// If the function returns a NULL IDLArgs the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_args_to_bytes(
    idl_args: &IDLArgs,
    error_ret: Option<&mut RetError >,
) -> Option<Box<CBytes>> {
    let idl_bytes = idl_args.to_bytes();

    match idl_bytes {
        Ok(t) => Some(Box::new(CBytes { data: t })),
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

/// @brief Translate IDLArgs from a byte representation
///
/// @param bytes Pointer to bytes content
/// @param bytes_len Length of byte array
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLArgs Structure
/// If the function returns a NULL IDLArgs the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_args_from_bytes(
    bytes: *const u8,
    bytes_len: c_int,
    error_ret: Option<&mut RetError >,
) -> Option<Box<IDLArgs>> {
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };

    let idl_args = IDLArgs::from_bytes(slice);

    match idl_args {
        Ok(t) => Some(Box::new(t)),
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

/// @brief Create IDLArgs from an IDLValue Array(C)/Vector(Rust)
///
/// @param elems Pointer to array of IDLValues, take in account rust will take
/// ownership of the memory where this array is stored and free it once it comes out of
/// the function scope. So the user should not use this array after calling
// this function
/// @param elems_len Number of IDLValues in the array
/// @return pointer to a IDLArgs structure
#[no_mangle]
pub extern "C" fn idl_args_from_vec(
    elems: *const *const IDLValue,
    elems_len: c_int,
) -> Box<IDLArgs> {
    let mut values = Vec::new();

    for i in 0..elems_len as usize {
        unsafe {
            let val_ptr = *elems.add(i);
            let boxed = Box::from_raw(val_ptr as *mut IDLValue);
            values.push(*boxed);
        }
    }

    let idl_args = IDLArgs { args: values };

    Box::new(idl_args)
}

/// @brief Create IDLValues array from IDLArgs
///
/// @param ptr Pointer to IDLArgs Array
/// @return Pointer to the Array of IDLValues , i.e. CIDLValuesVec struture
#[no_mangle]
pub extern "C" fn idl_args_to_vec(ptr: &IDLArgs) -> Option<Box<CIDLValuesVec>> {
    let r = {
        let idl_values = ptr.args.clone();

        let mut ptrs = Vec::new();

        for idl_value in idl_values {
            let boxed = Box::new(idl_value);
            let ptr = Box::into_raw(boxed);

            ptrs.push(ptr as *const IDLValue);
        }

        ptrs
    };

    Some(Box::new(CIDLValuesVec { data: r }))
}

/// @brief Free allocated memory
///
/// @param _ptr Pointer to IDLArgs Array
#[no_mangle]
pub extern "C" fn idl_args_destroy(_ptr: Option<Box<IDLArgs>>) {}

/*******************************************************************************
* Candid Values
********************************************************************************/

/// Format Text to IDLValue text format
#[no_mangle]
pub extern "C" fn idl_value_format_text(
    text: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let text = unsafe { CStr::from_ptr(text).to_str().map_err(AnyErr::from) }.ok()?;

    // Try to recoup brackets
    let text = if !text.starts_with('(') && !text.ends_with(')') {
        format!("({text})")
    } else {
        text.to_string()
    };

    let idl = IDLValue::from_str(&text).map_err(AnyErr::from);

    match idl {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Test if IDLValues are equal
///
/// @param idl_1 Pointer to IDLValue
/// @param idl_2 Pointer to IDLValue
/// @return Bool result true if IDLValues are equal
#[no_mangle]
pub extern "C" fn idl_value_is_equal(idl_1: &IDLValue, idl_2: &IDLValue) -> bool {
    idl_1 == idl_2
}

/// @brief Create IDLValue with nat
///
/// @param nat pointer to nat as string
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLValue Structure
/// If the function returns a NULL IDLValue the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_value_with_nat(
    nat: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let result = unsafe { CStr::from_ptr(nat).to_str().map_err(AnyErr::from) }
        .and_then(|nat| Nat::from_str(nat).map_err(AnyErr::from))
        .map(IDLValue::Nat);

    match result {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Create IDLValue with nat8
///
/// @param val nat8 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_nat8(val: u8) -> Box<IDLValue> {
    let idl_value = IDLValue::Nat8(val);
    Box::new(idl_value)
}

/// @brief Get nat8 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value nat8 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn nat8_from_idl_value(idl: &IDLValue, value: Option<&mut u8>) -> bool {
    let mut ret = false;

    if let IDLValue::Nat8(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with nat16
///
/// @param val nat16 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_nat16(val: u16) -> Box<IDLValue> {
    let idl_value = IDLValue::Nat16(val);
    Box::new(idl_value)
}

/// @brief Get nat16 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value nat16 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn nat16_from_idl_value(idl: &IDLValue, value: Option<&mut u16>) -> bool {
    let mut ret = false;

    if let IDLValue::Nat16(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with nat32
///
/// @param val nat32 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_nat32(val: u32) -> Box<IDLValue> {
    let idl_value = IDLValue::Nat32(val);
    Box::new(idl_value)
}

/// @brief Get nat32 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value nat32 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn nat32_from_idl_value(idl: &IDLValue, value: Option<&mut u32>) -> bool {
    let mut ret = false;

    if let IDLValue::Nat32(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with nat64
///
/// @param val nat64 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_nat64(val: u64) -> Box<IDLValue> {
    let idl_value = IDLValue::Nat64(val);
    Box::new(idl_value)
}

/// @brief Get nat64 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value nat64 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn nat64_from_idl_value(idl: &IDLValue, value: Option<&mut u64>) -> bool {
    let mut ret = false;

    if let IDLValue::Nat64(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with int8
///
/// @param val int8 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_int8(val: i8) -> Box<IDLValue> {
    let idl_value = IDLValue::Int8(val);
    Box::new(idl_value)
}

/// @brief Get int8 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value int8 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn int8_from_idl_value(idl: &IDLValue, value: Option<&mut i8>) -> bool {
    let mut ret = false;

    if let IDLValue::Int8(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with int16
///
/// @param val int16 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_int16(val: i16) -> Box<IDLValue> {
    let idl_value = IDLValue::Int16(val);
    Box::new(idl_value)
}

/// @brief Get int16 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value int16 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn int16_from_idl_value(idl: &IDLValue, value: Option<&mut i16>) -> bool {
    let mut ret = false;

    if let IDLValue::Int16(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with int32
///
/// @param val int32 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_int32(val: i32) -> Box<IDLValue> {
    let idl_value = IDLValue::Int32(val);
    Box::new(idl_value)
}

/// @brief Get int32 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value int32 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn int32_from_idl_value(idl: &IDLValue, value: Option<&mut i32>) -> bool {
    let mut ret = false;

    if let IDLValue::Int32(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with int64
///
/// @param val int64 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_int64(val: i64) -> Box<IDLValue> {
    let idl_value = IDLValue::Int64(val);
    Box::new(idl_value)
}

/// @brief Get int64 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value int64 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn int64_from_idl_value(idl: &IDLValue, value: Option<&mut i64>) -> bool {
    let mut ret = false;

    if let IDLValue::Int64(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with float32
///
/// @param val float32 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_float32(val: f32) -> Box<IDLValue> {
    let idl_value = IDLValue::Float32(val);
    Box::new(idl_value)
}

/// @brief Get float32 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value float32 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn float32_from_idl_value(idl: &IDLValue, value: Option<&mut f32>) -> bool {
    let mut ret = false;

    if let IDLValue::Float32(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with float64
///
/// @param val float64 value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_float64(val: f64) -> Box<IDLValue> {
    let idl_value = IDLValue::Float64(val);
    Box::new(idl_value)
}

/// @brief Get float64 from IDLValue
///
/// @param idl IDLValue pointer
/// @param value float64 value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn float64_from_idl_value(idl: &IDLValue, value: Option<&mut f64>) -> bool {
    let mut ret = false;

    if let IDLValue::Float64(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }
    ret
}

/// @brief Create IDLValue with bool
///
/// @param val bool value
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_bool(val: bool) -> Box<IDLValue> {
    let idl_value = IDLValue::Bool(val);
    Box::new(idl_value)
}

/// @brief Get float32 from bool
///
/// @param idl IDLValue pointer
/// @param value bool value
/// @return boolean value that indicates if idlValue is in fact of this type
/// User can send value NULL if the objective is only to test the type of idl
#[no_mangle]
pub extern "C" fn bool_from_idl_value(idl: &IDLValue, value: Option<&mut bool>) -> bool {
    let mut ret = false;

    if let IDLValue::Bool(v) = idl {
        ret = true;
        if let Some(ptr) = value {
            *ptr = *v;
        }
    }

    ret
}

/// @brief Create IDLValue with null
///
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_null() -> Box<IDLValue> {
    let idl_value = IDLValue::Null;
    Box::new(idl_value)
}

/// @brief Create IDLValue with none
///
/// @return Pointer to the IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_none() -> Box<IDLValue> {
    let idl_value = IDLValue::None;
    Box::new(idl_value)
}

/// @brief Create IDLValue with text
///
/// @param text Text pointer
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLValue Structure
/// If the function returns a NULL IDLValue the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_value_with_text(
    text: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let text = unsafe { CStr::from_ptr(text).to_str().map_err(AnyErr::from) };

    let idl = text.map(|text| IDLValue::Text(text.to_string()));
    match idl {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Get Text from IDLValue
///
/// @param ptr Pointer to IDLValue
/// @return pointer to a CText structure that can be used to access the result from rust
#[no_mangle]
pub extern "C" fn text_from_idl_value(ptr: &IDLValue) -> Option<Box<CText>> {
    let s = match ptr {
        IDLValue::Text(v) => v.to_owned(),
        _ => return None,
    };

    let c_string = CString::new(s).ok()?;
    let data = c_string.into_bytes();

    let c_text = Box::new(CText { data });
    Some(c_text)
}

/// @brief Create IDLValue with Principal
///
/// @param principal Pointer to Principal bytes
/// @param principal_len Length Principal array of bytes
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLValue Structure
/// If the function returns a NULL IDLValue the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_value_with_principal(
    principal: *const u8,
    principal_len: c_int,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let slice = unsafe { std::slice::from_raw_parts(principal, principal_len as usize) };

    let idl = Principal::try_from_slice(slice).map(IDLValue::Principal);

    match idl {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Principal from IDLValue
///
/// @param ptr IDLValue structure pointer
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn principal_from_idl_value(ptr: &IDLValue) -> Option<Box<CPrincipal>> {
    let r = match ptr {
        IDLValue::Principal(v) => Ok(*v),
        _ => Err(anyhow!("Value not match IDL type")),
    };

    if let Ok(s) = r {
        let arr = s.as_ref();
        let len = arr.len();
        let ptr = Box::into_raw(arr.to_owned().into_boxed_slice()) as *mut u8;
        Some(Box::new(CPrincipal { ptr, len }))
    } else {
        None
    }
}

/// @brief Create IDLValue with Service
///
/// @param principal Pointer to Principal bytes
/// @param principal_len Length Principal array of bytes
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLValue Structure
/// If the function returns a NULL IDLValue the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_value_with_service(
    principal: *const u8,
    principal_len: c_int,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let slice = unsafe { std::slice::from_raw_parts(principal, principal_len as usize) };

    let idl = Principal::try_from_slice(slice).map(IDLValue::Service);

    match idl {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Service from IDLValue
///
/// @param ptr IDLValue structure pointer
/// @return Pointer to CPrincipal structure
#[no_mangle]
pub extern "C" fn service_from_idl_value(ptr: &IDLValue) -> Option<Box<CPrincipal>> {
    let IDLValue::Service(principal) = ptr else {
        return None;
    };

    let len = principal.as_slice().len();
    let arr = principal.as_slice().to_vec();

    let ptr = Box::into_raw(arr.into_boxed_slice()) as *mut u8;
    Some(Box::new(CPrincipal { ptr, len }))

}

/// @brief Create IDLValue with Service
///
/// @param number Pointer number represented as a string
/// @param error_ret CallBack to get error
/// @return Pointer to the IDLValue Structure
/// If the function returns a NULL IDLValue the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn idl_value_with_number(
    number: *const c_char,
    error_ret: Option<&mut RetError>,
) -> Option<Box<IDLValue>> {
    let number = unsafe { CStr::from_ptr(number).to_str().map_err(AnyErr::from) };

    let idl = number.map(|text| IDLValue::Number(text.to_string()));

    match idl {
        Ok(idl) => Some(Box::new(idl)),
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

/// @brief Get Number from IDLValue
///
/// @param ptr IDLValue structure pointer
/// @return Pointer to CText Structure where number string is saved
#[no_mangle]
pub extern "C" fn number_from_idl_value(ptr: &IDLValue) -> Option<Box<CText>> {
    let s = match ptr {
        IDLValue::Number(v) => v.clone(),
        _ => return None,
    };

    let c_string = CString::new(s).ok()?;
    let data = c_string.into_bytes();

    let c_text = Box::new(CText { data });
    Some(c_text)
}

/// @brief Create Opt IDLValue
///
/// @param number Pointer to IDLValue
/// @return Pointer to the Opt IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_opt(value: Box<IDLValue>) -> Box<IDLValue> {
    let idl = IDLValue::Opt(value.clone());

    Box::new(idl)
}

/// @brief Get IDLValue from Opt
///
/// @param ptr Pointer to Opt IDLValue
/// @return Pointer to IDLValue Structure
#[no_mangle]
pub extern "C" fn opt_from_idl_value(ptr: &IDLValue) -> Option<Box<IDLValue>> {
    let result = match ptr {
        IDLValue::Opt(v) => Ok(v.deref().clone()),
        _ => Err(anyhow!("Value not match IDL type")),
    };

    if let Ok(s) = result {
        Some(Box::new(s))
    } else {
        None
    }
}

/// @brief Create Reserved IDLValue
///
/// @return Pointer to the Reserved IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_reserved() -> Option<Box<IDLValue>> {
    let idl_value = IDLValue::Reserved;

    Some(Box::new(idl_value))
}

/// @brief Create IDLValue Vec from an IDLValue Array(C)/Vector(Rust)
///
/// @param elems Pointer to array of IDLValues, take in account rust will take
/// ownership of the memory where this array is stored and free it once it comes out of
/// the function scope. So the user should not use this array after calling
// this function
/// @param elems_len Number of IDLValues on the array
/// @return Pointer to IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_vec(
    elems: *const *const IDLValue,
    elems_len: c_int,
) -> Option<Box<IDLValue>> {
    let mut values = Vec::new();

    for i in 0..elems_len as usize {
        unsafe {
            let val_ptr = *(elems.add(i));
            let boxed = Box::from_raw(val_ptr as *mut IDLValue);
            values.push(*boxed);
        }
    }

    let idl_value = IDLValue::Vec(values);

    Some(Box::new(idl_value))
}

/// @brief Get IDLValue Array(C)/Vector(Rust) from Vec IDLValue
///
/// @param ptr Pointer to IDLValue structure
/// @return Pointer to Array of IDLValues , CIDLValuesVec
#[no_mangle]
pub extern "C" fn vec_from_idl_value(ptr: &IDLValue) -> Option<Box<CIDLValuesVec>> {
    let r = {
        let s = match ptr {
            IDLValue::Vec(v) => v.to_owned(),
            _ => return None,
        };

        let mut ptrs = Vec::new();

        for idl_value in s {
            let boxed = Box::new(idl_value);
            let ptr = Box::into_raw(boxed);

            ptrs.push(ptr as *const IDLValue);
        }

        ptrs
    };

    Some(Box::new(CIDLValuesVec { data: r }))
}

/// @brief Create IDLValue with array of keys and values where each pair represents an IDLField
///
/// @param keys Pointer to array of keys
/// @param keys_len Number of Keys
/// @param vals Pointer to array of IDLValues
/// @param vals_len Number of Values, take in account rust will take
/// ownership of the memory where this array is stored and free it once it comes out of
/// the function scope. So the user should not use this array after calling
// this function
/// @return Pointer to IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_record(
    keys: *const *const c_char,
    keys_len: c_int,
    vals: *const *const IDLValue,
    vals_len: c_int,
) -> Option<Box<IDLValue>> {
    let once = || {
        if keys_len != vals_len {
            return Err(anyhow!("The length of keys and vals are not matched"));
        }

        let mut rkeys = Vec::new();
        let mut rvals = Vec::new();

        for i in 0..keys_len as usize {
            unsafe {
                let key_ptr = *keys.add(i);

                let c_str = CStr::from_ptr(key_ptr as *const c_char);
                let str = c_str.to_str()?;

                rkeys.push(str.to_string());
            }
        }

        for i in 0..vals_len as usize {
            unsafe {
                let val_ptr = *vals.add(i);
                let boxed = Box::from_raw(val_ptr as *mut IDLValue);

                rvals.push(*boxed);

            }
        }

        let fields: Vec<IDLField> = rkeys
            .drain(..)
            .zip(rvals.drain(..))
            .map(|(key, val)| IDLField {
                id: Label::Named(key),
                val,
            })
            .collect();

        Ok(IDLValue::Record(fields))
    };
    Some(Box::new(once().ok()?))
}

/// @brief Get Record from IDLValue
///
/// @param ptr Pointer to IDLValue Structure
/// @return Pointer to CRecord structure where user can access array of keys and IDLValues
#[no_mangle]
pub extern "C" fn record_from_idl_value(ptr: &IDLValue) -> Option<Box<CRecord>> {
    if let IDLValue::Record(v) = ptr {
        let fields = v.clone();

        let mut keys_vec = Vec::new();
        let mut vals_vec = Vec::new();

        for IDLField { id, val } in fields {
            let id = CText {
                data: (id.to_string() + "\0").into_bytes(),
            };
            keys_vec.push(id);
            vals_vec.push(val);
        }

        Some(Box::new(CRecord {
            keys: keys_vec,
            vals: vals_vec,
        }))
    } else {
        None
    }
}

/// @brief Create Variant IDLValue with key, IDValue and code
///
/// @param key Pointer to key
/// @param val Pointer to IDLValues, take in account rust will take
/// ownership of the memory where this value is stored and free it once it comes out of
/// the function scope. So the user should not use this array after calling
// this function
/// @param code Variant code
/// @return Pointer to IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_variant(
    key: *const c_char,
    val: *const IDLValue,
    code: u64,
) -> Option<Box<IDLValue>> {
    let once = || {
        let key = unsafe { CStr::from_ptr(key).to_str() }?.to_string();

        let boxed = unsafe { Box::from_raw(val as *mut IDLValue) };
        let val = *boxed;

        Ok::<IDLValue, Utf8Error>(IDLValue::Variant(VariantValue(
            Box::new(IDLField {
                id: Label::Named(key),
                val,
            }),
            code,
        )))
    };

    Some(Box::new(once().ok()?))
}

/// @brief Get Variant from IDLValue
///
/// @param ptr Pointer to IDLValue Structure
/// @return Pointer to Variant structure where user can access key, value and code
#[no_mangle]
pub extern "C" fn variant_from_idl_value(ptr: &IDLValue) -> Option<Box<CVariant>> {
    let IDLValue::Variant(v) = ptr else {
        return None;
    };

    let id = v.0.id.to_string() + "\0";
    let val = v.0.val.clone();
    let code = v.1;

    Some(Box::new(CVariant {
        id: id.into_bytes(),
        val,
        code,
    }))
}

/// @brief Create Func IDLValue with Principal and Function Name
///
/// @param bytes Principal array of bytes
/// @param bytes_len Length of Principal
/// @param func_name Function name string
/// @return Pointer to IDLValue Structure
#[no_mangle]
pub extern "C" fn idl_value_with_func(
    bytes: *const u8,
    bytes_len: c_int,
    func_name: *const c_char,
) -> Option<Box<IDLValue>> {
    let slice = unsafe { std::slice::from_raw_parts(bytes, bytes_len as usize) };

    let once = || {
        let principal = Principal::try_from_slice(slice).map_err(AnyErr::from)?;
        let func_name = unsafe {
            CStr::from_ptr(func_name)
                .to_str()
                .map_err(AnyErr::from)?
                .to_string()
        };

        Ok::<IDLValue, AnyErr>(IDLValue::Func(principal, func_name))
    };

    Some(Box::new(once().ok()?))
}

/// @brief Get Func from IDLValue
///
/// @param ptr Pointer to IDLValue Structure
/// @return Pointer to CFunc structure where user can Principal and Function Name
#[no_mangle]
pub extern "C" fn func_from_idl_value(ptr: &IDLValue) -> Option<Box<CFunc>> {
    let r = match ptr {
        IDLValue::Func(p, s) => Ok((*p, s.clone() + "\0")),
        _ => Err(anyhow!("Value not match IDL type")),
    };

    if let Ok((p, s)) = r {
        let text = CText {
            data: s.into_bytes(),
        };
        Some(Box::new(CFunc { s: text, p }))
    } else {
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use candid::Principal;

    const IDL_VALUES: [IDLValue; 3] = [
        IDLValue::Bool(true),
        IDLValue::Principal(Principal::anonymous()),
        IDLValue::Int32(-12),
    ];

    const IDL_ARGS_TEXT: &str = r#"(true, principal "2vxsx-fae", -12 : int32)"#;
    const IDL_ARGS_TEXT_C: &str = "(true, principal \"2vxsx-fae\", -12 : int32)\0";

    const IDL_ARGS_BYTES: &[u8] = &[
        68, 73, 68, 76, 0, 3, 126, 104, 117, 1, 1, 1, 4, 244, 255, 255, 255,
    ];

    fn create_value_list() -> Vec<IDLValue> {
        vec![
            IDLValue::Bool(true),
            IDLValue::Null,
            IDLValue::Principal(Principal::anonymous()),
            IDLValue::Int32(-12),
        ]
    }

    #[test]
    fn idl_args_to_text_test() {
        let idl_args = IDLArgs::new(&IDL_VALUES);
        let text = idl_args_to_text(&idl_args).unwrap();
        unsafe {
            let s = CStr::from_ptr(text.data.as_ptr() as *const i8);
            assert_eq!(IDL_ARGS_TEXT, s.to_str().unwrap());
        }
    }

    #[test]
    fn idl_args_from_text_test() {

        let result = idl_args_from_text(IDL_ARGS_TEXT_C.as_ptr() as *const c_char, None);
        let result = result.unwrap();
        assert_eq!(IDLArgs::new(&IDL_VALUES), *result);
    }

    #[test]
    fn idl_args_to_bytes_test() {
        let idl_args = IDLArgs::new(&IDL_VALUES);
        let result = idl_args_to_bytes(&idl_args, None).unwrap();
        assert_eq!(IDL_ARGS_BYTES, result.data);
    }

    #[test]
    fn idl_args_from_bytes_test() {

        let result = idl_args_from_bytes(
            IDL_ARGS_BYTES.as_ptr(),
            IDL_ARGS_BYTES.len() as c_int,
            None,
        );
        let result = result.unwrap();
        assert_eq!(IDLArgs::new(&IDL_VALUES), *result);
    }

    #[test]
    fn idl_args_from_vec_test() {

        let result = idl_args_from_bytes(
            IDL_ARGS_BYTES.as_ptr(),
            IDL_ARGS_BYTES.len() as c_int,
            None,
        );
        let result = result.unwrap();
        assert_eq!(IDLArgs::new(&IDL_VALUES), *result);
    }

    #[test]
    fn idl_args_to_vec_test() {
        let idl_args = IDLArgs::new(&IDL_VALUES);
        let result = idl_args_to_vec(&idl_args);
        let result = result.unwrap();
        assert_eq!(result.data.len(), 3);

        for i in 0..result.data.len() as usize {
            let element = result.data[i];
            unsafe {
                let idl_value = Box::from_raw(element as *mut IDLValue);
                assert_eq!(&IDL_VALUES[i], idl_value.deref());
            }
        }
    }

    #[test]
    fn idl_value_with_nat_test() {
        const NAT: &str = "98989898989898989898";
        const C_NAT: *const c_char = b"98989898989898989898\0".as_ptr() as *const c_char;

        let result = idl_value_with_nat(C_NAT, None);
        let result = result.unwrap();
        assert_eq!(IDLValue::Nat(Nat::from_str(NAT).unwrap()), *result);
    }

    #[test]
    fn idl_value_with_nat8_test() {
        const NAT: u8 = 111;
        let result = idl_value_with_nat8(NAT);
        assert_eq!(IDLValue::Nat8(NAT), *result);
    }

    #[test]
    fn nat8_from_idl_value_test() {
        const NAT: u8 = 111;
        let mut out = 0u8;
        let idl_value = IDLValue::Nat8(NAT);
        nat8_from_idl_value(&idl_value, Some(&mut out));
        assert_eq!(NAT, out);
    }

    #[test]
    fn idl_value_with_int8_test() {
        const INT: i8 = -111;
        let result = idl_value_with_int8(INT);
        assert_eq!(IDLValue::Int8(INT), *result);
    }

    #[test]
    fn int8_from_idl_value_test() {
        const INT: i8 = -111;
        let mut out = 0i8;
        let idl_value = IDLValue::Int8(INT);
        int8_from_idl_value(&idl_value, Some(&mut out));
        assert_eq!(INT, out);
    }

    #[test]
    fn idl_value_with_float32_test() {
        const FLOAT: f32 = 0.01234235;
        let result = idl_value_with_float32(FLOAT);
        assert_eq!(IDLValue::Float32(FLOAT), *result);
    }

    #[test]
    fn float32_from_idl_value_test() {
        const FLOAT: f32 = 0.01234235;
        let mut out = 0f32;
        let idl_value = IDLValue::Float32(FLOAT);
        float32_from_idl_value(&idl_value, Some(&mut out));
        assert_eq!(FLOAT, out);
    }

    #[test]
    fn idl_value_with_bool_test() {
        let result_true = idl_value_with_bool(true);
        assert_eq!(IDLValue::Bool(true), *result_true);

        let result_false = idl_value_with_bool(false);
        assert_eq!(IDLValue::Bool(false), *result_false);
    }

    #[test]
    fn bool_from_idl_value_test() {
        const BOOL: bool = true;
        let mut out = false;
        let idl_value = IDLValue::Bool(BOOL);
        bool_from_idl_value(&idl_value, Some(&mut out));
        assert_eq!(BOOL, out);
    }

    #[test]
    fn idl_value_with_null_test() {
        let result = idl_value_with_null();
        assert_eq!(IDLValue::Null, *result);
    }

    #[test]
    fn idl_value_with_none_test() {
        let result = idl_value_with_none();
        assert_eq!(IDLValue::None, *result);
    }

    #[test]
    fn idl_value_with_number_test() {
        const NUMBER: &[u8] = b"1234567890\0";

        let result = idl_value_with_number(NUMBER.as_ptr() as *const c_char, None);
        let result = result.unwrap();
        assert_eq!(IDLValue::Number("1234567890".to_string()), *result);
    }

    #[test]
    fn number_from_idl_value_test() {
        let idl_value = IDLValue::Number("123.45".to_owned());

        let result = number_from_idl_value(&idl_value);
        let result = result.unwrap();

        assert_eq!(result.data, b"123.45");
    }

    #[test]
    fn idl_value_with_reserved_test() {
        let result = idl_value_with_reserved();
        let result = result.unwrap();
        assert_eq!(IDLValue::Reserved, *result);
    }

    #[test]
    fn idl_value_with_text_test() {
        const BTEXT: &[u8] = b"Hello World\0";

        let result = idl_value_with_text(BTEXT.as_ptr() as *const c_char, None);
        let result = result.unwrap();
        assert_eq!(&IDLValue::Text("Hello World".to_string()), result.deref());
    }

    #[test]
    fn text_from_idl_value_test() {
        let idl_value = IDLValue::Text("123.45".to_owned());

        let result = text_from_idl_value(&idl_value);
        let result = result.unwrap();

        assert_eq!(result.data, b"123.45");
    }
    #[test]
    fn idl_value_with_vec_test() {
        let values = create_value_list();

        let values_ptrs = values
            .iter()
            .map(|v| {
                let val = Box::new(v.clone());
                Box::into_raw(val) as *const _
            })
            .collect::<Vec<*const IDLValue>>();

        let ptr = values_ptrs.as_slice();

        let result = idl_value_with_vec(ptr.as_ptr(), values_ptrs.len() as c_int)
            .expect("idl_value_with_vec should not fail");

        if let IDLValue::Vec(vals) = result.deref() {
            for (i, v) in vals.iter().enumerate() {
                assert_eq!(values[i], *v);
            }
        }
    }

    #[test]
    fn vec_from_idl_value_test() {
        let values = create_value_list();

        let idl_value = IDLValue::Vec(values.clone());
        let result = vec_from_idl_value(&idl_value).unwrap();

        assert_eq!(result.data.len(), values.len());

        for i in 0..result.data.len() as usize {
            let element = result.data[i];
            unsafe {
                let idl_value = Box::from_raw(element as *mut IDLValue);
                assert_eq!(&values[i], idl_value.deref());
            }
        }
    }

    #[test]
    fn idl_value_with_record_test() {
        const KEYS: &[*const c_char] = &[
            b"Zondax01\0".as_ptr() as *const c_char,
            b"Zondax02\0".as_ptr() as *const c_char,
            b"Zondax03\0".as_ptr() as *const c_char,
        ];
        let value_bool = Box::new(IDLValue::Bool(true));
        let value_null = Box::new(IDLValue::Null);
        let value_principal = Box::new(IDLValue::Principal(Principal::anonymous()));
        let idl_value_list: &[*const IDLValue] = &[
            Box::into_raw(value_bool.clone()),
            Box::into_raw(value_null.clone()),
            Box::into_raw(value_principal.clone()),

        ];


        let expected = IDLValue::Record(vec![
            IDLField {
                id: Label::Named("Zondax01".into()),
                val: IDLValue::Bool(true),
            },
            IDLField {
                id: Label::Named("Zondax02".into()),
                val: IDLValue::Null,
            },
            IDLField {
                id: Label::Named("Zondax03".into()),
                val: IDLValue::Principal(Principal::anonymous()),
            },
        ]);

        let result = idl_value_with_record(
            KEYS.as_ptr(),
            KEYS.len() as c_int,
            idl_value_list.as_ptr(),
            idl_value_list.len() as c_int,
        );
        let result = result.unwrap();
        assert_eq!(&expected, result.deref());
    }

    #[test]
    fn record_from_idl_value_test() {
        const KEYS: [&str; 3] = ["Zondax01", "Zondax02", "666"];
        const VALS: [IDLValue; 3] = [
            IDLValue::Bool(true),
            IDLValue::Int64(-12),
            IDLValue::Principal(Principal::anonymous()),
        ];

        let idl_value = IDLValue::Record(vec![
            IDLField {
                id: Label::Named("Zondax01".to_string()),
                val: IDLValue::Bool(true),
            },
            IDLField {
                id: Label::Named("Zondax02".to_string()),
                val: IDLValue::Int64(-12),
            },
            IDLField {
                id: Label::Id(666),
                val: IDLValue::Principal(Principal::anonymous()),
            },
        ]);

        let result = record_from_idl_value(&idl_value).unwrap();
        assert_eq!(3, result.keys.len());
        for i in 0..1 as usize {
            let element = &result.keys[i];
            let s =
                CStr::from_bytes_with_nul(element.data.as_slice()).expect("Null terminaded string");
            assert_eq!(KEYS[i], s.to_str().unwrap());
        }

        assert_eq!(3, result.vals.len());
        for i in 0..result.vals.len() as usize {
            let element = &result.vals[i];
            assert_eq!(&VALS[i], element);
        }
    }

    #[test]
    fn idl_value_with_variant_test() {
        const KEY: *const c_char = b"Zondax\0".as_ptr() as *const c_char;
        let val = Box::into_raw(Box::new(IDLValue::Bool(true)));
        const CODE: u64 = 64;

        let expected = IDLValue::Variant(VariantValue(
            Box::new(IDLField {
                id: Label::Named("Zondax".into()),
                val: IDLValue::Bool(true),
            }),
            CODE,
        ));

        let result = idl_value_with_variant(KEY, val, CODE);
        let result = result.unwrap();
        assert_eq!(&expected, result.deref());
    }

    #[test]
    fn variant_from_idl_value_test() {
        const ID: &str = "Zondax";
        const IDL_VALUE: IDLValue = IDLValue::Bool(true);
        const CODE: u64 = 0;

        let idl_field = IDLField {
            id: Label::Named(ID.to_string()),
            val: IDL_VALUE,
        };
        let variant_value = VariantValue(Box::new(idl_field), CODE);

        let idl_value = IDLValue::Variant(variant_value);

        let result = variant_from_idl_value(&idl_value).unwrap();
        assert_eq!(IDL_VALUE, result.val);
        assert_eq!(CODE, result.code);

        let result_id = result.id;
        let str_slice = CStr::from_bytes_with_nul(&result_id).expect("Invalid UTF-8 sequence");
        assert_eq!(ID, str_slice.to_str().unwrap());
    }

    #[test]
    fn idl_value_with_func_test() {
        const PRINCIPAL: Principal = Principal::anonymous();
        const FUNC_NAME: &str = "zondax";
        const P_FUNC_NAME: *const c_char = b"zondax\0".as_ptr() as *const c_char;

        let result = idl_value_with_func(
            PRINCIPAL.as_ref().as_ptr(),
            PRINCIPAL.as_ref().len() as c_int,
            P_FUNC_NAME,
        );
        let result = result.unwrap();
        assert_eq!(&IDLValue::Func(PRINCIPAL, FUNC_NAME.into()), result.deref());
    }

    #[test]
    fn func_from_idl_value_test() {
        const EXPECTED_PRINCIPAL: Principal = Principal::anonymous();
        const EXPECTED_STR: &str = "func";

        let idl_value = IDLValue::Func(EXPECTED_PRINCIPAL, EXPECTED_STR.to_string());
        let result = func_from_idl_value(&idl_value).unwrap();

        let string = CStr::from_bytes_with_nul(&result.s.data).unwrap();
        assert_eq!(EXPECTED_PRINCIPAL, result.p);
        assert_eq!(EXPECTED_STR, string.to_str().unwrap());
    }
}
