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
#ifndef FUNC_H
#define FUNC_H
#include <string>

#include "principal.h"

extern "C" {
#include "zondax_ic.h"
}

namespace zondax {
class Func {
 private:
  zondax::Principal princ;
  std::string method;

 public:
  Func() : princ(){};

  // Disable copies, just move semantics
  Func(const Func &args) = delete;
  void operator=(const Func &) = delete;

  /**
   * @brief Move constructor for Func objects.
   *
   * @param o The Func object to move.
   *
   * This constructor moves the content of the provided Func object into a
   * new Func object.
   */
  Func(Func &&o) noexcept;

  /**
   * @brief Move assignment operator for Func objects.
   *
   * @param o The Func object to move.
   *
   * This operator moves the content of the provided Func object into an
   * existing Func object.
   */
  Func &operator=(Func &&o) noexcept;

  /**
   * @brief Constructs a service by taking ownership of the passed principal
   * obj.
   *
   * @param principal The Principal object to move.
   *
   * This operator moves the content of the provided Principal object and string
   * into an a new Func object.
   */
  explicit Func(zondax::Principal &&principal,
                std::string &&method_name) noexcept;

  /**
   * @brief Borrow the inner principal object.
   *
   * @return Reference to the Principal instance.
   */
  const zondax::Principal &principal() const;

  /**
   * @brief Borrow the inner method name.
   *
   * @return Reference to the method name.
   */
  const std::string &method_name() const;

  /**
   * @brief Take out the principal obj.
   *
   * @return The Principal instance.
   */
  zondax::Principal principal_take() noexcept;

  ~Func();
};
}  // namespace zondax
#endif  // FUNC_H
