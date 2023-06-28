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
#include <optional>
#include <vector>

#include "idl_value_utils.h"
#include "principal.h"

extern "C" {
#include "zondax_ic.h"
}

namespace zondax {

struct Number {
  std::string value;
};

class IdlValue {
  friend class IdlArgs;

 private:
  IDLValue *ptr;
  // this will set inner pointer to null to avoid
  // calling destructor over it, this is useful when we
  // sent the original this.ptr to a c function that takes
  // ownership of passed value, so that we avoid double-free
  // errors by reset value to nullptr
  void resetValue();

 public:
  // Disable copies, just move semantics
  IdlValue(const IdlValue &args) = delete;
  void operator=(const IdlValue &) = delete;

  // declare move constructor
  IdlValue(IdlValue &&o) noexcept;
  // declare move assignment
  IdlValue &operator=(IdlValue &&o) noexcept;

  // Create IdlValues from types
  IdlValue(IDLValue *ptr);
  IdlValue();
  explicit IdlValue(uint8_t val);
  explicit IdlValue(uint16_t val);
  explicit IdlValue(uint32_t val);
  explicit IdlValue(uint64_t val);
  explicit IdlValue(int8_t val);
  explicit IdlValue(int16_t val);
  explicit IdlValue(int32_t val);
  explicit IdlValue(int64_t val);
  explicit IdlValue(float val);
  explicit IdlValue(double val);
  explicit IdlValue(bool val);
  explicit IdlValue(std::string text);
  explicit IdlValue(zondax::Principal principal,
                    bool type = true);  // true create principal false: service
  IdlValue FromNull(void);
  IdlValue FromNone(void);
  IdlValue FromReserved(void);
  IdlValue FromNumber(std::string number);
  IdlValue FromOpt(IdlValue *value);
  IdlValue FromVec(const std::vector<const IdlValue *> &elems);
  IdlValue FromRecord(const std::vector<std::string> &keys,
                      const std::vector<const IdlValue *> &elems);
  IdlValue FromVariant(std::string key, IdlValue *val, uint64_t code);
  IdlValue FromFunc(std::vector<uint8_t> vector, std::string func_name);

  // Get types
  template <typename T>
  std::optional<T> get();

  std::optional<zondax::Principal> getService();
  std::optional<IdlValue> getOpt();
  std::vector<IdlValue> getVec();
  // zondax::idl_value_utils::Record getRecord();
  // zondax::idl_value_utils::Variant getVariant();

  // methods bellow must be use carefully
  // the first will aliase pointer, breaking move semantics only
  // for this type if used wrong.
  IDLValue *getPtr() const;

  ~IdlValue();
};
}  // namespace zondax

#endif
