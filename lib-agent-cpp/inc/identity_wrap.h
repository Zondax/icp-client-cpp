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
#ifndef IDENTITY_WRAP_H
#define IDENTITY_WRAP_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include "principal_wrap.h"

extern "C" {
#include "bindings.h"
}

namespace zondax::identity {
class Identity {

private:
    void* ptr;
    IdentityType type;

public:
    Identity(void* ptr, IdentityType type);
    ~Identity();

    static Identity Anonymous();
    static std::optional<Identity> BasicFromPem(const std::string& pemData, RetPtr_u8 error);
    static std::optional<Identity> BasicFromKeyPair(const std::vector<uint8_t>& publicKey,
                                    const std::vector<uint8_t>& privateKeySeed,
                                    RetPtr_u8 error);
    static std::optional<Identity> Secp256k1FromPem(const std::string& pemData, RetPtr_u8 error);
    static Identity Secp256k1FromPrivateKey(const std::vector<char>& privateKey);

    std::optional<zondax::principal::Principal> Sender(RetPtr_u8 error) const;

    void* getPtr() const;
    IdentityType getType() const;
};

}
#endif  // IDENTITY_WRAP_H
