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

#include "identity_wrap.h"
namespace zondax::identity {

    Identity::Identity(void* ptr, IdentityType type) : ptr(ptr), type(type) {}

    Identity Identity::Anonymous() {
        void* ptr = identity_anonymous();
        return Identity(ptr, IdentityType::Anonym);
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

    std::optional<zondax::principal::Principal> Identity::Sender(RetPtr_u8 error) const{
        CPrincipal *p = identity_sender(ptr, type, error);

        if (p == nullptr) {
            return std::nullopt;
        }

        std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
        return zondax::principal::Principal(outBytes);
    }


    // CidenitySign is a struture in rust for we defined functions to get the two arrays inside
    // should we create a struture and use those function inside to populate the struture ?
    // struct CIdentitySign *identity_sign(const uint8_t *bytes, int bytes_len,
    //                                void *id_ptr,
    //                                enum IdentityType idType,
    //                                RetPtr_u8 error_ret);


    void* Identity::getPtr() const {
        return ptr;
    }

    IdentityType Identity::getType() const {
        return type;
    }

    Identity::~Identity() {}
}

