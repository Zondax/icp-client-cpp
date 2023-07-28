// This is an experimental feature to generate C++ bindings from Candid.
// You may want to manually adjust some of the types.

#pragma once

#include <optional>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

//lib-agent-cpp
#include "agent.h"
#include "func.h"
#include "idl_value.h"
#include "principal.h"

class SERVICE {
private:
zondax::Agent agent;
public:
SERVICE(zondax::Agent&& agent): agent(std::move(agent)) {}
std::variant<uint64_t, std::string>get(){
  auto result = agent.Query<uint64_t>("get");
  if (result.index() == 0){
    return std::variant<
      uint64_t
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      uint64_t
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::monostate, std::string>increment(){
  auto result = agent.Update<std::monostate>("increment");
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::monostate, std::string>set(uint64_t arg0){
  auto result = agent.Update<std::monostate>("set", arg0);
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
};
