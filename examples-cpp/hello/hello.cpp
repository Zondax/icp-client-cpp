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
#include "../../examples-cpp/hello/declarations/rust_hello/rust_hello.hpp"
using namespace zondax;

int main() {
  // Canister info from hello world deploy example
  std::string id_text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
  std::string did_file = "../examples-cpp/hello/declarations/rust_hello/rust_hello_backend.did";
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

  std::string arg = "Zondax";
  auto out = srv.greet(arg);
  // Get the first string value from the variant
  //std::string str1 = std::get<0>(out);

  // Get the second string value from the variant
  std::string str2 = std::get<1>(out);

  // Print the strings
  //std::cout << "String 1: " << str1 << std::endl;
  std::cout << "String 2: " << str2 << std::endl;

  return 0;
}
