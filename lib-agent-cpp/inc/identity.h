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
#ifndef IDENTITY_H
#define IDENTITY_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include <variant>

#include "principal.h"

extern "C" {
#include "zondax_ic.h"
}
struct IdentitySign {
    std::vector<uint8_t> pubkey;
    std::vector<uint8_t> signature;
};

namespace zondax::identity {
class Identity {

private:
    void* ptr;
    IdentityType type;
    explicit Identity(void* ptr, IdentityType type);

public:
    ~Identity();
    Identity();

    // Disable copies, just move semantics
    Identity(const Identity &args) = delete;
    void operator=(const Identity&) = delete;

    // declare move constructor
    Identity(Identity &&o) noexcept;
    // declare move assignment
    Identity& operator=(Identity &&o) noexcept;

    static Identity Anonymous();
    static std::variant<Identity, std::string> BasicFromPem(const std::string& pemData);
    static std::variant<Identity, std::string> BasicFromKeyPair(const std::vector<uint8_t>& publicKey,
                                    const std::vector<uint8_t>& privateKeySeed);
    static std::variant<Identity, std::string> Secp256k1FromPem(const std::string& pemData);
    static Identity Secp256k1FromPrivateKey(const std::vector<char>& privateKey);
    std::variant<zondax::principal::Principal, std::string> Sender();
    std::variant<IdentitySign, std::string> Sign(const std::vector<uint8_t>& bytes);

    void* getPtr() const;
    IdentityType getType() const;
};

}
#endif  // IDENTITY_H
