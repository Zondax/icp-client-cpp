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
#ifndef IDL_VALUE_UTILS_H
#define IDL_VALUE_UTILS_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include "principal.h"

namespace zondax {

class IdlValue;

struct Record {
  std::vector<std::string> keys;
  std::vector<std::unique_ptr<IdlValue *>> vals;
};

struct Variant {
  std::vector<uint8_t> id;
  IdlValue *val;
  uint64_t code;
};

}  // namespace zondax

#endif
