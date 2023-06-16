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
#include "identity_wrap.h"

extern "C" {
#include "helper.h"
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

int main() {
    // Creating an anonymous identity
    Identity anonymousIdentity = Identity::Anonymous();

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

}
