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
#ifndef IDL_ARGS_H
#define IDL_ARGS_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "idl_value.h"

extern "C" {
#include "zondax_ic.h"
}

namespace std {
template <>
struct default_delete<IDLArgs> {
  void operator()(IDLArgs *ptr) const {
    if (ptr != nullptr) idl_args_destroy(ptr);
  }
};
}  // namespace std

namespace zondax {
class IdlArgs {
 private:
  std::unique_ptr<IDLArgs> ptr;

 public:
  // Disable copies, just move semantics
  IdlArgs(const IdlArgs &args) = delete;
  void operator=(const IdlArgs &) = delete;

  // declare move constructor
  IdlArgs(IdlArgs &&o) noexcept;
  // declare move assignment
  IdlArgs &operator=(IdlArgs &&o) noexcept;

  // why this constructor?
  // does this takes ownership of memory pointed by argsPtr?
  explicit IdlArgs(IDLArgs *argsPtr);
  explicit IdlArgs(std::string text);
  explicit IdlArgs(std::vector<uint8_t> bytes);
  explicit IdlArgs(const std::vector<zondax::IdlValue> values);

  std::string getText();
  std::vector<uint8_t> getBytes();
  std::vector<zondax::IdlValue> getVec();

  std::unique_ptr<IDLArgs> getPtr();
};
}  // namespace zondax

#endif
