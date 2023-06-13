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
#ifndef PRINCIPAL_WRAP_H
#define PRINCIPAL_WRAP_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>

extern "C" {
#include "bindings.h"
}

class Principal {
private:
    std::vector<unsigned char> bytes;

public:
    Principal(const std::vector<unsigned char>& data);

    std::vector<unsigned char> getBytes() const;

    static Principal ManagementCanister();
    static Principal SelfAuthenticating(const uint8_t* public_key, int public_key_len);
    static Principal Anonymous();
    static Principal FromSlice(const uint8_t* bytes, int bytes_len);
    static std::variant<Principal, std::string> TryFromSlice(const uint8_t* bytes, int bytes_len);
    static std::optional<Principal> FromText(const char* text);
    static std::optional<Principal> ToText(const uint8_t* bytes, int bytes_len);
    ~Principal();
};

#endif  // PRINCIPAL_WRAP_H
