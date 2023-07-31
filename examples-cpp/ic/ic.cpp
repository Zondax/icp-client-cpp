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

#include "../../examples-cpp/ic/declarations/example/example.hpp"
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
  auto agent = Agent::create_agent(url, std::move(anonymousIdentity),
                                   std::get<Principal>(principal), buffer);

  if (std::holds_alternative<std::string>(agent)) {
    std::cerr << "Error: " << std::get<std::string>(agent) << std::endl;
    return -1;
  }

  SERVICE srv(std::move(std::get<Agent>(agent)));

  auto out = srv.lookup(1974211);
  if (std::holds_alternative<std::string>(agent)) {
    std::cerr << "Error: " << std::get<std::string>(agent) << std::endl;
    return -1;
  }

  auto deviceData = std::get<0>(out);

  for (auto device : deviceData) {
    std::cout << "Device alias: " << device.alias << std::endl;
    // std::cout << std::hex
    //           << (device.credential_id.has_value()
    //                   ? device.credential_id.value()
    //                   : std::vector<uint8_t>(0))
    //           << std::endl;
    std::cout << "---------------------------------" << std::endl;
  }

  return 0;
}
