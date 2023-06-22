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
#include <variant>
#include "agent.h"
#include "idl_value.h"

extern "C" {
#include "helper_c.h"
}
#include "helper.h"

using namespace zondax::agent;
using namespace zondax::principal;
using namespace zondax::identity;
using namespace zondax::idl_args;
using namespace zondax::idl_value;

int main() {
    // Canister info from hello world deploy example
    std::string id_text = "rdmx6-jaaaa-aaaaa-aaadq-cai";
    std::string did_file = "../examples/ic_c/rdmx6-jaaaa-aaaaa-aaadq-cai.did";
    std::string url = "https://ic0.app";

    // Get did file content
    long file_size = did_file_size(did_file);
    std::vector<char> buffer(file_size + 1);
    int result = did_file_content(did_file, file_size, buffer.data());

    //Get principal form text
    auto principal = Principal::FromText(id_text);

    if(std::holds_alternative<std::string>(principal)) {
        std::cout<<"Error: "<<std::get<std::string>(principal)<<std::endl;
        return -1;
    }
    
    //Construct anonymoous id
    Identity anonymousIdentity;
    
    //Create agent with agent constructor
    auto agent = Agent::create_agent(url, std::move(anonymousIdentity), std::get<Principal>(principal), buffer);

    if (std::holds_alternative<std::string>(agent)) {
        std::cout<<"Error: "<<std::get<std::string>(agent)<<std::endl;
        return -1;
    }

    // Create an IdlValue object with the uint64_t value (nat64 in candid)
    uint64_t nat64 = 1974211;
    auto elem = zondax::idl_value::IdlValue(nat64);

    // Create a vector of IdlValue pointers
    std::vector<zondax::idl_value::IdlValue*> values;
    values.push_back(&elem);

    //Make Query call to canister, pass args using move semantics
    auto out = std::get<Agent>(agent).Query("lookup", IdlArgs(values));


    //Get text representation and print
    std::string out_text = std::get<IdlArgs>(out).getText();
    std::cout << out_text << std::endl;

    return 0;
}
