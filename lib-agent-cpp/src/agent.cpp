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

#include "agent.h"

#include <utility>

using zondax::IdlArgs;

namespace zondax {

void Agent::error_callback(const unsigned char* data, int len,
                           void* user_data) {
  std::string error_msg((const char*)data, len);
  *(std::string*)user_data = error_msg;
}

// declare move constructor
Agent::Agent(Agent&& o) noexcept {
  agent = o.agent;
  o.agent = nullptr;
}

// declare move assignment
Agent& Agent::operator=(Agent&& o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  // now release our inner agent.
  if (agent != nullptr) agent_destroy(agent);

  // now takes ownership of the o.agent
  agent = o.agent;

  // ensure o.agent is null
  o.agent = nullptr;

  return *this;
}

std::variant<Agent, std::string> Agent::create_agent(
    std::string url, zondax::Identity id, zondax::Principal principal,
    const std::vector<char>& did_content) {
  // string to get error message from callback
  std::string data;

  RetError ret;
  ret.user_data = (void*)&data;
  ret.call = Agent::error_callback;

  FFIAgent* c_agent = agent_create_wrap(
      url.c_str(), id.getPtr(), id.getType(), principal.getBytes().data(),
      principal.getBytes().size(), did_content.data(), &ret);

  if (c_agent == nullptr) {
    std::variant<Agent, std::string> error(data);
    return error;
  }

  // here we can use private default constructor, but users can't, also if
  // default were disabled using the delete keyboard, we would not be able to
  // use it here.
  auto cpp_agent = Agent();

  cpp_agent.agent = c_agent;
  std::variant<Agent, std::string> ok(std::move(cpp_agent));

  return ok;
}

std::variant<IdlArgs, std::string> Agent::Query(std::string service,
                                                zondax::IdlArgs& args) {
  if (agent == nullptr) {
    std::variant<IdlArgs, std::string> error{std::in_place_type<std::string>,
                                             "Agent instance uninitialized"};
    return error;
  }

  CText* arg = idl_args_to_text(args.getPtr());

  RetError ret;
  std::string data;
  ret.user_data = (void*)&data;
  ret.call = Agent::error_callback;

  IDLArgs* argsPtr =
      agent_query_wrap(agent, service.c_str(), ctext_str(arg), &ret);

  if (argsPtr == nullptr) {
    std::variant<IdlArgs, std::string> error(data);
    return error;
  }

  // IdlArgs result(argsPtr);

  std::variant<IdlArgs, std::string> ok{std::in_place_type<IdlArgs>, argsPtr};

  return ok;
}

std::variant<IdlArgs, std::string> Agent::Update(std::string service,
                                                 zondax::IdlArgs& args) {
  if (agent == nullptr) {
    std::variant<IdlArgs, std::string> error{std::in_place_type<std::string>,
                                             "Agent instance uninitialized"};
    return error;
  }

  CText* arg = idl_args_to_text(args.getPtr());

  RetError ret;
  std::string data;
  ret.user_data = (void*)&data;
  ret.call = Agent::error_callback;

  IDLArgs* argsPtr =
      agent_update_wrap(agent, service.c_str(), ctext_str(arg), &ret);

  if (argsPtr == nullptr) {
    std::variant<IdlArgs, std::string> error(data);
    return error;
  }

  std::variant<IdlArgs, std::string> ok{std::in_place_type<IdlArgs>, argsPtr};

  return ok;
}

Agent::~Agent() {
  if (agent != nullptr) agent_destroy(agent);
}

}  // namespace zondax
