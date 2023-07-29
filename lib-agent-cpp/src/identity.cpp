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

#include "doctest.h"

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

// ****************************** Tests
using namespace zondax;

TEST_CASE("Identity_from_pem") {
  std::string BasicIdentityFile =
      "-----BEGIN PRIVATE KEY-----\n"
      "MFMCAQEwBQYDK2VwBCIEIL9r4XBKsg4pquYBHY6rgfzuBsvCy89tgqDfDpofXRBP\n"
      "oSMDIQBCkE1NL4X43clXS1LFauiceiiKW9NhjVTEpU6LpH9Qcw==\n"
      "-----END PRIVATE KEY-----";

  std::vector<uint8_t> PubKeyExpected = {
      0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21,
      0x00, 0x42, 0x90, 0x4d, 0x4d, 0x2f, 0x85, 0xf8, 0xdd, 0xc9, 0x57,
      0x4b, 0x52, 0xc5, 0x6a, 0xe8, 0x9c, 0x7a, 0x28, 0x8a, 0x5b, 0xd3,
      0x61, 0x8d, 0x54, 0xc4, 0xa5, 0x4e, 0x8b, 0xa4, 0x7f, 0x50, 0x73};

  std::vector<uint8_t> SignatureExpected = {
      0x6d, 0x7a, 0x2f, 0x85, 0xeb, 0x6c, 0xc2, 0x18, 0x80, 0xc8, 0x3d,
      0x9b, 0xb1, 0x70, 0xe2, 0x4b, 0xf5, 0xd8, 0x9a, 0xa9, 0x96, 0x92,
      0xb6, 0x89, 0xac, 0x9d, 0xe9, 0x5c, 0x1e, 0x3e, 0x50, 0xdc, 0x98,
      0x12, 0x2f, 0x94, 0x11, 0x2f, 0x6c, 0xc6, 0x6a, 0x0b, 0xbf, 0xc0,
      0x56, 0x5b, 0xdb, 0x87, 0xa9, 0xe2, 0x2c, 0x8e, 0x56, 0x94, 0x56,
      0x12, 0xde, 0xbf, 0x22, 0x4a, 0x3f, 0xdb, 0xf1, 0x03};

  // Creating an Basic Identity
  auto basic = Identity::BasicFromPem(BasicIdentityFile);

  REQUIRE(std::holds_alternative<Identity>(basic));

  SUBCASE("SignIdentity") {
    // identitySign
    auto result = std::get<Identity>(basic).Sign({});
    REQUIRE(std::holds_alternative<IdentitySign>(result));
    auto value = std::get<IdentitySign>(result);

    REQUIRE(std::equal(value.pubkey.begin(), value.pubkey.end(),
                       std::begin(PubKeyExpected)));
    REQUIRE(std::equal(value.signature.begin(), value.signature.end(),
                       std::begin(SignatureExpected)));
  }
}

TEST_CASE("Anonymous Identity") {
  // Creating an anonymous identity
  Identity anonymousIdentity;

  // Getting the sender principal
  auto senderPrincipal = anonymousIdentity.Sender();
  REQUIRE(std::holds_alternative<zondax::Principal>(senderPrincipal));

  auto bytes = std::get<Principal>(senderPrincipal).getBytes();

  REQUIRE(bytes.size() == 1);
  // REQUIRE(bytes[0] == 4);
  REQUIRE(bytes[0] == 4);
  REQUIRE(anonymousIdentity.getType() == Anonym);
}
