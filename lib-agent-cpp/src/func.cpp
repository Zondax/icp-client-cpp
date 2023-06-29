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
#include "func.h"

#include <cstdlib>
#include <optional>

namespace zondax {

// declare move constructor
Func::Func(Func &&o) noexcept
    : princ(std::move(o.princ)), method(std::move(o.method)) {}

// declare move assignment
Func &Func::operator=(Func &&o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  princ = std::move(o.princ);
  method = std::move(o.method);

  return *this;
}

Func::Func(zondax::Principal &&p, std::string &&m) noexcept
    : princ(std::move(p)), method(std::move(m)) {}

// zondax::Principal &Service::principal() noexcept { return principal; }
const zondax::Principal &Func::principal() const { return princ; }
const std::string &Func::method_name() const { return method; }

zondax::Principal Func::principal_take() noexcept { return std::move(princ); }

}  // namespace zondax
