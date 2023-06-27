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
#include "principal.h"

#include <cstdlib>
#include <optional>

namespace zondax {

void Principal::error_callback(const unsigned char *data, int len,
                               void *user_data) {
  std::string error_msg((const char *)data, len);
  *(std::string *)user_data = error_msg;
}

// declare move constructor
Principal::Principal(Principal &&o) noexcept : bytes(std::move(o.bytes)) {
  cPrincipal = o.cPrincipal;
  o.cPrincipal = nullptr;
}

// declare move assignment
Principal &Principal::operator=(Principal &&o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  if (cPrincipal != nullptr) principal_destroy(cPrincipal);

  cPrincipal = o.cPrincipal;
  bytes = std::move(o.bytes);

  o.cPrincipal = nullptr;

  return *this;
}

Principal::Principal(const std::vector<unsigned char> &data) : bytes(data) {
  cPrincipal = principal_from_slice(data.data(), data.size());

  std::vector<unsigned char> outBytes(cPrincipal->ptr,
                                      cPrincipal->ptr + cPrincipal->len);
  bytes = outBytes;
}

Principal::Principal(bool anonym) {
  cPrincipal = anonym ? principal_anonymous() : principal_management_canister();

  std::vector<unsigned char> outBytes(cPrincipal->ptr,
                                      cPrincipal->ptr + cPrincipal->len);
  bytes = outBytes;
}

Principal Principal::SelfAuthenticating(
    const std::vector<uint8_t> &public_key) {
  CPrincipal *p =
      principal_self_authenticating(public_key.data(), public_key.size());

  std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
  principal_destroy(p);
  return Principal(outBytes);
}

std::variant<Principal, std::string> Principal::TryFromSlice(
    const std::vector<uint8_t> &bytes) {
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = Principal::error_callback;

  // get error from error_ret here and send it on string of result
  CPrincipal *p = principal_try_from_slice(bytes.data(), bytes.size(), &ret);

  if (p == nullptr) {
    std::variant<Principal, std::string> error(data);
    return error;
  }

  // there is not constructor variant that supports reading from pointers
  // use tradicional loob to push_back bytes
  std::vector<unsigned char> outBytes;
  for (int i = 0; i < p->len; ++i) {
    outBytes.push_back(*(p->ptr + i));
  }

  std::variant<Principal, std::string> ok{std::in_place_type<Principal>,
                                          outBytes};

  return ok;
}

std::variant<Principal, std::string> Principal::FromText(
    const std::string &text) {
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = Principal::error_callback;

  // get error from error_ret here and send it on string of result
  CPrincipal *p = principal_from_text(text.c_str(), &ret);

  if (p == nullptr) {
    std::variant<Principal, std::string> error(data);
    return error;
  }

  // there is not constructor variant that supports reading from pointers
  // use tradicional loob to push_back bytes
  std::vector<unsigned char> outBytes;
  for (int i = 0; i < p->len; ++i) {
    outBytes.push_back(*(p->ptr + i));
  }

  std::variant<Principal, std::string> ok{std::in_place_type<Principal>,
                                          outBytes};

  return ok;
}

std::string Principal::ToText(const std::vector<unsigned char> &data) {
  // get error from error_ret here and send it on string of result
  CPrincipal *p = principal_to_text(data.data(), data.size(), nullptr);

  if (p == nullptr) {
    return "";
  }

  std::string result(reinterpret_cast<const char *>(p->ptr), p->len);

  principal_destroy(p);

  return result;
}

std::vector<unsigned char> Principal::getBytes() const { return bytes; }

Principal::~Principal() {
  if (cPrincipal != nullptr) {
    principal_destroy(cPrincipal);
  }
}

}  // namespace zondax
