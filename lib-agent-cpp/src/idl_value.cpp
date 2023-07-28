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

/******************** Public ***********************/

IdlValue::IdlValue(IdlValue &&o) noexcept : ptr(std::move(o.ptr)) {}

IdlValue &IdlValue::operator=(IdlValue &&o) noexcept {
  if (&o == this) return *this;

  ptr = std::move(o.ptr);

  return *this;
}

IdlValueType IdlValue::type() {
  if (ptr.get() == nullptr) return IdlValueType::Null;

  uint32_t ty = idl_value_type(ptr.get());

  auto max = static_cast<uint8_t>(IdlValueType::Reserved);

  if (ty > max) return static_cast<IdlValueType>(IdlValueType::Invalid);

  return static_cast<IdlValueType>(ty);
}

/******************** T -> IdlValue ***********************/

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

std::optional<std::tuple<std::string, std::size_t, IdlValue>>
IdlValue::asCVariant() {
  if (ptr == nullptr) return std::nullopt;

  CVariant *variant = variant_from_idl_value(ptr.get());
  if (variant == nullptr) return std::nullopt;

  auto str = cvariant_id(variant);
  if (str == nullptr) return std::nullopt;

  std::string key((const char *)str);

  auto code = cvariant_code(variant);

  IdlValue idlvalue(cvariant_idlvalue(variant));

  return std::make_optional(std::make_tuple(key, code, std::move(idlvalue)));
}

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

std::optional<IdlValue> IdlValue::getOpt() {
  if (ptr == nullptr) return std::nullopt;

  IDLValue *result = opt_from_idl_value(ptr.get());

  if (result == nullptr) return std::nullopt;

  return IdlValue(result);
}

std::unique_ptr<IDLValue> IdlValue::getPtr() { return std::move(ptr); }

}  // namespace zondax

// ------------------------------------------------- TESTS
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
  auto copy = vec;

  IdlValue value(std::move(vec));
  auto back = value.get<std::vector<std::string>>();
  REQUIRE(back.has_value());
  auto val = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(copy.begin(), copy.end(), std::begin(val)));
}

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to vector of ints", T,
                          test_id_vector) {
  std::vector<T> val{1, 2, 4, 5};
  IdlValue value(std::move(val));
  auto back = value.get<std::vector<T>>();

  REQUIRE(back.has_value());
  auto vec = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(vec.begin(), vec.end(), std::begin(val)));

  // check that it fails if comparing with another vector with different
  // values
  std::vector<T> val2{6, 7, 8, 9};
  REQUIRE(!std::equal(vec.begin(), vec.end(), std::begin(val2)));
}

TEST_CASE_TEMPLATE_INVOKE(test_id_vector, uint8_t, uint16_t, uint32_t, uint64_t,
                          int8_t, int16_t, int32_t, int64_t);

TEST_CASE_TEMPLATE_DEFINE("IdlValue from/to vector of floats", T,
                          test_id_vector_floats) {
  std::vector<T> val{1.0, 2.04, 4.555, 5.89};
  IdlValue value(std::move(val));
  auto back = value.get<std::vector<T>>();

  REQUIRE(back.has_value());
  auto vec = back.value();
  REQUIRE(vec.size() == val.size());
  REQUIRE(std::equal(vec.begin(), vec.end(), std::begin(val)));
}
TEST_CASE_TEMPLATE_INVOKE(test_id_vector_floats, float, double);

TEST_CASE("IdlValue from/to tuple") {
  // Get Principal from slice of bytes
  std::vector<uint8_t> slice = {0x1};

  zondax::Principal principal(slice);
  // construct tuple
  std::tuple<uint8_t, uint64_t, double, std::string, zondax::Principal> tuple =
      std::make_tuple(1, 100500, 2.5, std::string("zondax"),
                      std::move(principal));

  IdlValue value(std::move(tuple));

  auto back = value.get<
      std::tuple<uint8_t, uint64_t, double, std::string, zondax::Principal>>();
  REQUIRE(back.has_value());

  auto tuple2 = std::move(back.value());

  REQUIRE(1 == std::get<0>(tuple2));
  REQUIRE(100500 == std::get<1>(tuple2));
  REQUIRE(2.5 == std::get<2>(tuple2));
  REQUIRE(std::string("zondax") == std::get<3>(tuple2));

  auto p = std::move(std::get<4>(tuple2));
  auto bytes = p.getBytes();

  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == 0x01);
}

TEST_CASE("IdlValue from/to vector<std::tuple<Args...>>") {
  // construct tuple
  std::vector<std::tuple<uint8_t, uint64_t, double, std::string, bool>> vec;
  for (int i = 0; i < 10; ++i) {
    std::tuple<uint8_t, uint64_t, double, std::string, bool> tuple =
        std::make_tuple((uint8_t)i, 100500, 2.5, std::string("zondax"),
                        (i % 2 == 0));
    vec.push_back(tuple);
  }

  auto copy = vec;

  IdlValue value(std::move(vec));

  auto back = value.get<
      std::vector<std::tuple<uint8_t, uint64_t, double, std::string, bool>>>();

  REQUIRE(back.has_value());
  auto vec2 = back.value();
  REQUIRE(copy.size() == vec2.size());
  auto equal = copy == vec2;
  // REQUIRE(true == equal);
  REQUIRE(copy == vec2);
}

TEST_CASE("IdlValue from/to std::optional<T>") {
  uint32_t num = 100;
  std::optional<uint32_t> op(num);
  IdlValue value(op);

  auto back = value.get<std::optional<uint32_t>>();
  REQUIRE(back.has_value());
  auto inner = back.value();
  REQUIRE(inner.has_value());
  auto result = inner.value();
  REQUIRE(result == num);

  SUBCASE("Inner optional null") {
    std::optional<uint32_t> val;
    val = std::nullopt;
    IdlValue value(val);

    auto back = value.get<std::optional<uint32_t>>();
    REQUIRE(back.has_value());
    auto inner = back.value();
    REQUIRE(!inner.has_value());
  }
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
    auto val = IdlValue(peer.value);
    fields.emplace(std::make_pair(name, std::move(val)));
  }

  *this = std::move(IdlValue::FromRecord(fields));
}

template <>
IdlValue::IdlValue(Sender_report sender) {
  std::unordered_map<std::string, IdlValue> fields;

  {
    auto name = std::string("report");
    auto val = IdlValue(sender.report);
    fields.emplace(std::make_pair(name, std::move(val)));
  }

  *this = std::move(IdlValue::FromRecord(fields));
}

template <>
std::optional<Peer_authentication> IdlValue::getImpl(
    helper::tag_type<Peer_authentication> t) {
  Peer_authentication result;

  auto fields = getRecord();
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

  auto fields = getRecord();
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

TEST_CASE("IdlValue from/to std::variant<Peer_authentication, Sender_report>") {
  // construct variant
  Peer_authentication auth;
  auth.value = "Peer_authentication";
  std::variant<Peer_authentication, Sender_report> variant(std::move(auth));
  IdlValue obj(std::move(variant));

  auto back = obj.get<std::variant<Peer_authentication, Sender_report>>();
  REQUIRE(back.has_value());
  auto var = back.value();
  REQUIRE(std::holds_alternative<Peer_authentication>(var));
  auto peer = std::get<Peer_authentication>(var);
  REQUIRE(peer.value.compare(auth.value));

  auto back2 = obj.get<std::variant<Sender_report, Peer_authentication>>();
  REQUIRE(back2.has_value());
  auto var2 = back2.value();
  REQUIRE(std::holds_alternative<Peer_authentication>(var2));
  auto peer2 = std::get<Peer_authentication>(var2);
  REQUIRE(peer2.value.compare(auth.value));
}
