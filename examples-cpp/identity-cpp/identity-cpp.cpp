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
#include <iostream>
#include "identity.h"

extern "C" {
#include "helper_c.h"
}

using namespace zondax::identity;
using namespace zondax::principal;

Error error_cpp;

void error_cb_cpp(const uint8_t* p, int len) {
    if (error_cpp.ptr != nullptr) {
         free((void*)error_cpp.ptr);
    }
    error_cpp.ptr = static_cast<const uint8_t*>(malloc(len));
    error_cpp.len = len;
    memcpy((void*)error_cpp.ptr, p, len);
}
std::string basic_expected = "emrl6-qe3wz-fh5ib-sx2r4-fbx46-6g4ql-5ro3g-zhbtm-nxdrq-q2oqo-jqe";
std::string BasicIdentityFile =
    "-----BEGIN PRIVATE KEY-----\n"
    "MFMCAQEwBQYDK2VwBCIEIL9r4XBKsg4pquYBHY6rgfzuBsvCy89tgqDfDpofXRBP\n"
    "oSMDIQBCkE1NL4X43clXS1LFauiceiiKW9NhjVTEpU6LpH9Qcw==\n"
    "-----END PRIVATE KEY-----";

std::vector<uint8_t> PubKeyExpected = {
    0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00, 0x42, 0x90,
    0x4d, 0x4d, 0x2f, 0x85, 0xf8, 0xdd, 0xc9, 0x57, 0x4b, 0x52, 0xc5, 0x6a, 0xe8, 0x9c,
    0x7a, 0x28, 0x8a, 0x5b, 0xd3, 0x61, 0x8d, 0x54, 0xc4, 0xa5, 0x4e, 0x8b, 0xa4, 0x7f,
    0x50, 0x73
};

std::vector<uint8_t> SignatureExpected = {
    0x6d, 0x7a, 0x2f, 0x85, 0xeb, 0x6c, 0xc2, 0x18, 0x80, 0xc8, 0x3d, 0x9b, 0xb1, 0x70,
    0xe2, 0x4b, 0xf5, 0xd8, 0x9a, 0xa9, 0x96, 0x92, 0xb6, 0x89, 0xac, 0x9d, 0xe9, 0x5c,
    0x1e, 0x3e, 0x50, 0xdc, 0x98, 0x12, 0x2f, 0x94, 0x11, 0x2f, 0x6c, 0xc6, 0x6a, 0x0b,
    0xbf, 0xc0, 0x56, 0x5b, 0xdb, 0x87, 0xa9, 0xe2, 0x2c, 0x8e, 0x56, 0x94, 0x56, 0x12,
    0xde, 0xbf, 0x22, 0x4a, 0x3f, 0xdb, 0xf1, 0x03
};

int main() {
    // Creating an anonymous identity
    Identity anonymousIdentity;

    // Getting the sender principal
    std::optional<Principal> senderPrincipal = anonymousIdentity.Sender(error_cb_cpp);
    if(senderPrincipal.has_value()) {
        std::vector<unsigned char> bytes = senderPrincipal.value().getBytes();
        if (bytes.size() == 1 && bytes[0] == 4 && anonymousIdentity.getType() == Anonym) {
            std::cout << "Test 1: Valid Anonym identity" << std::endl;
        } else {
            std::cout << "Test 1: Invalid Anonym identity" << std::endl;
        }
    } else {
        std::cout << "Test 1: "<< error_cpp.ptr << std::endl;
    }

    // Creating an Basic Identity
    std::optional<Identity> id = Identity::BasicFromPem(BasicIdentityFile, error_cb_cpp);

    // Getting the sender principal
    std::optional<Principal> idPrincipal = id.value().Sender(error_cb_cpp);
    std::string str = Principal::ToText(idPrincipal.value().getBytes(), error_cb_cpp);

    if(idPrincipal.has_value()) {
        if(str == basic_expected) {
            std::cout << "Test 2: Valid Basic from file identity" << std::endl;
        } else {
            std::cout << "Test 2: Invalid Basic from file identity" << std::endl;
        }
    } else {
        std::cout << "Test 2: "<< error_cpp.ptr << std::endl;
    }

    // Creating an Basic Identity
    std::optional<Identity> basic = Identity::BasicFromPem(BasicIdentityFile, error_cb_cpp);
    std::optional<IdentitySign> result = basic.value().Sign({}, error_cb_cpp);

    if(result.has_value()) {
        if(result.value().pubkey == PubKeyExpected && result.value().signature == SignatureExpected) {
            std::cout << "Test 3: Valid Sign Basic" << std::endl;
        } else {
            std::cout << "Test 3: Invalid Sign Basic" << std::endl;
        }
    } else {
        std::cout << "Test 3: "<< error_cpp.ptr << std::endl;
    }


}
