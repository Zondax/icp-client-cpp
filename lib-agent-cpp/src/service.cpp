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
#include "service.h"

#include <cstdlib>
#include <optional>

namespace zondax {

// declare move constructor
Service::Service(Service &&o) noexcept : princ(std::move(o.princ)) {}

// declare move assignment
Service &Service::operator=(Service &&o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  princ = std::move(o.princ);

  return *this;
}

Service::Service(zondax::Principal &&p) noexcept : princ(std::move(p)) {}

Service::~Service() {}

// zondax::Principal &Service::principal() noexcept { return principal; }
const zondax::Principal &Service::principal() const { return princ; }

zondax::Principal Service::principal_take() noexcept {
  return std::move(princ);
}

}  // namespace zondax
