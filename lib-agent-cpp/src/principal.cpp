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

#include "principal.h"

namespace zondax::principal {

void error_callback(const unsigned char *data, int len, void *user_data) {
    std::string error_msg((const char *)data, len);
    *(std::string *)user_data = error_msg;
}

// declare move constructor
Principal::Principal(Principal &&o) noexcept : bytes(std::move(o.bytes)) {
    cPrincipal = o.cPrincipal;
} 

// declare move assignment
Principal& Principal::operator=(Principal &&o) noexcept {
    // check they are not the same object
    if (&o == this)
        return *this;

    // now release our inner identity.
    if (cPrincipal != nullptr)
        principal_destroy(cPrincipal);

    // now takes ownership of the values
    cPrincipal = o.cPrincipal;
    bytes = std::move(o.bytes);

    // set other to null
    o.cPrincipal = nullptr;

    return *this;
}

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

Principal::Principal(bool anonym) {
    cPrincipal = anonym ? principal_anonymous() : principal_management_canister(); 

    std::vector<unsigned char> outBytes(cPrincipal->ptr, cPrincipal->ptr + cPrincipal->len);
    bytes =  outBytes;
}

Principal Principal::SelfAuthenticating(const std::vector<uint8_t> &public_key) {
    CPrincipal* p = principal_self_authenticating(public_key.data(), public_key.size());

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    principal_destroy(p);
    return Principal(outBytes);
}

/**
 * @brief 
 * 
 * @param bytes 
 * @param errorCallback 
 * @return std::optional<Principal> 
 */
std::variant<Principal, std::string> Principal::TryFromSlice(const std::vector<uint8_t> &bytes) {

    std::string data;

    RetError ret;
    ret.user_data = (void *)&data;
    ret.call = error_callback;

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_try_from_slice(bytes.data(), bytes.size(), &ret);

    if (p == nullptr) {
        // TODO: Why a call to destroy over a null pointer?
        // principal_destroy(p);
        std::variant<Principal, std::string> error(data);
        return error;
    }

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    auto cpp_principal = Principal(outBytes);

    std::variant<Principal, std::string> ok(std::move(cpp_principal));

    return ok;
}

/**
 * @brief 
 * 
 * @param text 
 * @param errorCallback 
 * @return std::optional<Principal> 
 */
std::variant<Principal, std::string> Principal::FromText(const std::string& text) {

    std::string data;

    RetError ret;
    ret.user_data = (void *)&data;
    ret.call = error_callback;

    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_from_text(text.c_str(), &ret);

    if (p == nullptr) {
        // TODO: Why calling destructor over a null ptr?
        // principal_destroy(p);
        std::variant<Principal, std::string> error(data);
        return error;
    }

    std::vector<unsigned char> outBytes(p->ptr, p->ptr + p->len);
    auto cpp_principal = Principal(outBytes);

    std::variant<Principal, std::string> ok(std::move(cpp_principal));

    return ok;

}

/**
 * @brief 
 * 
 * @param data 
 * @param errorCallback 
 * @return std::string 
 */
std::string Principal::ToText(const std::vector<unsigned char>& data) {
    // get error from error_ret here and send it on string of result
    CPrincipal *p = principal_to_text(data.data(), data.size(), nullptr);

    if (p == nullptr) {
        return "";
    }

    std::string result(reinterpret_cast<const char*>(p->ptr), p->len);

    principal_destroy(p);

    return result;
}

std::vector<unsigned char> Principal::getBytes() const {
    return bytes;
}

Principal::~Principal() {
    if (cPrincipal != nullptr) {
        principal_destroy(cPrincipal);
    }
}

}
