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
#include "agent.h"

extern "C" {
#include "helper_c.h"
}
#include "helper.h"

using namespace zondax::agent;
using namespace zondax::principal;
using namespace zondax::identity;
using namespace zondax::idl_args;
using namespace zondax::idl_value;

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
    // Canister info from hello world deploy example
    std::string id_text = "rdmx6-jaaaa-aaaaa-aaadq-cai";
    std::string did_file = "../examples/icp-app/rdmx6-jaaaa-aaaaa-aaadq-cai.did";
    std::string url = "https://ic0.app";

    // Get did file content
    long file_size = did_file_size(did_file);
    std::vector<char> buffer(file_size + 1);
    int result = did_file_content(did_file, file_size, buffer.data());

    //Get principal form text
    std::optional<Principal> principal = Principal::FromText(id_text, error_cb_cpp);
    
    //Construct anonymoous id
    Identity anonymousIdentity;
    
    //Create agent with agent constructor
    Agent agent(url, anonymousIdentity, principal.value(), buffer, error_cb_cpp);

    // Create an IdlValue object with the uint64_t value (nat64 in candid)
    uint64_t nat64 = 1974211;
    IdlValue elem(nat64);

    // Create a vector of IdlValue pointers
    std::vector<zondax::idl_value::IdlValue*> values;
    values.push_back(&elem);

    // Create an IdlArgs object using the vector of IdlValue pointers
    IdlArgs args(values);

    //Make Query call to canister
    IdlArgs out = agent.Query("lookup", args, error_cb_cpp);

    //Get text representation and print
    std::string out_text = out.getText();
    std::cout << out_text << std::endl;

    return 0;
}
