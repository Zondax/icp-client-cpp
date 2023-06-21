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

namespace zondax::identity {

    Identity::Identity(void* ptr, IdentityType type) : ptr(ptr), type(type) {}

    Identity::Identity() {
        ptr = identity_anonymous();
        type = IdentityType::Anonym;
    }

    std::optional<Identity> Identity::BasicFromPem(const std::string& pemData, RetPtr_u8 error) {
        void* ptr = identity_basic_from_pem(pemData.data(), error);
        
        if (ptr == nullptr) {
            return std::nullopt;
        }

        return Identity(ptr, IdentityType::Basic);
    }

    std::optional<Identity> Identity::Secp256k1FromPem(const std::string& pemData, RetPtr_u8 error) {
        void* ptr = identity_secp256k1_from_pem(pemData.data(), error);

        if (ptr == nullptr) {
            return std::nullopt;
        }

        return Identity(ptr, IdentityType::Secp256k1);
    }

    std::optional<Identity> Identity::BasicFromKeyPair(const std::vector<uint8_t>& publicKey,
                                     const std::vector<uint8_t>& privateKeySeed,
                                     RetPtr_u8 error) {
        void *ptr = identity_basic_from_key_pair(publicKey.data(), privateKeySeed.data(), error);
        
        if (ptr == nullptr) {
            return std::nullopt;
        }

        return Identity(ptr, IdentityType::Basic);
    }

    Identity Identity::Secp256k1FromPrivateKey(const std::vector<char>& privateKey) {
        void *ptr = identity_secp256k1_from_private_key(privateKey.data(), privateKey.size());
        
        return Identity(ptr, IdentityType::Basic);
    }

    std::optional<zondax::principal::Principal> Identity::Sender(RetPtr_u8 error) {
        CPrincipal *p = identity_sender(ptr, type, error);

        if (p == nullptr) {
            return std::nullopt;
        }

        std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
        ptr = nullptr;
        
        return zondax::principal::Principal(outBytes);
    }

    std::optional<IdentitySign> Identity::Sign(const std::vector<uint8_t>& bytes, RetPtr_u8 error) {
        CIdentitySign* p = identity_sign(bytes.data(), bytes.size(), ptr, type, error);

        if (p == nullptr) {
            return std::nullopt;
        }

        IdentitySign result;

        std::vector<unsigned char> pk(cidentitysign_pubkey(p), cidentitysign_pubkey(p) + cidentitysign_pubkey_len(p));
        std::vector<unsigned char> sig(cidentitysign_sig(p), cidentitysign_sig(p) + cidentitysign_sig_len(p));

        result.pubkey = pk;
        result.signature = sig;
        cidentitysign_destroy(p);

        return result;
    }

    void* Identity::getPtr() const {
        return ptr;
    }

    IdentityType Identity::getType() const {
        return type;
    }

    Identity::~Identity() {}
}

