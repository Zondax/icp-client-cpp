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

namespace zondax::agent {

Agent::Agent(std::string url, zondax::identity::Identity id, zondax::principal::Principal principal,
                const std::vector<char>& did_content, RetPtr_u8 error) {

        agent = agent_create_wrap(url.c_str(), id.getPtr(), id.getType(), principal.getBytes().data(), principal.getBytes().size(), did_content.data(), error);
}

zondax::idl_args::IdlArgs Agent::Query(std::string service,
                                        zondax::idl_args::IdlArgs args,
                                        RetPtr_u8 error){
    CText *arg = idl_args_to_text(args.getPtr());
    IDLArgs* argsPtr = agent_query_wrap(agent, service.c_str(), ctext_str(arg), error);
    zondax::idl_args::IdlArgs result(argsPtr);
    return result;
}

zondax::idl_args::IdlArgs Agent::Update(std::string service,
                                        zondax::idl_args::IdlArgs args,
                                        RetPtr_u8 error){
    CText *arg = idl_args_to_text(args.getPtr());
    IDLArgs* argsPtr = agent_update_wrap(agent, service.c_str(), ctext_str(arg), error);
    zondax::idl_args::IdlArgs result(argsPtr);
    return result;
}

Agent::~Agent(){
    agent_destroy(agent);
}

}
