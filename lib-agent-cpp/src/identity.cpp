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
#include "identity.h"

namespace zondax {

void error_callback(const unsigned char *data, int len, void *user_data) {
  std::string error_msg((const char *)data, len);
  *(std::string *)user_data = error_msg;
}

// declare move constructor
Identity::Identity(Identity &&o) noexcept : ptr(o.ptr), type(o.type) {
  o.ptr = nullptr;
}

// declare move assignment
Identity &Identity::operator=(Identity &&o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  // now release our inner identity.
  if (ptr != nullptr) identity_destroy(ptr, type);

  // now takes ownership of pointer
  ptr = o.ptr;
  type = o.type;

  // ensure o.ptr is null
  o.ptr = nullptr;

  return *this;
}

Identity::Identity(void *ptr, IdentityType type) : ptr(ptr), type(type) {}

Identity::Identity() {
  ptr = identity_anonymous();
  type = IdentityType::Anonym;
}

std::variant<Identity, std::string> Identity::BasicFromPem(
    const std::string &pemData) {
  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = error_callback;

  void *ptr = identity_basic_from_pem(pemData.data(), &ret);

  if (ptr == nullptr) {
    std::variant<Identity, std::string> error(data);
    return error;
  }

  std::variant<Identity, std::string> ok(
      std::move(Identity(ptr, IdentityType::Basic)));

  return ok;
}

std::variant<Identity, std::string> Identity::Secp256k1FromPem(
    const std::string &pemData) {
  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = error_callback;

  void *ptr = identity_secp256k1_from_pem(pemData.data(), &ret);

  if (ptr == nullptr) {
    std::variant<Identity, std::string> error(data);
    return error;
  }

  std::variant<Identity, std::string> ok(
      std::move(Identity(ptr, IdentityType::Secp256k1)));
  return ok;
}

std::variant<Identity, std::string> Identity::BasicFromKeyPair(
    const std::vector<uint8_t> &publicKey,
    const std::vector<uint8_t> &privateKeySeed) {
  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = error_callback;

  void *ptr = identity_basic_from_key_pair(publicKey.data(),
                                           privateKeySeed.data(), &ret);

  if (ptr == nullptr) {
    std::variant<Identity, std::string> error(data);
    return error;
  }

  std::variant<Identity, std::string> ok(
      std::move(Identity(ptr, IdentityType::Basic)));

  return ok;
}

Identity Identity::Secp256k1FromPrivateKey(
    const std::vector<char> &privateKey) {
  void *ptr =
      identity_secp256k1_from_private_key(privateKey.data(), privateKey.size());

  return Identity(ptr, IdentityType::Basic);
}

std::variant<zondax::Principal, std::string> Identity::Sender() {
  if (ptr == nullptr) {
    std::variant<Principal, std::string> error{
        std::in_place_type<std::string>, "Identity instance uninitialized"};
    return error;
  }

  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = error_callback;

  CPrincipal *p = identity_sender(ptr, type, &ret);

  if (p == nullptr) {
    std::variant<zondax::Principal, std::string> error(data);
    return error;
  }

  std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
  ptr = nullptr;

  auto principal = zondax::Principal(outBytes);

  std::variant<zondax::Principal, std::string> ok(std::move(principal));

  return ok;
}

std::variant<IdentitySign, std::string> Identity::Sign(
    const std::vector<uint8_t> &bytes) {
  if (ptr == nullptr) {
    std::variant<IdentitySign, std::string> error{
        std::in_place_type<std::string>, "Identity instance uninitialized"};
    return error;
  }
  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void *)&data;
  ret.call = error_callback;

  CIdentitySign *p = identity_sign(bytes.data(), bytes.size(), ptr, type, &ret);

  if (p == nullptr) {
    std::variant<IdentitySign, std::string> error(data);
    return error;
  }

  IdentitySign result;

  std::vector<unsigned char> pk(
      cidentitysign_pubkey(p),
      cidentitysign_pubkey(p) + cidentitysign_pubkey_len(p));
  std::vector<unsigned char> sig(
      cidentitysign_sig(p), cidentitysign_sig(p) + cidentitysign_sig_len(p));

  result.pubkey = pk;
  result.signature = sig;
  cidentitysign_destroy(p);

  std::variant<IdentitySign, std::string> ok(std::move(result));

  return result;
}

void *Identity::getPtr() const { return ptr; }

IdentityType Identity::getType() const { return type; }

Identity::~Identity() {
  if (ptr != nullptr) identity_destroy(ptr, type);
}
}  // namespace zondax
