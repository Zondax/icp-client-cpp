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

#include <iostream>

extern "C" {
#include "helper_c.h"
}

using namespace zondax;

Error error_cpp;

// TODO: Add example to verify callback mechanism works fine!!!!

std::string basic_expected =
    "emrl6-qe3wz-fh5ib-sx2r4-fbx46-6g4ql-5ro3g-zhbtm-nxdrq-q2oqo-jqe";

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

int main() {
  // Creating an anonymous identity
  Identity anonymousIdentity;

  // Getting the sender principal
  auto senderPrincipal = anonymousIdentity.Sender();

  if (std::holds_alternative<Principal>(senderPrincipal)) {
    std::vector<unsigned char> bytes =
        std::get<Principal>(senderPrincipal).getBytes();

    if (bytes.size() == 1 && bytes[0] == 4 &&
        anonymousIdentity.getType() == Anonym) {
      std::cout << "Test 1: Valid Anonym identity" << std::endl;
    } else {
      std::cout << "Test 1: Invalid Anonym identity" << std::endl;
    }

  } else {
    std::cout << "Test 1: " << std::get<std::string>(senderPrincipal)
              << std::endl;
  }

  // Creating an Basic Identity
  auto id = Identity::BasicFromPem(BasicIdentityFile);

  if (!std::holds_alternative<Identity>(id)) {
    std::cout << std::get<std::string>(id) << std::endl;
    return -1;
  }

  // Getting the sender principal
  auto idPrincipal = std::get<Identity>(id).Sender();

  if (std::holds_alternative<std::string>(idPrincipal)) {
    std::cout << "Error:  " << std::get<std::string>(idPrincipal) << std::endl;
    return -1;
  }

  std::string str =
      Principal::ToText(std::get<Principal>(idPrincipal).getBytes());

  // Creating an Basic Identity
  auto basic = Identity::BasicFromPem(BasicIdentityFile);

  if (std::holds_alternative<std::string>(basic)) {
    std::cout << "Error:  " << std::get<std::string>(basic) << std::endl;
    return -1;
  }
  // identitySign
  auto result = std::get<Identity>(basic).Sign({});

  if (std::holds_alternative<std::string>(result)) {
    std::cout << "Error:  " << std::get<std::string>(result) << std::endl;
    return -1;
  }

  auto value = std::get<IdentitySign>(result);
  if (value.pubkey == PubKeyExpected && value.signature == SignatureExpected) {
    std::cout << "Test 3: Valid Sign Basic" << std::endl;
  } else {
    std::cout << "Test 3: Invalid Sign Basic" << std::endl;
  }

  return 0;
}
