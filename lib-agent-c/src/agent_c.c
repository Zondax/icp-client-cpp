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
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "zondax_ic.h"
#include "helper_c.h"

#include "agent_c.h"
/**
 * @brief Create and Returns agent instance 
 *
 * @param url  url points to the ic net
 * @param id agent assignied indetity
 * @param canister canister identity
 * @param did_content .did file content
 * @param error_cb returned error
 * @return FFIAgent pointer 
 */
FFIAgent *agent_create(const char *url, CIdentity *id, CPrincipal *canister,
                const char *did_content, RetError *error_cb) {

    return agent_create_wrap(url, id->ptr, id->type, canister->ptr,
                                          canister->len, did_content,
                                          error_cb);
}

/**
 * @brief Calls and returns the information returned by the status endpoint of a replica
 *
 * @param agent agent to get status from
 * @param error_cb returned error
 * @return agent call result
 */
CText* agent_status(const struct FFIAgent *agent, RetError *error_cb) {
    return agent_status_wrap(agent, error_cb);
}

/**
 * @brief Update call
 *
 * @param agent agent to make the call
 * @param method canister method (verified with .did content)
 * @param method_args arguments required by method
 * @param error_cb returned error
 * @return agent call result
 */
IDLArgs *agent_update(const struct FFIAgent *agent, const char *method,
                IDLArgs *method_args, RetError *error_cb) {

    CText *arg = idl_args_to_text(method_args);
    return agent_update_wrap(agent, method, ctext_str(arg), error_cb);
}

/**
 * @brief Query call
 *
 * @param agent agent to make the call
 * @param method canister method (verified with .did content)
 * @param method_args arguments required by method
 * @param error_cb returned error
 * @return agent call result
 */
IDLArgs *agent_query(const struct FFIAgent *agent, const char *method,
                IDLArgs *method_args, RetError *error_cb) {

    CText *arg = idl_args_to_text(method_args);
    return agent_query_wrap(agent, method, ctext_str(arg), error_cb);
}


