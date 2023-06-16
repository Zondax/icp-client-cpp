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

namespace zondax::principal {

/**
 * @brief Constructs a Principal object from a vector of bytes.
 * @param data The vector of bytes to initialize the Principal.
 *
 * This constructor initializes a Principal object with the provided vector of bytes.
 * The bytes parameter contains the data used to construct the Principal.
 */
Principal::Principal(const std::vector<unsigned char>& data) : bytes(data) {
    cPrincipal = principal_from_slice(data.data(), data.size());

    std::vector<unsigned char> outBytes(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);
    bytes = outBytes;
}

/**
 * @brief Constructor for creating an anonymous or management Principal.
 * 
 * @param anonym if set to true this constructor creates an anonymous Principal object.
 *               if set to false this constructor creates an management Principal object.
 */
Principal::Principal(bool anonym) {
    cPrincipal = anonym ? principal_anonymous() : principal_management_canister(); 

    std::vector<unsigned char> outBytes(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);
    bytes =  outBytes;
}

/**
 * @brief Constructor for creating an Self Authenticating Principal.
 * 
 * @param public_key 
 * @return Principal 
 */
Principal Principal::SelfAuthenticating(const std::vector<uint8_t> &public_key) {
    CPrincipal* p = principal_self_authenticating(public_key.data(), public_key.size());

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

/**
 * @brief 
 * 
 * @param bytes 
 * @param errorCallback 
 * @return std::optional<Principal> 
 */
std::optional<Principal> Principal::TryFromSlice(const std::vector<uint8_t> &bytes, RetPtr_u8 errorCallback) {

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_try_from_slice(bytes.data(), bytes.size(), errorCallback);

    if (p == nullptr) {
        return std::nullopt;
    }

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

/**
 * @brief 
 * 
 * @param text 
 * @param errorCallback 
 * @return std::optional<Principal> 
 */
std::optional<Principal> Principal::FromText(const std::string& text,  RetPtr_u8 errorCallback) {

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_from_text(text.data(), errorCallback);

    if (p == nullptr) {
        return std::nullopt;
    }

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    return Principal(outBytes);
}

/**
 * @brief 
 * 
 * @param data 
 * @param errorCallback 
 * @return std::string 
 */
std::string Principal::ToText(const std::vector<unsigned char>& data,  RetPtr_u8 errorCallback) {

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_to_text(data.data(), data.size(), errorCallback);

    if (p == nullptr) {
        return "";
    }

    std::string result(reinterpret_cast<const char*>(p->ptr), p->len);
    return result;
}

/**
 * @brief Get the bytes of the Principal.
 * @return The vector of bytes representing the Principal.
 *
 * This method returns the bytes of the Principal as a vector of unsigned char.
 * The bytes represent the underlying data of the Principal.
 */
std::vector<unsigned char> Principal::getBytes() const {
    return bytes;
}

Principal::~Principal() {
    if (cPrincipal != nullptr) {
        principal_destroy(cPrincipal);
        cPrincipal = nullptr;  // Reset the member variable after destruction
    }
}
}
