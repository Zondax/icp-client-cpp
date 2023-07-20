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

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
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

// Helper types to expand std::variant<Args...>
// and create a tuple of its types.iterating over it.
template <typename... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

// A helper to convert a tuple to a variant
template <typename T>
struct tuple_to_variant;
template <typename... Ts>
struct tuple_to_variant<std::tuple<Ts...>> {
  using type = std::variant<Ts...>;
};

// A helper to prepend an element to a tuple, it would be the monostate
template <typename T, typename Tuple>
struct tuple_prepend;
template <typename T, typename... Ts>
struct tuple_prepend<T, std::tuple<Ts...>> {
  using type = std::tuple<T, Ts...>;
};

// Get the variant's types as a tuple
template <typename V>
struct variant_to_tuple;
template <typename... Ts>
struct variant_to_tuple<std::variant<Ts...>> {
  using type = std::tuple<Ts...>;
};

template <typename V>
using VariantWithMonostate = typename tuple_to_variant<typename tuple_prepend<
    std::monostate, typename variant_to_tuple<V>::type>::type>::type;

// This static for iterates over the variant types,
// and returns the position at which the type T
// is found in the variant. otherwise returns nullopt
template <typename V, typename T, std::size_t I = 0>
constexpr std::optional<std::size_t> static_for() {
  if constexpr (I < std::variant_size_v<V>) {
    if constexpr (std::is_same_v<std::variant_alternative_t<I, V>, T>) {
      return I;
    } else {
      return static_for<V, T, I + 1>();
    }
  }
  return std::nullopt;
}

template <std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type for_each(
    std::tuple<Tp...> &, FuncT)  // Unused arguments are given no names.
{}

template <std::size_t I = 0, typename FuncT, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...(Tp), void>::type for_each(std::tuple<Tp...> &t, FuncT f) {
  f(std::get<I>(t), std::integral_constant<size_t, I>());
  for_each<I + 1, FuncT, Tp...>(t, f);
}

template <typename V>
using VariantWithMonostate = typename tuple_to_variant<typename tuple_prepend<
    std::monostate, typename variant_to_tuple<V>::type>::type>::type;
template <typename T>
struct tuple_to_types;  // forward declare the template

template <typename... Ts>
struct tuple_to_types<std::tuple<Ts...>> {
  template <template <typename...> typename U>
  using rebind = U<Ts...>;
};

template <typename Tuple, template <typename...> typename U>
using tuple_to_types_t = typename tuple_to_types<Tuple>::template rebind<U>;

}  // namespace helper

struct Number {
  std::string value;
};

class IdlValue {
  friend class IdlArgs;

 private:
  template <typename T>
  struct callGetVariant;
  template <typename... Ts>
  struct callGetVariant<std::variant<Ts...>> {
    static std::optional<std::variant<Ts...>> call(IdlValue *val) {
      constexpr auto indices = std::index_sequence_for<Ts...>{};
      return val->getVariant<Ts...>(indices);
    }
  };

  std::unique_ptr<IDLValue> ptr;

  // Helper to initialize .ptr from an std::tuple-like set of items
  // used by IdlValue(std::tuple<Args...>) constructor
  template <typename Tuple, size_t... Indices>
  void initializeFromTuple(Tuple &tuple, std::index_sequence<Indices...>);

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
  std::optional<T> getHelper(std::true_type, std::false_type) {
    return callGetVariant<T>::call(this);
  }

  // Specialization for tuple
  template <typename T, typename Tuple, std::size_t... I>
  std::optional<T> getTupleHelper(Tuple &&, std::index_sequence<I...>) {
    return getTuple<std::tuple_element_t<I, T>...>();
  }

  // Specialization for tuple
  template <typename T>
  std::optional<T> getHelper(std::false_type, std::true_type) {
    auto optTuple = getTupleHelper<T>(
        T{}, std::make_index_sequence<std::tuple_size_v<T>>{});
    if (!optTuple.has_value()) return std::nullopt;
    return optTuple;
  }

  template <typename... Ts, std::size_t... Is>
  static std::optional<std::tuple<Ts...>> vectorToTupleImpl(
      std::vector<IdlValue> &vector, std::index_sequence<Is...>) {
    using variant_t = std::tuple<std::optional<Ts>...>;
    variant_t variantVector{vector[Is].get<Ts>()...};

    // all this because std::move(std::nullopt) is undefined behavior in C++
    if (!std::apply([](auto &&...opt) { return ((opt.has_value()) && ...); },
                    variantVector)) {
      return std::nullopt;
    }
    std::tuple<Ts...> tuple{std::move(*(std::get<Is>(variantVector)))...};

    return std::optional<std::tuple<Ts...>>{std::move(tuple)};
  }

  template <typename... Ts>
  std::optional<std::tuple<Ts...>> vectorToTuple(
      std::vector<IdlValue> &vector) {
    return std::move(
        vectorToTupleImpl<Ts...>(vector, std::index_sequence_for<Ts...>{}));
  }

  template <typename... Ts>
  std::optional<std::tuple<Ts...>> getTuple() {
    // tuples are in reality a list or records, we have an specialization for
    // it, so lets use that here;
    auto records = this->get<std::unordered_map<std::string, IdlValue>>();

    if (!records.has_value()) return std::nullopt;

    auto values = std::move(records.value());

    std::vector<int> keys;
    for (auto &pair : values) {
      keys.push_back(std::stoi(pair.first));
    }

    // Sort the keys, yes why? because std::sort does not support sorting
    // move-only types
    std::sort(keys.begin(), keys.end());

    // Create a vector of IdlValue objects in the order defined by keys
    std::vector<IdlValue> vecValues;

    for (int key : keys) {
      vecValues.push_back(std::move(values[std::to_string(key)]));
    }
    return std::move(vectorToTuple<Ts...>(vecValues));
  }

  // Attempt to extract the Indexth variant type from the CVariant
  template <typename Variant, std::size_t Index>
  bool tryVariant(std::optional<Variant> &loc, std::string_view key,
                  std::size_t code) {
    using T = std::variant_alternative_t<Index, Variant>;

    if (loc.has_value()) return false;

    // check code & key match
    if (T::__CANDID_VARIANT_NAME != key || T::__CANDID_VARIANT_CODE != code)
      return false;

    // get to obtain the inner type
    auto v = get<T>();
    if (!v.has_value()) return false;

    // initialize loc with the proper type
    loc.emplace(std::in_place_index<Index>, std::move(v.value()));
    return true;
  }

  template <typename... Ts, std::size_t... Indices>
  std::optional<std::variant<Ts...>> getVariant(
      std::index_sequence<Indices...>) {
    using Variant = std::variant<Ts...>;

    auto cvariant_data = this->asCVariant();
    if (!cvariant_data.has_value()) return std::nullopt;

    auto [key, code] = cvariant_data.value();
    std::optional<Variant> result;

    (tryVariant<Variant, Indices>(result, key, code), ...);

    return result;
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
  explicit IdlValue(std::tuple<Args...> &);

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
  std::optional<std::pair<std::string_view, std::size_t>> asCVariant();
};

/******************** Private ***********************/

template <typename Tuple, size_t... Indices>
void IdlValue::initializeFromTuple(Tuple &tuple,
                                   std::index_sequence<Indices...>) {
  std::vector<IDLValue *> values;
  std::vector<std::array<uint8_t, 4>> indices;

  auto func = [&](auto &tp, auto index) {
    IdlValue val(std::move(tp));
    values.push_back(val.ptr.release());

    // Array to store the resulting bytes
    std::array<uint8_t, 4> bytes;
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
  };
  helper::for_each(tuple, func);

  std::vector<const char *> cstr_indices(indices.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    cstr_indices[i] = reinterpret_cast<const char *>(indices[i].data());
  }

  ptr.reset(idl_value_with_record(cstr_indices.data(), indices.size(),
                                  values.data(), values.size(), true));
}

}  // namespace zondax

#endif
