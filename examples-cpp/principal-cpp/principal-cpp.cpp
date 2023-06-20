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
#include "principal.h"

extern "C" {
#include "helper_c.h"
}

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

int main() {
    
    // Get Management Principal
    Principal management(false);

    std::vector<unsigned char> bytes = management.getBytes();
    if (bytes.size() == 0 ) {
        std::cout << "Test 1: Valid Principal Management" << std::endl;
    } else {
        std::cout << "Test 1: Invalid Principal Management" << std::endl;
    }

    // Get Anonymous Principal
    Principal anonym;

    bytes = anonym.getBytes();
    if (bytes.size() == 1 && bytes[0] == 4) {
        std::cout << "Test 2: Valid Principal Anonymous" << std::endl;
    } else {
        std::cout << "Test 2: Invalid Principal Anonymous" << std::endl;
    }

    // Get SelfAuthenticating Principal
    std::vector<uint8_t> publicKey=
    {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00,
    };

    std::vector<uint8_t> bytesExpected=
    {
        0x2f, 0x8e, 0x47, 0x38, 0xf9, 0xd7, 0x68, 0x16, 0x82, 0x99, 0x85, 0x41, 0x52, 0x67,
        0x86, 0x38, 0x07, 0xd3, 0x7d, 0x20, 0x6a, 0xd9, 0x0f, 0xea, 0x72, 0xbf, 0x9d, 0xcf,
        0x02,
    };

    Principal p = Principal::SelfAuthenticating(publicKey);

    bytes = p.getBytes();
    if (bytes.size() == bytesExpected.size() &&
        std::equal(bytes.begin(), bytes.end(), std::begin(bytesExpected))) {
        std::cout << "Test 3: Valid Principal Self Authenticating" << std::endl;
    } else {
        std::cout << "Test 3: Invalid Principal Self Authenticating" << std::endl;
    }


    // Get Principal from slice of bytes
    std::vector<uint8_t> slice = {0x1};
    Principal principal(slice);

    bytes = principal.getBytes();
    if (bytes.size() == 1 && static_cast<int>(bytes[0])==0x1 ) {
        std::cout << "Test 4.0: Valid Principal from bytes" << std::endl;
    } else {
        std::cout << "Test 4.0: Invalid Principal from bytes" << std::endl;
    }

    // IF we send a byte array bigger than 29 an error is expected
    std::vector<uint8_t> slice_long = {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00,
    };

   std::optional<Principal> result = Principal::TryFromSlice(slice_long, error_cb_cpp);

    if(result.has_value()) {
        bytes = result.value().getBytes();
        if (bytes.size() == slice_long.size() &&
            std::equal(bytes.begin(), bytes.end(), std::begin(slice_long))) {
            std::cout << "Test 4.1: Valid Principal from slice" << std::endl;
        } else {
            std::cout << "Test 4.1: Invalid Principal from slice" << std::endl;
        }
    } else {
        std::cout << "Test 4.1: "<< error_cpp.ptr << std::endl;
    }

    // Get principal from Text , testing with anonymous principal
    std::string text = "2vxsx-fae";
    
    std::optional<Principal> result2 = Principal::FromText(text, error_cb_cpp);
    
    if(result2.has_value()) {
        bytes = result2.value().getBytes();
        if (bytes.size() == 1 && bytes[0] == 4) {
            std::cout << "Test 5: Valid Principal from text" << std::endl;
        } else {
            std::cout << "Test 5: Invalid Principal from text" << std::endl;
        }
    } else {
        std::cout << "Test 5: "<< error_cpp.ptr << std::endl;
    }

    // Get Text from principal, testing with anonymous principal
    std::vector<unsigned char> p_slice= { 4 };

    std::string str = Principal::ToText(p_slice, error_cb_cpp);
    
    if (str == text) {
        std::cout << "Test 6: Valid Text from Principal"<< std::endl;
    } else {
        std::cout << "Test 6: Invalid Text from Principal"<< std::endl;
    }

    return 0;
}
