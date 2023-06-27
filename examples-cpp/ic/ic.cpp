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
#include "helper.h"
#include "idl_value.h"

using namespace zondax;

int main() {
  // Canister info from hello world deploy example
  std::string id_text = "rdmx6-jaaaa-aaaaa-aaadq-cai";
  // path is relative to binary location, not source
  std::string did_file = "../examples/ic_c/rdmx6-jaaaa-aaaaa-aaadq-cai.did";
  std::string url = "https://ic0.app";

  std::vector<char> buffer;
  auto bytes_read = did_file_content(did_file, buffer);

  // Get principal form text
  auto principal = Principal::FromText(id_text);

  if (std::holds_alternative<std::string>(principal)) {
    return -1;
  }

  // Construct anonymoous id
  Identity anonymousIdentity;

  // Create agent with agent constructor
  auto agent =
      Agent::create_agent(url, std::move(anonymousIdentity),
                          std::move(std::get<Principal>(principal)), buffer);

  if (std::holds_alternative<std::string>(agent)) {
    std::cerr << "Error: " << std::get<std::string>(agent) << std::endl;
    return -1;
  }

  // Create an IdlValue object with the uint64_t value (nat64 in candid)
  uint64_t nat64 = 1974211;

  // Create a vector of IdlValue pointers
  std::vector<IdlValue> values;
  values.emplace_back(IdlValue(nat64));

  auto args = IdlArgs(std::move(values));

  // Make Query call to canister, pass args using move semantics
  auto out = std::get<Agent>(agent).Query("lookup", args);

  // Get text representation and print
  std::string out_text = std::get<IdlArgs>(out).getText();
  std::cout << out_text << std::endl;

  return 0;
}
