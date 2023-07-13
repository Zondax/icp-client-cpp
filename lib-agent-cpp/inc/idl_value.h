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
#ifndef IDL_VALUE_H
#define IDL_VALUE_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "func.h"
#include "idl_value_utils.h"
#include "service.h"
#include "zondax_ic.h"

extern "C" {
#include "zondax_ic.h"
}

namespace std {
template <>
struct default_delete<IDLValue> {
  void operator()(IDLValue *ptr) const {
    if (ptr != nullptr) idl_value_destroy(ptr);
  }
};
}  // namespace std

namespace zondax {

namespace helper {
template <typename T>
struct is_candid_variant {
  template <typename C>
  static constexpr auto test(int)
      -> decltype(std::declval<C>().__CANDID_VARIANT_NAME,
                  std::declval<C>().__CANDID_VARIANT_CODE, std::true_type{});

  template <typename>
  static constexpr std::false_type test(...);

  using type = decltype(test<T>(0));
  static constexpr bool value = type::value;
};

template <typename T>
inline constexpr bool is_candid_variant_v = is_candid_variant<T>::value;

// Helper to specialize IdlValue::get() method
template <typename... Args>
struct tag_type {};

// Helper struct to check whether a type is a specialization of std::variant
template <typename T>
struct is_variant : std::false_type {};

template <typename... T>
struct is_variant<std::variant<T...>> : std::true_type {};

// Helper struct to check whether a type is a specialization of std::tuple
template <typename T>
struct is_tuple : std::false_type {};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

// This is a helper for to use for checking that our
// variant field is valid for the expected return type
// when processing variants from idlValue in the
// getVariant method.
template <std::size_t I = 0, typename V, typename T>
constexpr bool static_for() {
  if constexpr (I < std::variant_size_v<V>) {
    // the field at I in the variant
    using VariantType = std::variant_alternative_t<I, V>;

    // VariantType value that we compute at runtime should be
    // at positiong I?
    // TODO: mmm we need to fix due to the shift the generator does?
    if (T::__CANDID_VARIANT_CODE == VariantType::__CANDID_VARIANT_CODE) {
      return true;
    } else {
      return static_for<I + 1, V, T>();
    }
  } else {
    return false;
  }
}

}  // namespace helper

struct Number {
  std::string value;
};

class IdlValue {
  friend class IdlArgs;

 private:
  std::unique_ptr<IDLValue> ptr;

  // Helper to initialize .ptr from an std::tuple-like set of items
  // used by IdlValue(std::tuple<Args...>) constructor
  template <typename Tuple, size_t... Indices>
  void initializeFromTuple(const Tuple &tuple, std::index_sequence<Indices...>);

  template <typename T>
  std::optional<T> getImpl(helper::tag_type<T> t) {
    // Your default implementation here
    return std::nullopt;
  }

  // Fallback function for non-variant, non-tuples, non-map-like
  // types
  template <typename T>
  std::optional<T> getHelper(std::false_type, std::false_type) {
    return getImpl<T>(helper::tag_type<T>{});
  }

  // Specialization for variant
  template <typename T>
  auto getHelper(std::true_type, std::false_type) {
    return getVariant<T>();
  }

  // Specialization for tuple
  template <typename T>
  std::optional<T> getHelper(std::false_type, std::true_type) {
    auto optTuple = getTuple<T>();
    if (!optTuple.has_value()) return std::nullopt;

    auto tup = std::move(optTuple.value());
    return std::optional<T>{std::get<0>(tup)};
  }

  template <typename... Ts>
  std::optional<std::tuple<Ts...>> getTuple() {
    // tuples are in reality a list or records, we have an specialization
    // for auto r = obj.get<unordered_map<string, idlValue>>();
    // so lets use that here;
    auto records = get<std::unordered_map<std::string, IdlValue>>();
    if (!records.has_value()) return std::nullopt;
    // now we need to iterate over the records and "push" each value at its
    // corresponding index in the tuple, this index is define by the key in the
    // map but this is imposible due to how tuple is designed
    return std::optional<std::tuple<Ts...>>{};  // Return an empty std::optional
  }

  template <typename V>
  std::optional<V> getVariant() {
    if (ptr == nullptr) return std::nullopt;

    CVariant *cvariant = variant_from_idl_value(ptr.get());
    if (cvariant == nullptr) return std::nullopt;
    // start with the key.
    auto id_ptr = cvariant_id(cvariant);
    auto id_len = cvariant_id_len(cvariant);

    auto value = cvariant_idlvalue(cvariant);
    auto code = cvariant_code(cvariant);
    if (id_ptr == nullptr || value == nullptr) return std::nullopt;

    auto key = std::string(id_ptr, id_ptr + id_len);
    IdlValue idl_value(value);

    // variant;
    V variant;
    bool success = false;
    std::visit(
        [&](auto &&arg) -> std::optional<V> {
          using T = std::decay_t<decltype(arg)>;

          if (T::__CANDID_VARIANT_CODE == code &&
              key.compare(T::__CANDID_VARIANT_NAME) == 0) {
            auto inner_value = get<T>();
            if (inner_value.has_value()) {
              if (helper::static_for<0, V, T>()) {
                cvariant_destroy(cvariant);
                return std::optional<V>(V(std::move(*inner_value)));
              }
            }
          }
          return std::nullopt;
        },
        V{});
    cvariant_destroy(cvariant);

    return success ? std::optional<V>(V{}) : std::nullopt;
  }

 public:
  // Disable copies, just move semantics
  IdlValue(const IdlValue &args) = delete;
  void operator=(const IdlValue &) = delete;

  // declare move constructor & assignment
  IdlValue(IdlValue &&o) noexcept;
  IdlValue &operator=(IdlValue &&o) noexcept;

  /******************** Constructors ***********************/
  template <typename T>
  explicit IdlValue(T);
  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<IdlValue, T>>>
  explicit IdlValue(std::optional<T>);

  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<IdlValue, T>>>
  explicit IdlValue(const std::vector<T> &);

  template <typename... Args,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  explicit IdlValue(const std::tuple<Args...> &);

  template <
      typename... Args,
      typename = std::enable_if_t<(helper::is_candid_variant_v<Args> && ...)>,
      typename =
          std::enable_if_t<(std::is_constructible_v<IdlValue, Args> && ...)>>
  explicit IdlValue(const std::variant<Args...> &);

  // Specific constructors
  explicit IdlValue() : ptr(nullptr) {}

  static IdlValue null();
  static IdlValue reserved();

  static IdlValue FromRecord(std::unordered_map<std::string, IdlValue> &fields);
  static IdlValue FromVariant(std::string key, IdlValue *val, uint64_t code);
  static IdlValue FromFunc(std::vector<uint8_t> vector, std::string func_name);

  /******************** Getters ***********************/

  template <typename T>
  std::optional<T> get() {
    return getHelper<T>(helper::is_variant<T>{}, helper::is_tuple<T>{});
  }

  template <
      template <typename, typename> class T, typename U, typename V,
      std::enable_if_t<
          std::is_invocable_v<decltype(&IdlValue::get<U>), IdlValue &>, U>,
      std::enable_if_t<
          std::is_invocable_v<decltype(&IdlValue::get<V>), IdlValue &>, V>,
      std::enable_if_t<!std::is_same_v<T, std::variant<U, V>>>>
  T<U, V> get() {
    return getImpl(helper::tag_type<T<U, V>>{});
  }

  template <template <typename, typename> class T, typename U, typename V>
  std::optional<T<U, V>> get(helper::tag_type<T<U, V>>) {
    return std::nullopt;
  }

  std::optional<IdlValue> getOpt();
  std::unordered_map<std::string, IdlValue> getRecord();

  std::unique_ptr<IDLValue> getPtr();
};

/******************** Private ***********************/

template <typename Tuple, size_t... Indices>
void IdlValue::initializeFromTuple(const Tuple &tuple,
                                   std::index_sequence<Indices...>) {
  std::vector<IDLValue *> values;
  std::vector<uint8_t[4]> indices;

  (
      [&](size_t index) {
        IdlValue val(std::get<index>(tuple));
        values.push_back(val.ptr.release());
        val.ptr = nullptr;

        uint8_t bytes[4];  // Array to store the resulting bytes
        auto value = static_cast<uint32_t>(index);

        bytes[0] = value & 0xFF;
        bytes[1] = (value >> 8) & 0xFF;
        bytes[2] = (value >> 16) & 0xFF;
        bytes[3] = (value >> 24) & 0xFF;

// If the platform is big endian, swap the byte order
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::swap(bytes[0], bytes[3]);
        std::swap(bytes[1], bytes[2]);
#endif
        indices.push_back(bytes);
      }(Indices),
      ...);

  ptr.reset(idl_value_with_record(
      reinterpret_cast<const char *const *>(indices.data()), indices.size(),
      values.data(), values.size(), true));
}

}  // namespace zondax

#endif
