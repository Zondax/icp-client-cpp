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
#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <variant>
#include <vector>

extern "C" {
#include "zondax_ic.h"
}

namespace zondax {
class Principal {
 private:
  std::vector<unsigned char> bytes;
  CPrincipal *cPrincipal;

  /**
   * @brief Callback function for handling errors.
   *
   * @param data The error message data.
   * @param len The length of the error message data.
   * @param user_data User data for error handling.
   *
   * This function is a callback that handles errors. It is used internally in
   * the Principal class.
   */
  static void error_callback(const unsigned char *data, int len,
                             void *user_data);

 public:
  // Disable copies, just move semantics
  Principal(const Principal &args) = delete;
  void operator=(const Principal &) = delete;

  /**
   * @brief Move constructor for Principal objects.
   *
   * @param o The Principal object to move.
   *
   * This constructor moves the content of the provided Principal object into a
   * new Principal object.
   */
  Principal(Principal &&o) noexcept;

  /**
   * @brief Move assignment operator for Principal objects.
   *
   * @param o The Principal object to move.
   * @return Reference to the moved Principal object.
   *
   * This operator moves the content of the provided Principal object into an
   * existing Principal object.
   */
  Principal &operator=(Principal &&o) noexcept;

  /**
   * @brief Constructs a Principal object.
   *
   * @param anonym Whether the Principal should be anonymous.
   *
   * This constructor initializes a Principal object, either as an anonymous
   * Principal or a management canister Principal. The `anonym` parameter
   * determines the type of Principal to construct.
   */
  explicit Principal(bool anonym = true);

  /**
   * @brief Constructs a Principal object from a vector of bytes.
   *
   * @param data The vector of bytes to initialize the Principal.
   *
   * This constructor initializes a Principal object with the provided vector of
   * bytes. The bytes parameter contains the data used to construct the
   * Principal.
   */
  explicit Principal(const std::vector<uint8_t> &bytes);

  /**
   * @brief Destructor for Principal objects.
   *
   * This destructor cleans up the memory allocated for the Principal object.
   */
  ~Principal();

  /**
   * @brief Constructs a self-authenticating Principal object.
   *
   * @param public_key The public key used for self-authentication.
   * @return The constructed Principal object.
   *
   * This function constructs a self-authenticating Principal object using the
   * provided public key. The public key is used for self-authentication of the
   * Principal.
   */
  static Principal SelfAuthenticating(const std::vector<uint8_t> &public_key);

  /**
   * @brief Tries to construct a Principal object from a slice of bytes.
   *
   * @param bytes The slice of bytes to initialize the Principal.
   * @return Either the constructed Principal object or an error message as a
   * string.
   *
   * This function tries to construct a Principal object from the provided slice
   * of bytes. It returns either the constructed Principal object or an error
   * message if the construction fails.
   */
  static std::variant<Principal, std::string> TryFromSlice(
      const std::vector<uint8_t> &bytes);

  /**
   * @brief Constructs a Principal object from a text string.
   *
   * @param text The text string to initialize the Principal.
   * @return Either the constructed Principal object or an error message as a
   * string.
   *
   * This function constructs a Principal object from the provided text string.
   * It returns either the constructed Principal object or an error message if
   * the construction fails.
   */
  static std::variant<Principal, std::string> FromText(const std::string &text);

  /**
   * @brief Converts the Principal object to a text string.
   *
   * @param data The Principal object to convert to text.
   * @return The text representation of the Principal object.
   *
   * This function converts the provided Principal object to a text string
   * representation. It returns the resulting text representation.
   */
  static std::string ToText(const std::vector<uint8_t> &bytes);

  /**
   * @brief Retrieves the bytes of the Principal object.
   *
   * @return The bytes of the Principal object.
   *
   * This function returns the bytes of the Principal object as a vector of
   * unsigned characters.
   */
  std::vector<unsigned char> getBytes() const;
};

}  // namespace zondax
#endif  // PRINCIPAL_H
