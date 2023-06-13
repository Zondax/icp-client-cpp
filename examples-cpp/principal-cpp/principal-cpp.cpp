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
#include "principal_wrap.h"

extern "C" {
#include "helper.h"
}

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

    // Get Principal Management Canister
    Principal p = Principal::ManagementCanister();

    std::vector<unsigned char> bytes = p.getBytes();
    if (bytes.size() == 0 ) {
        std::cout << "Test 1: Valid Principal Management" << std::endl;
    } else {
        std::cout << "Test 1: Invalid Principal Management" << std::endl;
    }


    // Get Anonymous Principal
    p = Principal::Anonymous();

    bytes = p.getBytes();
    if (bytes.size() == 1 && bytes[0] == 4) {
        std::cout << "Test 2: Valid Principal Anonymous" << std::endl;
    } else {
        std::cout << "Test 2: Invalid Principal Anonymous" << std::endl;
    }

    // Get SelfAuthenticating Principal
    unsigned char publicKey[] =
    {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00,
    };

    int publicKey_len = sizeof(publicKey) / sizeof(publicKey[0]);

    unsigned char bytesExpected[] =
    {
        0x2f, 0x8e, 0x47, 0x38, 0xf9, 0xd7, 0x68, 0x16, 0x82, 0x99, 0x85, 0x41, 0x52, 0x67,
        0x86, 0x38, 0x07, 0xd3, 0x7d, 0x20, 0x6a, 0xd9, 0x0f, 0xea, 0x72, 0xbf, 0x9d, 0xcf,
        0x02,
    };

    int bytesExpected_len = sizeof(bytesExpected) / sizeof(bytesExpected[0]);

    p = Principal::SelfAuthenticating(publicKey, publicKey_len);

    bytes = p.getBytes();
    if (bytes.size() == bytesExpected_len &&
        std::equal(bytes.begin(), bytes.end(), std::begin(bytesExpected))) {
        std::cout << "Test 3: Valid Principal Self Authenticating" << std::endl;
    } else {
        std::cout << "Test 3: InValid Principal Self Authenticating" << std::endl;
    }


    // Get Principal from slice of bytes
    unsigned char slice[] = { 0x22 };
    p = Principal::FromSlice(slice, 1);

    bytes = p.getBytes();
    if (bytes.size() == 1 && bytes[0] == 0x22) {
        printf("Test 4.0: Valid Principal from bytes.\n");
    } else {
        printf("Test 4.0: Error.\n");
    }

    // IF we send a byte array bigger than 29 an error is expected
    unsigned char slice_long[] =
    {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        //0x33, 0x22, 0x11, 0x00,
    };

    std::variant<Principal, std::string> result = Principal::TryFromSlice(slice_long, 32, error_cb_cpp);
    
    std::string error = std::get<std::string>(result);
    std::cout << "Error: " << error << std::endl;
    return 0;
}
