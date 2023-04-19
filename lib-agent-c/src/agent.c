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

#include "bindings.h"
#include "helper.h"

#include "agent.h"

/**
 * @brief Create agent instance
 *
 * @param url  url points to the ic net
 * @param id agent assignied indetity
 * @param canister canister identity
 * @param did_content .did file content
 * @param agent returned agent to make calls to a Replica endpoint.
 * @param error_cb returned error
 * @return ResultCode 0:ok -1:error
 */
ResultCode agent_create(const char *url, struct Identity *id, struct Principal *canister,
                const char *did_content, const struct FFIAgent **agent,
                RetPtr_u8 error_cb) {

    return agent_create_wrap(url, id->ptr, id->type, canister->ptr,
                                          canister->len, did_content, agent,
                                          error_cb);
}

/**
 * @brief Calls and returns the information returned by the status endpoint of a replica
 *
 * @param agent agent to get status from
 * @param status_cb returned agent status
 * @param error_cb returned error
 * @return ResultCode 0:ok -1:error
 */
ResultCode agent_status(const struct FFIAgent *agent, RetPtr_u8 status_cb,
                RetPtr_u8 error_cb) {

    return agent_status_wrap(agent, status_cb, error_cb);
}

/**
 * @brief Update call
 *
 * @param agent agent to make the call
 * @param method canister method (verified with .did content)
 * @param method_args arguments required by method
 * @param ret method response
 * @param error_cb returned error
 * @return ResultCode 0:ok -1:error
 */
ResultCode agent_update(const struct FFIAgent *agent, const char *method,
                const char *method_args, const void **ret, RetPtr_u8 error_cb) {

    return agent_update_wrap(agent, method, method_args, ret, error_cb);
}

/**
 * @brief Query call
 *
 * @param agent agent to make the call
 * @param method canister method (verified with .did content)
 * @param method_args arguments required by method
 * @param ret method response
 * @param error_cb returned error
 * @return ResultCode 0:ok -1:error
 */
ResultCode agent_query(const struct FFIAgent *agent, const char *method,
               const char *method_args, const void **ret, RetPtr_u8 error_cb) {

    return agent_query_wrap(agent, method, method_args, ret, error_cb);

}
