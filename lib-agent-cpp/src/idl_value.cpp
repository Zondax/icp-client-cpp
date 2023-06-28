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
#include "idl_value.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

namespace zondax {

#define PRIMITIVE_TYPES_GETTER(name, type)         \
  template <>                                      \
  std::optional<type> zondax::IdlValue::get() {    \
    if (ptr == nullptr) return std::nullopt;       \
    type value;                                    \
    bool ret = name##_from_idl_value(ptr, &value); \
    if (!ret) return std::nullopt;                 \
    return std::make_optional<type>(value);        \
  }  // namespace zondax

IdlValue::~IdlValue() {
  if (ptr != nullptr) {
    idl_value_destroy(ptr);  // Call the destroy function
    ptr = nullptr;
  }
}

// declare move constructor
IdlValue::IdlValue(IdlValue &&o) noexcept : ptr(o.ptr) { o.ptr = nullptr; }

// declare move assignment
IdlValue &IdlValue::operator=(IdlValue &&o) noexcept {
  if (&o == this) return *this;

  if (ptr != nullptr) idl_value_destroy(ptr);

  ptr = o.ptr;

  o.ptr = nullptr;

  return *this;
}

template <>
IdlValue::IdlValue(IDLValue *ptr) : ptr(ptr){};

template <>
IdlValue::IdlValue(uint8_t value) {
  ptr = idl_value_with_nat8(value);
}

template <>
IdlValue::IdlValue(uint16_t value) {
  ptr = idl_value_with_nat16(value);
}

template <>
IdlValue::IdlValue(uint32_t value) {
  ptr = idl_value_with_nat32(value);
}

template <>
IdlValue::IdlValue(uint64_t value) {
  ptr = idl_value_with_nat64(value);
}

template <>
IdlValue::IdlValue(int8_t value) {
  ptr = idl_value_with_int8(value);
}

template <>
IdlValue::IdlValue(int16_t value) {
  ptr = idl_value_with_int16(value);
}

template <>
IdlValue::IdlValue(int32_t value) {
  ptr = idl_value_with_int32(value);
}

template <>
IdlValue::IdlValue(int64_t value) {
  ptr = idl_value_with_int64(value);
}

template <>
IdlValue::IdlValue(float value) {
  ptr = idl_value_with_float32(value);
}

template <>
IdlValue::IdlValue(double value) {
  ptr = idl_value_with_float64(value);
}

template <>
IdlValue::IdlValue(bool value) {
  ptr = idl_value_with_bool(value);
}

template <>
IdlValue::IdlValue(std::string text) {
  // TODO: Use RetError
  ptr = idl_value_with_text(text.c_str(), nullptr);
}

IdlValue::IdlValue(zondax::Principal principal, bool is_principal) {
  // TODO: Use RetError
  // RetPtr_u8 error;

  ptr = is_principal
            ? idl_value_with_principal(principal.getBytes().data(),
                                       principal.getBytes().size(), nullptr)
            : idl_value_with_service(principal.getBytes().data(),
                                     principal.getBytes().size(), nullptr);
}

template <typename T, typename>
IdlValue::IdlValue(std::optional<T> val) {
  if (val.has_value()) {
    *this = IdlValue(val.value());
    // TOOD: ptr = idl_value_with_opt(value->ptr);
  } else {
    ptr = idl_value_with_none();
  }
}

template <typename Tuple, size_t... Indices>
void IdlValue::initializeFromTuple(const Tuple &tuple,
                                   std::index_sequence<Indices...>) {
  std::vector<IDLValue *> values;
  // TODO: use uint32_t directly
  std::vector<const char *> indices;

  (
      [&](size_t index) {
        IdlValue val(std::get<index>(tuple));
        values.push_back(val.ptr);
        val.ptr = nullptr;

        // TODO: don't convert to string, but use directly
        auto key = std::to_string(static_cast<uint32_t>(index));
        char *copiedKey = new char[key.length() + 1];
        key.copy(copiedKey, key.length());

        indices.push_back(copiedKey);
      }(Indices),
      ...);

  // TODO: replace with `idl_value_with_tuple` to use Label::Unnamed
  ptr = idl_value_with_record(indices.data(), indices.size(), values.data(),
                              values.size());
}

template <typename... Args, typename>
IdlValue::IdlValue(const std::tuple<Args...> &tuple) {
  initializeFromTuple(tuple, std::index_sequence_for<Args...>());
}

template <typename... Args, typename>
IdlValue::IdlValue(const std::variant<Args...> &variant) {
  std::visit([this](const auto &value) { *this = IdlValue(value); }, variant);
}

IdlValue IdlValue::null(void) {
  IdlValue val;
  val.ptr = idl_value_with_null();
  return val;
}

IdlValue IdlValue::reserved(void) {
  IdlValue val;
  val.ptr = idl_value_with_reserved();
  return val;
}

IdlValue IdlValue::BigNum(std::string number) {
  // TODO: Use RetError
  // RetPtr_u8 error;
  IdlValue val;

  val.ptr = idl_value_with_number(number.c_str(), nullptr);
  return val;
}

template <typename T, typename>
IdlValue::IdlValue(const std::vector<T> &elems) {
  std::vector<const IDLValue *> cElems;
  cElems.reserve(elems.size());

  for (auto e : elems) {
    IdlValue val(e);
    cElems.push_back(val.ptr);
    val.ptr = nullptr;  // avoid destructor freeing this memory
  }

  ptr = idl_value_with_vec(cElems.data(), cElems.size());
}

// TODO: improve the constructors below

IdlValue IdlValue::FromRecord(const std::vector<std::string> &keys,
                              const std::vector<const IdlValue *> &elems) {
  std::vector<const IDLValue *> cElems;
  cElems.reserve(elems.size());
  for (const IdlValue *elem : elems) {
    cElems.push_back(elem->ptr);
  }

  std::vector<const char *> cKeys;
  cKeys.reserve(keys.size());
  for (const std::string &key : keys) {
    cKeys.push_back(key.c_str());
  }

  ptr = idl_value_with_record(cKeys.data(), cKeys.size(), cElems.data(),
                              cElems.size());
  return IdlValue(ptr);
}

IdlValue IdlValue::FromVariant(std::string key, IdlValue *val, uint64_t code) {
  ptr = idl_value_with_variant(key.c_str(), val->ptr, code);
  return IdlValue(ptr);
}

IdlValue IdlValue::FromFunc(std::vector<uint8_t> vector,
                            std::string func_name) {
  ptr = idl_value_with_func(vector.data(), vector.size(), func_name.c_str());
  return IdlValue(ptr);
}
PRIMITIVE_TYPES_GETTER(int8, int8_t)
PRIMITIVE_TYPES_GETTER(int16, int16_t)
PRIMITIVE_TYPES_GETTER(int32, int32_t)
PRIMITIVE_TYPES_GETTER(int64, int64_t)
PRIMITIVE_TYPES_GETTER(nat8, uint8_t)
PRIMITIVE_TYPES_GETTER(nat16, uint16_t)
PRIMITIVE_TYPES_GETTER(nat32, uint32_t)
PRIMITIVE_TYPES_GETTER(nat64, uint64_t)
PRIMITIVE_TYPES_GETTER(float32, float)
PRIMITIVE_TYPES_GETTER(float64, double)
PRIMITIVE_TYPES_GETTER(bool, bool)

template <>
std::optional<std::string> IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  CText *ctext = text_from_idl_value(ptr);
  if (ctext == nullptr) return std::nullopt;

  const char *str = ctext_str(ctext);
  uintptr_t len = ctext_len(ctext);

  std::string s(str, len);

  ctext_destroy(ctext);

  return std::make_optional<std::string>(s);
}

template <>
std::optional<zondax::Principal> IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  CPrincipal *p = principal_from_idl_value(ptr);
  if (p == nullptr) return std::nullopt;

  std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
  zondax::Principal principal(bytes);

  principal_destroy(p);

  return std::make_optional(std::move(principal));
}

template <>
std::optional<Number> IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  CText *ctext = number_from_idl_value(ptr);
  if (ctext == nullptr) return std::nullopt;

  const char *str = ctext_str(ctext);
  uintptr_t len = ctext_len(ctext);

  std::string text(str, len);

  ctext_destroy(ctext);
  Number number;
  number.value = text;

  return std::make_optional(number);
}

template <>
std::optional<zondax::Func> IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  struct CFunc *cFunc = func_from_idl_value(ptr);
  if (cFunc == nullptr) return std::nullopt;

  zondax::Func result;

  // Extract string
  result.s = std::string(cfunc_string(cFunc),
                         cfunc_string(cFunc) + cfunc_string_len(cFunc));

  // Extract principal
  struct CPrincipal *cPrincipal = cfunc_principal(cFunc);
  std::vector<uint8_t> vec =
      std::vector<uint8_t>(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);
  result.p = zondax::Principal(vec);

  // Free the allocated CFunc
  cfunc_destroy(cFunc);

  return std::optional<zondax::Func>(std::move(result));
}

template <>
std::optional<std::vector<IdlValue> > IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  // call bellow creates a new IDLValue::Vec,
  // this means it "clones" from this.ptr
  struct CIDLValuesVec *cVec = vec_from_idl_value(ptr);
  if (cVec == nullptr) return std::nullopt;

  uintptr_t length = cidlval_vec_len(cVec);

  std::vector<IdlValue> result;
  result.reserve(length);

  // then it should take out each value from the vector
  // and give ownership to the caller.
  for (uintptr_t i = 0; i < length; ++i) {
    // the take variant leave the value in the vector in a null state
    // this is valid as inner value belongs now to result
    const IDLValue *valuePtr = cidlval_vec_value_take(cVec, i);
    result.emplace_back(valuePtr);
  }

  // Free the allocated CIDLValuesVec
  cidlval_vec_destroy(cVec);

  return std::make_optional(std::move(result));
}

template <>
std::optional<zondax::Service> IdlValue::get() {
  if (ptr == nullptr) return std::nullopt;

  CPrincipal *p = service_from_idl_value(ptr);

  if (p == nullptr) return std::nullopt;

  std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
  zondax::Principal principal(bytes);

  // there is not issue in destroying raw principal
  // because bytes makes a deep copy
  principal_destroy(p);

  return std::make_optional(zondax::Service(std::move(principal)));
}

std::optional<IdlValue> IdlValue::getOpt() {
  if (ptr == nullptr) return std::nullopt;

  IDLValue *result = opt_from_idl_value(ptr);

  if (result == nullptr) return std::nullopt;

  return IdlValue(result);
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
//         const char* keyPtr = reinterpret_cast<const
//         char*>(crecord_get_key(cRecord, i));
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
//     result.id.assign(cvariant_id(cVariant), cvariant_id(cVariant) +
//     cvariant_id_len(cVariant));
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

IDLValue *IdlValue::getPtr() const { return ptr; }

void IdlValue::resetValue() { ptr = nullptr; }

}  // namespace zondax
