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
#pragma once
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Status {
  const uint8_t *ptr;
  int len;
};

int agent_create(const char *url, struct Identity *id,
                 struct Principal *canister, const char *did_content,
                 const struct FFIAgent **agent, struct Error *e);
int agent_status(const struct FFIAgent *agent, struct Status *s,
                 struct Error *e);
int agent_update(const struct FFIAgent *agent, const char *method,
                 const char *method_args, const void **ret, struct Error *e);
int agent_query(const struct FFIAgent *agent, const char *method,
                const char *method_args, const void **ret, struct Error *e);
void free_agent(void);
