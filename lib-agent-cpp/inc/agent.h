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
#include <iostream>
#include <vector>

#include "identity.h"
#include "idl_args.h"
#include "principal.h"

extern "C" {
#include "zondax_ic.h"
}

namespace zondax::agent {
class Agent {
    private:
        FFIAgent* agent;

    public:
        Agent(std::string url, zondax::identity::Identity id,
            zondax::principal::Principal principal,
            const std::vector<char>& did_content, RetPtr_u8 error);
        ~Agent();

        zondax::idl_args::IdlArgs Query(std::string service,
                                        zondax::idl_args::IdlArgs args,
                                        RetPtr_u8 error);
        zondax::idl_args::IdlArgs Update(std::string service,
                                        zondax::idl_args::IdlArgs args,
                                        RetPtr_u8 error);
        std::string Status(RetPtr_u8 error);
};
}  // namespace zondax::agent

#endif  // IDENTITY_H
