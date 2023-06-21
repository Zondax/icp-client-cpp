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
#include <memory>
#include "idl_value.h"
// #include "idl_value_utils.h"


namespace zondax::idl_value {

IdlValue::IdlValue(IDLValue *ptr) : ptr(ptr) {};

IdlValue::IdlValue() : ptr(nullptr) {};

IdlValue::IdlValue(uint8_t value) {
    ptr = idl_value_with_nat8(value);
}

IdlValue::IdlValue(uint16_t value) {
    ptr = idl_value_with_nat16(value);
}

IdlValue::IdlValue(uint32_t value) {
    ptr = idl_value_with_nat32(value);
}

IdlValue::IdlValue(uint64_t value) {
    ptr = idl_value_with_nat64(value);
}

IdlValue::IdlValue(int8_t value) {
    ptr = idl_value_with_int8(value);
}

IdlValue::IdlValue(int16_t value) {
    ptr = idl_value_with_int16(value);
}

IdlValue::IdlValue(int32_t value) {
    ptr = idl_value_with_int32(value);
}

IdlValue::IdlValue(int64_t value) {
    ptr = idl_value_with_int64(value);
}

IdlValue::IdlValue(float value) {
    ptr = idl_value_with_float32(value);
}

IdlValue::IdlValue(double value) {
    ptr = idl_value_with_float64(value);
}

IdlValue::IdlValue(bool value) {
    ptr = idl_value_with_bool(value);
}

IdlValue::IdlValue(std::string text) {
    // TODO: Use RetError
    // RetPtr_u8 error;

    ptr = idl_value_with_text(text.c_str(), nullptr);
}

IdlValue::IdlValue(zondax::principal::Principal principal, bool is_principal) {
    // TODO: Use RetError
    // RetPtr_u8 error;

    ptr = is_principal ? idl_value_with_principal(principal.getBytes().data(), principal.getBytes().size(), nullptr) :
                        idl_value_with_service(principal.getBytes().data(), principal.getBytes().size(), nullptr);
}

IdlValue IdlValue::FromNone(void) {
    ptr = idl_value_with_none();
    return IdlValue(ptr);
}

IdlValue IdlValue::FromNull(void) {
    ptr = idl_value_with_null();
    return IdlValue(ptr);
}

IdlValue IdlValue::FromReserved(void) {
    ptr = idl_value_with_reserved();
    return IdlValue(ptr);
}

IdlValue IdlValue::FromNumber(std::string number) {
    // TODO: Use RetError
    // RetPtr_u8 error;

    ptr = idl_value_with_number(number.c_str(), nullptr);
    return IdlValue(ptr);
}

IdlValue IdlValue::FromOpt(IdlValue *value) {
    ptr = idl_value_with_opt(value->ptr);
    return IdlValue(ptr);
}

IdlValue IdlValue::FromVec(const std::vector<const IdlValue *> &elems) {
    std::vector<const IDLValue *> cElems;
    cElems.reserve(elems.size());
    for (const IdlValue *elem : elems) {
        cElems.push_back(elem->ptr);
    }

    ptr = idl_value_with_vec(cElems.data(), cElems.size());
    return IdlValue(ptr);
}

IdlValue IdlValue::FromRecord(const std::vector<std::string> &keys, const std::vector<const IdlValue *> &elems) {
    std::vector<const IDLValue *> cElems;
    cElems.reserve(elems.size());
    for (const IdlValue *elem : elems) {
        cElems.push_back(elem->ptr);
    }

    std::vector<const char*> cKeys;
    cKeys.reserve(keys.size());
    for (const std::string& key : keys) {
        cKeys.push_back(key.c_str());
    }

    ptr = idl_value_with_record(cKeys.data(), cKeys.size(), cElems.data(), cElems.size());
    return IdlValue(ptr);
}

IdlValue IdlValue::FromVariant(std::string key, IdlValue *val, uint64_t code) {
    ptr = idl_value_with_variant(key.c_str(), val->ptr, code);
    return IdlValue(ptr);
}

IdlValue IdlValue::FromFunc(std::vector<uint8_t> vector, std::string func_name) {
    ptr = idl_value_with_func(vector.data(), vector.size(), func_name.c_str());
    return IdlValue(ptr);
}

bool IdlValue::getInt8(int8_t *val) {
    return int8_from_idl_value(ptr, val);
}

bool IdlValue::getInt16(int16_t *val) {
    return int16_from_idl_value(ptr, val);
}

bool IdlValue::getInt32(int32_t *val) {
    return int32_from_idl_value(ptr, val);
}

bool IdlValue::getInt64(int64_t *val) {
    return int64_from_idl_value(ptr, val);
}

bool IdlValue::getNat8(uint8_t *val) {
    return nat8_from_idl_value(ptr, val);
}

bool IdlValue::getNat16(uint16_t *val) {
    return nat16_from_idl_value(ptr, val);
}

bool IdlValue::getNat32(uint32_t *val) {
    return nat32_from_idl_value(ptr, val);
}

bool IdlValue::getNat64(uint64_t *val) {
    return nat64_from_idl_value(ptr, val);
}

bool IdlValue::getFloat32(float *val) {
    return float32_from_idl_value(ptr, val);
}

bool IdlValue::getFloat64(double *val) {
    return float64_from_idl_value(ptr, val);
}

bool IdlValue::getBool(bool *val) {
    return bool_from_idl_value(ptr, val);
}

std::string IdlValue::getText() {
    CText *ctext = text_from_idl_value(ptr);
    const char* str = ctext_str(ctext);
    uintptr_t len = ctext_len(ctext);

    std::string text(str, len);

    ctext_destroy(ctext);

    return text;
}

zondax::principal::Principal IdlValue::getPrincipal() {
    CPrincipal *p = principal_from_idl_value(ptr);

    std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
    zondax::principal::Principal principal(bytes);

    principal_destroy(p);

    return principal;
}

zondax::principal::Principal IdlValue::getService() {
    CPrincipal *p = service_from_idl_value(ptr);

    std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
    zondax::principal::Principal principal(bytes);

    principal_destroy(p);

    return principal;
}

std::string IdlValue::getNumber() {
    CText *ctext = text_from_idl_value(ptr);
        const char* str = ctext_str(ctext);
    uintptr_t len = ctext_len(ctext);

    std::string text(str, len);

    ctext_destroy(ctext);

    return text;
}

IdlValue IdlValue::getOpt() {
    IDLValue* result = opt_from_idl_value(ptr);
    return IdlValue(result);
}

std::vector<IdlValue> IdlValue::getVec() {
    struct CIDLValuesVec* cVec = vec_from_idl_value(ptr);
    uintptr_t length = cidlval_vec_len(cVec);

    std::vector<IdlValue> result;
    result.reserve(length);

    for (uintptr_t i = 0; i < length; ++i) {
        const IDLValue* valuePtr = cidlval_vec_value(cVec, i);
        result.emplace_back(valuePtr);
    }

    // Free the allocated CIDLValuesVec
    cidlval_vec_destroy(cVec);

    return result;
}

// TODO: Enable later forward declaration is tricky here,
// specially because it requires raw pointers, which might lead 
// to aliasing of the inner idlValue ptr, and this if doing wrong
// could cause memory issues, double free and so on.
// also smart pointers do not work here as types are not fully
// qualify.

// zondax::idl_value_utils::Record IdlValue::getRecord() {
//     struct CRecord* cRecord = record_from_idl_value(ptr);
//     zondax::idl_value_utils::Record result;
//
//     // Extract keys
//     uintptr_t keysLength = crecord_keys_len(cRecord);
//     for (uintptr_t i = 0; i < keysLength; ++i) {
//         const char* keyPtr = reinterpret_cast<const char*>(crecord_get_key(cRecord, i));
//         result.keys.emplace_back(keyPtr);
//     }
//
//     // Extract values
//     uintptr_t valsLength = crecord_vals_len(cRecord);
//     for (uintptr_t i = 0; i < valsLength; ++i) {
//         const IDLValue* valPtr = crecord_get_val(cRecord, i);
//         result.vals.emplace_back(valPtr);
//     }
//
//     // Free the allocated CRecord
//     crecord_destroy(cRecord);
//
//     return result;
// }

// zondax::idl_value_utils::Variant IdlValue::getVariant(){
//     struct CVariant* cVariant = variant_from_idl_value(ptr);
//     zondax::idl_value_utils::Variant result;
//
//     // Extract ID
//     result.id.assign(cvariant_id(cVariant), cvariant_id(cVariant) + cvariant_id_len(cVariant));
//
//     // Extract value
//     const IDLValue* valPtr = cvariant_idlvalue(cVariant);
//
//     result.val(new IdlValue(valPtr));
//
//     // Extract code
//     result.code = cvariant_code(cVariant);
//
//     // Free the allocated CVariant
//     cvariant_destroy(cVariant);
//
//     return result;
// }

zondax::idl_value_utils::Func IdlValue::getFunc() {
    struct CFunc* cFunc = func_from_idl_value(ptr);
    zondax::idl_value_utils::Func result;

    // Extract string
    result.s = std::string(cfunc_string(cFunc), cfunc_string(cFunc) + cfunc_string_len(cFunc));

    // Extract principal
    struct CPrincipal* cPrincipal = cfunc_principal(cFunc);
    std::vector<uint8_t> vec = std::vector<uint8_t>(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);
    result.p = zondax::principal::Principal(vec);

    // Free the allocated CFunc
    cfunc_destroy(cFunc);

    return result;
}

IDLValue* IdlValue::getPtr() const{
    return ptr;
}

IdlValue::~IdlValue() {
    idl_value_destroy(ptr); // Call the destroy function
}

}


