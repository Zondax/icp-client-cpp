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
#ifndef AGENT_H
#define AGENT_H

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "identity.h"
#include "idl_args.h"
#include "idl_value.h"
#include "principal.h"
#include "service.h"

extern "C" {
#include "zondax_ic.h"
}

using zondax::IdlArgs;

namespace zondax {

class Agent {
 private:
  FFIAgent *agent;

  Agent() noexcept { agent = nullptr; };

  static void error_callback(const unsigned char *data, int len,
                             void *user_data);

 public:
  // Disable copies, just move semantics
  Agent(const Agent &args) = delete;
  void operator=(const Agent &) = delete;

  // declare move constructor
  Agent(Agent &&o) noexcept;
  // declare move assignment
  Agent &operator=(Agent &&o) noexcept;

  static std::variant<Agent, std::string> create_agent(
      std::string url, zondax::Identity id, zondax::Principal principal,
      const std::vector<char> &did_content);

  ~Agent();

  template <typename... Args,
            typename std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>,
  std::variant<IdlArgs, std::string> Query(const std::string &method,
                                           Args &&...);

  std::variant<IdlArgs, std::string> Query(const std::string &method,
                                           zondax::IdlArgs &&args);

  template <typename... Args,
            typename std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  std::variant<IdlArgs, std::string> Update(const std::string &method,
                                            Args &&...);

  std::variant<IdlArgs, std::string> Update(const std::string &method,
                                            zondax::IdlArgs &&args);
};
}  // namespace zondax

#endif  // IDENTITY_H
