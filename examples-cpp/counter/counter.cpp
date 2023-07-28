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
#include "../../examples-cpp/counter/declarations/counter/counter.hpp"
using namespace zondax;

int main() {
  // Canister info from hello world deploy example
  std::string id_text = "q4eej-kyaaa-aaaaa-aaaha-cai";
  // path is relative to binary location, not source
  std::string did_file = "../examples-cpp/counter/declarations/counter/counter_backend.did";
  std::string url = "http://127.0.0.1:4943";

  std::vector<char> buffer;
  auto bytes_read = did_file_content(did_file, buffer);

  // Get principal form text
  auto principal = Principal::FromText(id_text);

  if (std::holds_alternative<std::string>(principal)) {
     std::cerr << "Error: " << std::get<std::string>(principal) << std::endl;
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

  auto out = srv.set(20);

  auto result = srv.get();
  std::visit([](const auto& value) {
    // Check if the value is an uint64_t
    if constexpr (std::is_same_v<std::decay_t<decltype(value)>, uint64_t>) {
        std::cout << "uint64_t value: " << value << std::endl;
    }
    // Check if the value is a string
    else if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::string>) {
        std::cout << "string value: " << value << std::endl;
    }
    // Handle any other types if needed
    else {
        std::cout << "Unknown type!" << std::endl;
    }
  }, result);

  return 0;
}
