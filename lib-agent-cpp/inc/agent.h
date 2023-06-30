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

  /**
   * Performs a query using the specified method and arguments.
   *
   * @param method The method to query.
   * @param args The arguments for the query.
   * @return A variant that can contain either `IdlArgs` or a string error
   * message.
   *
   * @remarks This function is used internally by the generic implementation and
   * should not be called directly.
   */
  std::variant<IdlArgs, std::string> Query(const std::string &method,
                                           zondax::IdlArgs &&args);

 public:
  // Disable copies, just move semantics
  Agent(const Agent &args) = delete;
  void operator=(const Agent &) = delete;

  // declare move constructor
  Agent(Agent &&o) noexcept;
  // declare move assignment
  Agent &operator=(Agent &&o) noexcept;

  static std::variant<Agent, std::string> create_agent(
      std::string url, zondax::Identity id, zondax::Principal &principal,
      const std::vector<char> &did_content);

  ~Agent();

  /**
   * Performs a query using the specified method and arguments.
   * The arguments `args` are forwarded to construct `IdlValue` objects.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam R The return type of the query.
   * @param method The method to call.
   * @param args The arguments for the call.
   * @return A variant containing the call result or an error string.
   */
  template <typename... Args,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  std::variant<IdlArgs, std::string> Query(const std::string &method,
                                           Args &&...);

  /**
   * Performs a query using the specified method and arguments.
   * The return type `R` must be constructible from `IdlValue`.
   * The arguments `rawArgs` are forwarded to construct `IdlValue` objects.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam R The return type of the query.
   * @param method The method to query.
   * @param args The arguments for the query.
   * @return A variant containing the converted query result (if the type
   * matches) or an error string.
   */
  template <typename R, typename... Args,
            typename = std::enable_if_t<std::is_constructible_v<IdlValue, R>>,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  std::variant<std::optional<R>, std::string> Query(const std::string &method,
                                                    Args &&...args);
  /**
   * Performs a query using the specified method and arguments.
   * This version is specialized for tuple return types, should be used when the
   * method returns multiple values.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam RArgs Variadic template parameter pack for the return types
   * of the tuple.
   * @param method The method to query.
   * @param args The tuple of arguments for the query.
   * @return A variant containing the converted query result (if the types
   * match) in the form of a tupleor an error string.
   */
  template <
      typename... RArgs, typename... Args,
      typename =
          std::enable_if_t<(std::is_constructible_v<IdlValue, RArgs> && ...)>,
      typename =
          std::enable_if_t<(std::is_constructible_v<IdlValue, Args> && ...)>,
      typename = std::enable_if_t<!std::is_same_v<RArgs..., IdlArgs>>>
  std::variant<std::optional<std::tuple<RArgs...>>, std::string> Query(
      const std::string &method, Args &&...args);

  /**
   * Performs an update using the specified method and arguments.
   *
   * @param method The method to call.
   * @param args The arguments for the call.
   * @return A variant that can contain either `IdlArgs` or a string error
   * message.
   *
   * @remarks This function is used internally by the generic implementation and
   * should not be called directly.
   */
  std::variant<IdlArgs, std::string> Update(const std::string &method,
                                            zondax::IdlArgs &&args);

  /**
   * Performs an update using the specified method and arguments.
   * The arguments `args` are forwarded to construct `IdlValue` objects.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam R The return type of the call.
   * @param method The method to call.
   * @param args The arguments for the call.
   * @return A variant containing the call result or an error string.
   */
  template <typename... Args,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  std::variant<IdlArgs, std::string> Update(const std::string &method,
                                            Args &&...);

  /**
   * Performs an update using the specified method and arguments.
   * The return type `R` must be constructible from `IdlValue`.
   * The arguments `args` are forwarded to construct `IdlValue` objects.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam R The return type of the call.
   * @param method The method to call.
   * @param args The arguments for the call.
   * @return A variant containing the converted call result (if the type
   * matches) or an error string.
   */
  template <typename R, typename... Args,
            typename = std::enable_if_t<std::is_constructible_v<IdlValue, R>>,
            typename = std::enable_if_t<
                (std::is_constructible_v<IdlValue, Args> && ...)>>
  std::variant<std::optional<R>, std::string> Update(const std::string &method,
                                                     Args &&...);

  /**
   * Performs an update using the specified method and arguments.
   * This version is specialized for tuple return types, should be used when the
   * method returns multiple values.
   *
   * @tparam Args Variadic template parameter pack for the argument types.
   * @tparam RArgs Variadic template parameter pack for the return types
   * of the tuple.
   * @param method The method to call.
   * @param args The tuple of arguments for the call.
   * @return A variant containing the converted call result (if the types
   * match) in the form of a tupleor an error string.
   */
  template <
      typename... RArgs, typename... Args,
      typename =
          std::enable_if_t<(std::is_constructible_v<IdlValue, RArgs> && ...)>,
      typename =
          std::enable_if_t<(std::is_constructible_v<IdlValue, Args> && ...)>,
      typename = std::enable_if_t<!std::is_same_v<RArgs..., IdlArgs>>>
  std::variant<std::optional<std::tuple<RArgs...>>, std::string> Update(
      const std::string &method, Args &&...args);
};

template <typename... Args, typename>
std::variant<IdlArgs, std::string> Agent::Query(const std::string &method,
                                                Args &&...rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward<Args>(rawArgs))));

  IdlArgs args(v);

  return Query(method, std::move(args));
}

template <typename R, typename... Args, typename, typename>
std::variant<std::optional<R>, std::string> Agent::Query(
    const std::string &method, Args &&...rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward<Args>(rawArgs))));

  IdlArgs args(v);

  auto result = Query(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  auto value = std::move(returned_values.getVec()[0]);

  return value.get<R>();
}

template <typename... RArgs, typename... Args, typename, typename, typename>
std::variant<std::optional<std::tuple<RArgs...>>, std::string> Agent::Query(
    const std::string &method, Args &&...rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward<Args>(rawArgs))));

  IdlArgs args(v);

  auto result = Query(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != sizeof...(RArgs)) return std::nullopt;

  std::tuple<RArgs...> tuple;
  for (std::size_t i = 0; i < values.size(); ++i) {
    IdlValue &val = values[i];
    auto &loc = std::get<i>(tuple);

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

template <typename... Args, typename>
std::variant<IdlArgs, std::string> Agent::Update(const std::string &method,
                                                 Args &&...rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward<Args>(rawArgs))));

  IdlArgs args(v);

  return Update(method, std::move(args));
}

template <typename R, typename... Args, typename, typename>
std::variant<std::optional<R>, std::string> Agent::Update(
    const std::string &method, Args &&...rawArgs) {
  auto result = Update(method, std::forward<Args>(rawArgs)...);

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != 1) return std::nullopt;

  return values[0].get<R>();
}

template <typename... RArgs, typename... Args, typename, typename, typename>
std::variant<std::optional<std::tuple<RArgs...>>, std::string> Agent::Update(
    const std::string &method, Args &&...rawArgs) {
  std::vector<IdlValue> v;
  v.reserve(sizeof...(rawArgs));

  (..., v.emplace_back(IdlValue(std::forward<Args>(rawArgs))));

  IdlArgs args(v);

  auto result = Update(method, std::move(args));

  if (result.index() == 1) return std::get<1>(result);

  auto returned_values = std::move(std::get<0>(result));
  std::vector<IdlValue> values = std::move(returned_values.getVec());

  if (values.size() != sizeof...(RArgs)) return std::nullopt;

  std::tuple<RArgs...> tuple;
  for (std::size_t i = 0; i < values.size(); ++i) {
    IdlValue &val = values[i];
    auto &loc = std::get<i>(tuple);

    auto mby_converted = val.get<decltype(loc)>();
    if (mby_converted.has_value()) {
      loc = std::move(mby_converted.value());
    } else {
      return std::nullopt;
    }
  }

  return std::make_optional(std::move(tuple));
}

}  // namespace zondax

#endif  // IDENTITY_H
