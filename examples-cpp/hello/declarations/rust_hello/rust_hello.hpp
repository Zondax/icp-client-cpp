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
std::variant<std::string, std::string>greet(std::string arg0){
  auto result = agent.Query<std::string>("greet", arg0);
  if (result.index() == 0){
    return std::variant<
      std::string
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::string
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
};
