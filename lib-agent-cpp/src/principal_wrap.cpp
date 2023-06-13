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
#include <optional>
#include <cstdlib>
#include "principal_wrap.h"

// Define the Principal constructor
Principal::Principal(const std::vector<unsigned char>& data) : bytes(data) {}

std::vector<unsigned char> Principal::getBytes() const {
    return bytes;
}

// Define the Anonymous static method
Principal Principal::Anonymous() {
    CPrincipal* p = principal_anonymous();

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

Principal Principal::ManagementCanister() {
    CPrincipal* p = principal_management_canister();

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

Principal Principal::SelfAuthenticating(const uint8_t* public_key, int public_key_len) {
    CPrincipal* p = principal_self_authenticating(public_key, public_key_len);

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

Principal Principal::FromSlice(const uint8_t* bytes, int bytes_len) {
    CPrincipal* p = principal_from_slice(bytes, bytes_len);

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

// Trying to use the variant to get the principal an error all in result
// can we have the error_ret procedure here ?
std::variant<Principal, std::string> Principal::TryFromSlice(const uint8_t* bytes, int bytes_len) {
    std::string errorMessage;
    
    // Tried this nut no success I get an error
    /*
        // Create a closure to capture the error message
        auto errorCallback = [&](const uint8_t* data, int len) {
            errorMessage = std::string(reinterpret_cast<const char*>(data), len);
        };

        // Cast the errorCallback to the RetPtr_u8 type
        RetPtr_u8 errorFunc = reinterpret_cast<RetPtr_u8>(errorCallback);
    */

    // Define a separate function with a matching signature
    void errorCallback(const uint8_t* data, int len) {
        errorMessage = std::string(reinterpret_cast<const char*>(data), len);
    }

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_try_from_slice(bytes, bytes_len, errorCallback);

    if (p->ptr == nullptr) {
        return errorMessage;
    }

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

// Is optional an option can we wrap the error on this type to return the error
std::optional<Principal> FromText(const char* text){}
std::optional<Principal> ToText(const uint8_t* bytes, int bytes_len){}

// Will this work ? I tested with calling principal_destroy but is showing double freed memory
Principal::~Principal() {
    if (bytes.empty())
        return;

    //principal_destroy(reinterpret_cast<CPrincipal*>(&bytes[0]));
}
