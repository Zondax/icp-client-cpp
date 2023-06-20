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
#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>

extern "C" {
#include "zondax_ic.h"
}

namespace zondax::principal {
class Principal {
private:
    std::vector<unsigned char> bytes;
    CPrincipal* cPrincipal;

public:
    explicit Principal(bool anonym = true);
    explicit Principal(const std::vector<uint8_t> &bytes);

    static Principal SelfAuthenticating(const std::vector<uint8_t> &public_key);
    static std::optional<Principal> TryFromSlice(const std::vector<uint8_t> &bytes, RetPtr_u8 errorCallback);
    static std::optional<Principal> FromText(const std::string& text,  RetPtr_u8 errorCallback);
    static std::string ToText(const std::vector<uint8_t> &bytes,  RetPtr_u8 errorCallback);

    std::vector<unsigned char> getBytes() const;
    ~Principal();
};

}
#endif  // PRINCIPAL_H
