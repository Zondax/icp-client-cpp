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
#ifndef SERVICE_H
#define SERVICE_H
#include <cstdint>
#include <cstring>
#include <iostream>
#include <variant>
#include <vector>

#include "principal.h"

extern "C" {
#include "zondax_ic.h"
}

namespace zondax {
class Service {
 private:
  zondax::Principal princ;

 public:
  // Disable copies, just move semantics
  Service(const Service &args) = delete;
  void operator=(const Service &) = delete;

  /**
   * @brief Move constructor for Principal objects.
   *
   * @param o The Principal object to move.
   *
   * This constructor moves the content of the provided Principal object into a
   * new Principal object.
   */
  Service(Service &&o) noexcept;

  /**
   * @brief Move assignment operator for Principal objects.
   *
   * @param o The Principal object to move.
   *
   * This operator moves the content of the provided Principal object into an
   * existing Principal object.
   */
  Service &operator=(Service &&o) noexcept;

  /**
   * @brief Constructs a service by taking ownership of the passed principal
   * obj.
   *
   * @param o The Principal object to move.
   *
   * This operator moves the content of the provided Principal object into an
   * existing Principal object.
   */
  explicit Service(zondax::Principal &&principal) noexcept;

  /**
   * @brief Borrow the inner principal object.
   *
   * @return Reference to the Principal instance.
   */
  const zondax::Principal &principal() const;

  /**
   * @brief Take out the principal obj.
   *
   * @return The Principal instance.
   */
  zondax::Principal principal_take() noexcept;

  ~Service();
};
}  // namespace zondax
#endif  // SERVICE_H
