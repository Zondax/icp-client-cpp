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
#include <type_traits>
#include <vector>

#include "idl_value_utils.h"

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

  template <typename... Args,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  explicit IdlValue(const std::variant<Args...> &);

  // Specific constructors
  explicit IdlValue() : ptr(nullptr) {}

  static IdlValue null();
  static IdlValue reserved();

  static IdlValue FromRecord(std::vector<std::pair<std::string, IdlValue>> &);
  static IdlValue FromVariant(std::string key, IdlValue *val, uint64_t code);
  static IdlValue FromFunc(std::vector<uint8_t> vector, std::string func_name);

  /******************** Getters ***********************/
  template <typename T>
  std::optional<T> get();

  std::optional<IdlValue> getOpt();
  std::vector<IdlValue> getVec();
  // zondax::idl_value_utils::Record getRecord();
  // zondax::idl_value_utils::Variant getVariant();

  std::unique_ptr<IDLValue> getPtr();
};
}  // namespace zondax

#endif
