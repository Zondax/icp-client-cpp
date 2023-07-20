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
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include "doctest.h"
#include "func.h"
#include "service.h"
#include "zondax_ic.h"

namespace zondax {

#define PRIMITIVE_TYPES_GETTER(name, type)                                  \
  template <>                                                               \
  std::optional<type> zondax::IdlValue::getImpl(helper::tag_type<type> t) { \
    if (ptr == nullptr) return std::nullopt;                                \
    type value;                                                             \
    bool ret = name##_from_idl_value(ptr.get(), &value);                    \
    if (!ret) return std::nullopt;                                          \
    return std::make_optional<type>(value);                                 \
  }

#define VEC_PRIMITIVE_TYPES_GETTER(type)                                     \
  template <>                                                                \
  std::optional<std::vector<type>> zondax::IdlValue::getImpl(                \
      helper::tag_type<std::vector<type>> t) {                               \
    auto values_vec = vec_from_idl_value(ptr.get());                         \
    if (values_vec == nullptr) return std::nullopt;                          \
    auto vec_len = cidlval_vec_len(values_vec);                              \
    std::vector<type> ret;                                                   \
    for (int i = 0; i < vec_len; ++i) {                                      \
      auto p = cidlval_vec_value_take(values_vec, i);                        \
      if (p == nullptr) return std::nullopt;                                 \
      IdlValue value(p);                                                     \
      std::optional<type> val = value.get<type>();                           \
      if (!val.has_value()) return std::nullopt;                             \
      ret.emplace_back(std::move(val.value()));                              \
    }                                                                        \
    /*we took ownership of inner fields but still needs to destroy vector */ \
    cidlval_vec_destroy(values_vec);                                         \
    return std::make_optional<std::vector<type>>(std::move(ret));            \
  }                                                                          \
                                                                             \
  /******************** Public ***********************/

IdlValue::IdlValue(IdlValue &&o) noexcept : ptr(std::move(o.ptr)) {}

IdlValue &IdlValue::operator=(IdlValue &&o) noexcept {
  if (&o == this) return *this;

  ptr = std::move(o.ptr);

  return *this;
}

/******************** T -> IdlValue ***********************/

template <>
IdlValue::IdlValue(uint8_t value) {
  ptr.reset(idl_value_with_nat8(value));
}

template <>
IdlValue::IdlValue(uint16_t value) {
  ptr.reset(idl_value_with_nat16(value));
}

template <>
IdlValue::IdlValue(uint32_t value) {
  ptr.reset(idl_value_with_nat32(value));
}

template <>
IdlValue::IdlValue(uint64_t value) {
  ptr.reset(idl_value_with_nat64(value));
}

template <>
IdlValue::IdlValue(int8_t value) {
  ptr.reset(idl_value_with_int8(value));
}

template <>
IdlValue::IdlValue(int16_t value) {
  ptr.reset(idl_value_with_int16(value));
}

template <>
IdlValue::IdlValue(int32_t value) {
  ptr.reset(idl_value_with_int32(value));
}

template <>
IdlValue::IdlValue(int64_t value) {
  ptr.reset(idl_value_with_int64(value));
}

template <>
IdlValue::IdlValue(float value) {
  ptr.reset(idl_value_with_float32(value));
}

template <>
IdlValue::IdlValue(double value) {
  ptr.reset(idl_value_with_float64(value));
}

template <>
IdlValue::IdlValue(bool value) {
  ptr.reset(idl_value_with_bool(value));
}

template <>
IdlValue::IdlValue(std::string text) {
  // TODO: Use RetError
  ptr.reset(idl_value_with_text(text.c_str(), nullptr));
}

template <>
IdlValue::IdlValue(Number number) {
  // TODO: Use RetError
  // RetPtr_u8 error;

  ptr.reset(idl_value_with_number(number.value.c_str(), nullptr));
}

template <typename T, typename>
IdlValue::IdlValue(std::optional<T> val) {
  if (val.has_value()) {
    auto value = IdlValue(val.value());
    ptr.reset(idl_value_with_opt(value.ptr.release()));
  } else {
    ptr.reset(idl_value_with_none());
  }
}

template <typename T, typename>
IdlValue::IdlValue(const std::vector<T> &elems) {
  std::vector<const IDLValue *> cElems;
  cElems.reserve(elems.size());

  for (auto e : elems) {
    IdlValue val(e);
    cElems.push_back(val.ptr.release());
  }

  ptr.reset(idl_value_with_vec(cElems.data(), cElems.size()));
}

template <typename... Args, typename>
IdlValue::IdlValue(std::tuple<Args...> &tuple) {
  initializeFromTuple(tuple, std::index_sequence_for<Args...>());
}

template <typename... Args, typename, typename>
IdlValue::IdlValue(const std::variant<Args...> &variant) {
  std::visit(
      [this](auto &value) {
        IdlValue val(std::move(value));
        *this = IdlValue::FromVariant(std::string(value.__CANDID_VARIANT_NAME),
                                      &val, value.__CANDID_VARIANT_CODE);
      },
      variant);
}

template <>
IdlValue::IdlValue(IDLValue *ptr) : ptr(ptr){};

template <>
IdlValue::IdlValue(const IDLValue *ptr) : ptr((IDLValue *)ptr){};

template <>
IdlValue::IdlValue(zondax::Principal principal) {
  // TODO: Use RetError
  // RetPtr_u8 error;

  ptr.reset(idl_value_with_principal(principal.getBytes().data(),
                                     principal.getBytes().size(), nullptr));
}

template <>
IdlValue::IdlValue(zondax::Service service) {
  // TODO: Use RetError
  // RetPtr_u8 error;

  auto principal = service.principal_take();
  ptr.reset(idl_value_with_service(principal.getBytes().data(),
                                   principal.getBytes().size(), nullptr));
}

template <>
IdlValue::IdlValue(zondax::Func func) {
  auto princ = func.principal().getBytes();

  ptr.reset(idl_value_with_func(princ.data(), princ.size(),
                                func.method_name().c_str()));
}

template <>
IdlValue::IdlValue(std::monostate) {
  ptr.reset(idl_value_with_null());
}

IdlValue IdlValue::null(void) {
  IdlValue val;
  val.ptr.reset(idl_value_with_null());
  return val;
}

IdlValue IdlValue::reserved(void) {
  IdlValue val;
  val.ptr.reset(idl_value_with_reserved());
  return val;
}

IdlValue IdlValue::FromRecord(
    std::unordered_map<std::string, IdlValue> &fields) {
  std::vector<const char *> cKeys;
  cKeys.reserve(fields.size());
  std::vector<const IDLValue *> cElems;
  cElems.reserve(fields.size());

  for (auto &[key, value] : fields) {
    cKeys.push_back(key.c_str());
    cElems.push_back(value.ptr.release());
  }

  auto p = idl_value_with_record(cKeys.data(), cKeys.size(), cElems.data(),
                                 cElems.size(), false);
  return IdlValue(p);
}

// TODO: improve the constructors below

IdlValue IdlValue::FromVariant(std::string key, IdlValue *val, uint64_t code) {
  auto p = idl_value_with_variant(key.c_str(), val->ptr.release(), code);
  return IdlValue(p);
}

// ---------------------- IdlValue::get() specializations ---

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
std::optional<std::string> IdlValue::getImpl(
    helper::tag_type<std::string> type) {
  if (ptr == nullptr) return std::nullopt;

  CText *ctext = text_from_idl_value(ptr.get());
  if (ctext == nullptr) return std::nullopt;

  const char *str = ctext_str(ctext);
  if (str == nullptr) return std::nullopt;

  // use c null-terminated string constructor
  std::string s(str);

  ctext_destroy(ctext);

  return std::make_optional<std::string>(s);
}

template <>
std::optional<zondax::Principal> IdlValue::getImpl(
    helper::tag_type<zondax::Principal> type) {
  if (ptr == nullptr) return std::nullopt;

  CPrincipal *p = principal_from_idl_value(ptr.get());
  if (p == nullptr) return std::nullopt;

  std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
  zondax::Principal principal(bytes);

  principal_destroy(p);

  return std::make_optional(std::move(principal));
}

template <>
std::optional<Number> IdlValue::getImpl(helper::tag_type<Number> type) {
  if (ptr == nullptr) return std::nullopt;

  CText *ctext = number_from_idl_value(ptr.get());
  if (ctext == nullptr) return std::nullopt;

  const char *str = ctext_str(ctext);

  std::string text(str);

  ctext_destroy(ctext);
  Number number;
  number.value = text;

  return std::make_optional(number);
}

template <>
std::optional<zondax::Func> IdlValue::getImpl(helper::tag_type<Func> type) {
  if (ptr == nullptr) return std::nullopt;

  struct CFunc *cFunc = func_from_idl_value(ptr.get());
  if (cFunc == nullptr) return std::nullopt;

  // Extract principal
  struct CPrincipal *cPrincipal = cfunc_principal(cFunc);
  std::vector<uint8_t> vec =
      std::vector<uint8_t>(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);

  zondax::Func result(
      zondax::Principal(vec),
      std::string(cfunc_string(cFunc),
                  cfunc_string(cFunc) + cfunc_string_len(cFunc)));

  // Free the allocated CFunc
  cfunc_destroy(cFunc);

  return std::optional<zondax::Func>(std::move(result));
}

template <>
std::optional<std::vector<IdlValue>> IdlValue::getImpl(
    helper::tag_type<std::vector<IdlValue>> type) {
  if (ptr == nullptr) return std::nullopt;

  // call bellow creates a new IDLValue::Vec,
  // this means it "clones" from this.ptr
  struct CIDLValuesVec *cVec = vec_from_idl_value(ptr.get());
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
std::optional<zondax::Service> IdlValue::getImpl(
    helper::tag_type<zondax::Service> type) {
  if (ptr == nullptr) return std::nullopt;

  CPrincipal *p = service_from_idl_value(ptr.get());

  if (p == nullptr) return std::nullopt;

  std::vector<unsigned char> bytes(p->ptr, p->ptr + p->len);
  zondax::Principal principal(bytes);

  // there is not issue in destroying raw principal
  // because bytes makes a deep copy
  principal_destroy(p);

  return std::make_optional(zondax::Service(std::move(principal)));
}

template <>
std::optional<std::monostate> IdlValue::getImpl(
    helper::tag_type<std::monostate> type) {
  if (ptr == nullptr) return std::nullopt;

  return idl_value_is_null(ptr.get()) ? std::make_optional<std::monostate>()
                                      : std::nullopt;
}

std::optional<IdlValue> IdlValue::getOpt() {
  if (ptr == nullptr) return std::nullopt;

  IDLValue *result = opt_from_idl_value(ptr.get());

  if (result == nullptr) return std::nullopt;

  return IdlValue(result);
}

// Record specialization
// TODO: We need to add testing for this!!!!!
template <>
std::optional<std::unordered_map<std::string, IdlValue>>
IdlValue::getImpl<std::unordered_map<std::string, IdlValue>>(
    helper::tag_type<std::unordered_map<std::string, IdlValue>> t) {
  if (ptr.get() == nullptr) return std::nullopt;
  auto record = record_from_idl_value(ptr.get());
  if (record == nullptr) return std::nullopt;

  std::unordered_map<std::string, IdlValue> fields;

  auto len = crecord_keys_len(record);

  for (uintptr_t i = 0; i < len; ++i) {
    auto cKey = crecord_take_key(record, i);
    auto cVal = crecord_take_val(record, i);

    if (cKey == nullptr || cVal == nullptr) return std::nullopt;

    auto str = ctext_str(cKey);
    if (str == nullptr) return std::nullopt;

    auto key = std::string("");
    if (str != nullptr) {
      key = std::string(str);
    }

    IdlValue val(cVal);

    fields.emplace(std::make_pair(key, std::move(val)));
  }

  // crecord_destroy(record);

  return std::make_optional(std::move(fields));
}

VEC_PRIMITIVE_TYPES_GETTER(int8_t)
VEC_PRIMITIVE_TYPES_GETTER(int16_t)
VEC_PRIMITIVE_TYPES_GETTER(int32_t)
VEC_PRIMITIVE_TYPES_GETTER(int64_t)
VEC_PRIMITIVE_TYPES_GETTER(uint8_t)
VEC_PRIMITIVE_TYPES_GETTER(uint16_t)
VEC_PRIMITIVE_TYPES_GETTER(uint32_t)
VEC_PRIMITIVE_TYPES_GETTER(uint64_t)
VEC_PRIMITIVE_TYPES_GETTER(float)
VEC_PRIMITIVE_TYPES_GETTER(double)
VEC_PRIMITIVE_TYPES_GETTER(bool)
VEC_PRIMITIVE_TYPES_GETTER(std::string)
VEC_PRIMITIVE_TYPES_GETTER(zondax::Principal)
VEC_PRIMITIVE_TYPES_GETTER(zondax::Func)
VEC_PRIMITIVE_TYPES_GETTER(zondax::Service)
VEC_PRIMITIVE_TYPES_GETTER(std::monostate)

// TODO: Original getRecord, with some tweeks, to handle null values.
// do we keep this or the specialization above is enough?
std::unordered_map<std::string, IdlValue> IdlValue::getRecord() {
  std::unordered_map<std::string, IdlValue> fields;
  auto record = record_from_idl_value(ptr.get());

  if (record == nullptr) return fields;

  auto len = crecord_keys_len(record);

  for (uintptr_t i = 0; i < len; ++i) {
    auto cKey = crecord_take_key(record, i);
    auto cVal = crecord_take_val(record, i);

    if (cKey == nullptr || cVal == nullptr) return fields;

    auto str = ctext_str(cKey);
    if (str == nullptr) return fields;

    auto key = std::string("");
    if (str != nullptr) {
      // because str is null terminated, we should use the
      // string constructor for it.
      key = std::string(str);
    }

    IdlValue val(cVal);

    fields.emplace(std::make_pair(key, std::move(val)));
  }

  crecord_destroy(record);

  return std::move(fields);
}

std::unique_ptr<IDLValue> IdlValue::getPtr() { return std::move(ptr); }
}  // namespace zondax
//
using namespace zondax;

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to ints", T, test_id) {
  T integer = 100;
  IdlValue value(integer);
  auto back = value.get<T>();

  REQUIRE(back.has_value());
  REQUIRE(back.value() == integer);
}

TEST_CASE_TEMPLATE_INVOKE(test_id, uint8_t, uint16_t, uint32_t, uint64_t,
                          int8_t, int16_t, int32_t, int64_t);

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to float/double", T, test_id_floats) {
  T val = 1.5;
  IdlValue value(val);
  auto back = value.get<T>();

  REQUIRE(back.has_value());
  REQUIRE(back.value() == val);
}

TEST_CASE_TEMPLATE_INVOKE(test_id_floats, float, double);

TEST_CASE("IdlValue from/to string") {
  std::string str("Zondax");
  IdlValue value(str);
  auto back = value.get<std::string>();
  REQUIRE(back.has_value());
  REQUIRE(back.value() == str);
}

TEST_CASE("IdlValue from/to Vec<string>") {
  std::string str("Zondax");
  std::vector<std::string> vec{str, str, str, str, str};

  IdlValue value(vec);
  auto back = value.get<std::vector<std::string>>();
  REQUIRE(back.has_value());
  auto val = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(vec.begin(), vec.end(), std::begin(val)));
}

struct Peer_authentication {
  explicit Peer_authentication() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"authentication"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  std::string value;
};

struct Sender_report {
  explicit Sender_report() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"report"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  std::string report;
};

template <>
IdlValue::IdlValue(Peer_authentication peer) {
  std::unordered_map<std::string, IdlValue> fields;

  {
    auto name = std::string("value");
    auto val = IdlValue(std::move(peer.value));
    fields.emplace(std::make_pair(name, std::move(val)));
  }

  *this = std::move(IdlValue::FromRecord(fields));
}

template <>
IdlValue::IdlValue(Sender_report sender) {
  std::unordered_map<std::string, IdlValue> fields;

  {
    auto name = std::string("report");
    auto val = IdlValue(std::move(sender.report));
    fields.emplace(std::make_pair(name, std::move(val)));
  }

  *this = std::move(IdlValue::FromRecord(fields));
}

template <>
std::optional<Peer_authentication> IdlValue::getImpl(
    helper::tag_type<Peer_authentication> t) {
  Peer_authentication result;
  auto fields = this->getRecord();
  {
    auto field = std::move(fields["value"]);
    auto val = field.get<std::string>();

    if (val.has_value()) {
      result.value = std::move(val.value());
    } else {
      return std::nullopt;
    }
  }

  return std::make_optional(std::move(result));
}

template <>
std::optional<Sender_report> IdlValue::getImpl(
    helper::tag_type<Sender_report> t) {
  Sender_report result;
  auto fields = this->getRecord();
  {
    auto field = std::move(fields["report"]);
    auto val = field.get<std::string>();

    if (val.has_value()) {
      result.report = std::move(val.value());
    } else {
      return std::nullopt;
    }
  }

  return std::make_optional(std::move(result));
}

std::optional<std::pair<std::string_view, std::size_t>> IdlValue::asCVariant() {
  if (ptr == nullptr) return std::nullopt;

  CVariant *variant = variant_from_idl_value(ptr.get());
  if (variant == nullptr) return std::nullopt;

  auto str = cvariant_id(variant);
  if (str == nullptr) return std::nullopt;

  auto key = std::string(reinterpret_cast<const char *const>(str));

  //  auto key_len = cvariant_id_len(variant);
  auto code = cvariant_code(variant);

  return std::make_optional(std::make_pair(key, code));
}

TEST_CASE("IdlValue from/to std::variant<Peer_authentication, Sender_report>") {
  // construct variant
  Peer_authentication auth;
  auth.value = "Peer_authentication";
  std::variant<Peer_authentication, Sender_report> variant(std::move(auth));
  IdlValue obj(std::move(variant));

  auto back = obj.get<std::variant<Peer_authentication, Sender_report>>();
  // FIXME: This require must pass.
  REQUIRE(back.has_value());
  auto ret = back.value();
}

// TODO: Uncomment test, currently this does not compile due to constructor
TEST_CASE("IdlValue from/to std::tuple<std::string, std::string>") {
  std::string first("first");
  std::string second("second");

  // construct tuple
  std::tuple<std::string, std::string> tuple{first, second};

  IdlValue value(tuple);

  // auto back = value.get<std::unordered_map<std::string, IdlValue>>();
  auto map = value.getRecord();

  REQUIRE(map.size() == 2);

  for (auto &n : map) {
    auto val = n.second.get<std::string>();
    REQUIRE(val.has_value());
    auto str = val.value();

    if (n.first.compare("0") == 0) {
      REQUIRE(str == first);
    } else if (n.first.compare("1") == 0) {
      REQUIRE(str == second);
    }
  }
}

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to vector of ints", T,
                          test_id_vector) {
  std::vector<T> val{1, 2, 4, 5};
  IdlValue value(val);
  auto back = value.get<std::vector<T>>();

  REQUIRE(back.has_value());
  auto vec = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(vec.begin(), vec.end(), std::begin(val)));

  // check that it fails if comparing with another vector with different values
  std::vector<T> val2{6, 7, 8, 9};
  REQUIRE(!std::equal(vec.begin(), vec.end(), std::begin(val2)));
}

TEST_CASE_TEMPLATE_INVOKE(test_id_vector, uint8_t, uint16_t, uint32_t, uint64_t,
                          int8_t, int16_t, int32_t, int64_t);

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to vector of floats", T,
                          test_id_vector_floats) {
  std::vector<T> val{1.0, 2.04, 4.555, 5.89};
  IdlValue value(val);
  auto back = value.get<std::vector<T>>();

  REQUIRE(back.has_value());
  auto vec = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(vec.begin(), vec.end(), std::begin(val)));
}
TEST_CASE_TEMPLATE_INVOKE(test_id_vector_floats, float, double);

TEST_CASE("IdlValue from/to tuple") {
  std::tuple<uint32_t, std::string, float> tuple1{1000, std::string("tuple"),
                                                  25.5};
  // make a copy because in constructor impl, we move values
  auto copy = tuple1;
  IdlValue value(copy);

  auto back = value.get<std::tuple<uint32_t, std::string, float>>();

  REQUIRE(back.has_value());

  auto tuple2 = back.value();

  REQUIRE(std::get<0>(tuple2) == std::get<0>(tuple1));
  REQUIRE(std::get<1>(tuple2) == std::get<1>(tuple1));
  REQUIRE(std::get<2>(tuple2) == std::get<2>(tuple1));
}
