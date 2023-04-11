/*******************************************************************************
*   (c) 2018 - 2022 Zondax AG
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
#include "principal.h"
#include "identity.h"

#include "agent.h"

uint8_t *error;
uint8_t *status;
int error_len;
int status_len;

static RetPtr_u8 status_agent_cb(const uint8_t *p, int len) {
    status = malloc(len);
    status_len=len;
    memcpy(status,p,len);

    return LIB_C_OK;
}

static RetPtr_u8 error_agent_cb(const uint8_t *p, int len) {
    error = malloc(len);
    error_len=len;
    memcpy(error,p,len);

    return LIB_C_OK;
}

int agent_create(const char *url, struct Identity *id, struct Principal *canister,
                const char *did_content, const struct FFIAgent **agent,
                struct Error *e) {

    ResultCode result = agent_create_wrap(url, id->identity, id->type, canister->ptr,
                                          canister->len, did_content, agent,
                                          *(RetPtr_u8)error_agent_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }
    return LIB_C_OK;
}

int agent_status(const struct FFIAgent *agent, struct Status *s,
                struct Error *e) {

    ResultCode result = agent_status_wrap(agent, *(RetPtr_u8)status_agent_cb,
                                          *(RetPtr_u8)error_agent_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }
    s->ptr = status;
    s->len = status_len;

    return LIB_C_OK;
}

int agent_update(const struct FFIAgent *agent, const char *method,
                const char *method_args, const void **ret, struct Error *e) {

    ResultCode result = agent_update_wrap(agent, method, method_args, ret,
                                          *(RetPtr_u8)error_agent_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }
    return LIB_C_OK;
}

int agent_query(const struct FFIAgent *agent, const char *method,
               const char *method_args, const void **ret, struct Error *e) {

    ResultCode result = agent_query_wrap(agent, method, method_args, ret,
                                         *(RetPtr_u8)error_agent_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }
    return LIB_C_OK;
}

void freeAgent(void) {
    free(error);
    free(status);
}
