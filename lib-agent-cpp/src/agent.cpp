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

#include <bits/utility.h>

#include <optional>
#include <string>
#include <utility>
#include <variant>

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

/* *********************** Query ************************/

std::variant<IdlArgs, std::string> Agent::Query(const std::string& method,
                                                zondax::IdlArgs&& args) {
  if (agent == nullptr) return std::string("Agent instance uninitialized");

  CText* arg = idl_args_to_text(args.getPtr().get());

  RetError ret;
  std::string data;
  ret.user_data = (void*)&data;
  ret.call = Agent::error_callback;

  IDLArgs* argsPtr =
      agent_query_wrap(agent, method.c_str(), ctext_str(arg), &ret);

  if (argsPtr == nullptr) return std::string(data);

  return std::move(IdlArgs(argsPtr));
}

template <typename... Args, typename>
std::variant<IdlArgs, std::string> Agent::Query(const std::string& method,
                                                Args&&... rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward(rawArgs))));

  IdlArgs args(v);

  return Query(method, std::move(args));
}

template <typename R, typename... Args, typename, typename>
std::variant<std::optional<R>, std::string> Agent::Query(
    const std::string& method, Args&&... rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward(rawArgs))));

  IdlArgs args(v);

  auto result = Query(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  auto value = std::move(returned_values.getVec()[0]);

  return value.get<R>();
}

template <typename... RArgs, typename... Args, typename, typename, typename>
std::variant<std::optional<std::tuple<RArgs...>>, std::string> Agent::Query(
    const std::string& method, Args&&... rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward(rawArgs))));

  IdlArgs args(v);

  auto result = Query(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != sizeof...(RArgs)) return std::nullopt;

  std::tuple<RArgs...> tuple;
  for (std::size_t i = 0; i < values.size(); ++i) {
    IdlValue& val = values[i];
    auto& loc = std::get<i>(tuple);

    auto mby_converted = val.get<decltype(loc)>();
    if (mby_converted.has_value()) {
      loc = std::move(mby_converted.value());
    } else {
      return std::nullopt;
    }
  }

  return std::make_optional(std::move(tuple));
}

/* *********************** Update ************************/

std::variant<IdlArgs, std::string> Agent::Update(const std::string& method,
                                                 IdlArgs&& args) {
  if (agent == nullptr) return std::string("Agent instance uninitialized");

  CText* arg = idl_args_to_text(args.getPtr().get());

  RetError ret;
  std::string data;
  ret.user_data = (void*)&data;
  ret.call = Agent::error_callback;

  IDLArgs* argsPtr =
      agent_update_wrap(agent, method.c_str(), ctext_str(arg), &ret);

  if (argsPtr == nullptr) return std::string(data);

  return std::move(IdlArgs(argsPtr));
}

template <typename... Args, typename>
std::variant<IdlArgs, std::string> Agent::Update(const std::string& method,
                                                 Args&&... rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward(rawArgs))));

  IdlArgs args(v);

  return Update(method, std::move(args));
}

template <typename R, typename... Args, typename, typename>
std::variant<std::optional<R>, std::string> Agent::Update(
    const std::string& method, Args&&... rawArgs) {
  auto result = Update(method, std::forward(rawArgs...));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != 1) return std::nullopt;

  return values[0].get<R>();
}

template <typename... RArgs, typename... Args, typename, typename, typename>
std::variant<std::optional<std::tuple<RArgs...>>, std::string> Agent::Update(
    const std::string& method, Args&&... rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward(rawArgs))));

  IdlArgs args(v);

  auto result = Update(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != sizeof...(RArgs)) return std::nullopt;

  std::tuple<RArgs...> tuple;
  for (std::size_t i = 0; i < values.size(); ++i) {
    IdlValue& val = values[i];
    auto& loc = std::get<i>(tuple);

    auto mby_converted = val.get<decltype(loc)>();
    if (mby_converted.has_value()) {
      loc = std::move(mby_converted.value());
    } else {
      return std::nullopt;
    }
  }

  return std::make_optional(std::move(tuple));
}

Agent::~Agent() {
  if (agent != nullptr) agent_destroy(agent);
}

}  // namespace zondax
