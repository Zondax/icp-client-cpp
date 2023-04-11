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
#include "helper.h"

struct Principal {
  const uint8_t *ptr;
  int len;
};

int get_principal_management_canister(struct Principal *p);

int get_principal_anonymous(struct Principal *p);

int get_principal_self_authenticating(const uint8_t *pk, int pkLen,
                                   struct Principal *p);

int get_principal_from_bytes(const uint8_t *bytes, int bytesLen,
                          struct Principal *p);

int get_principal_try_from_bytes(const uint8_t *bytes, int bytesLen,
                             struct Principal *p, struct Error *e);

int get_principal_from_text(const char *text, struct Principal *p,
                         struct Error *e);

int get_principal_to_text(const uint8_t *bytes, int bytesLen, struct Principal *p,
                       struct Error *e);

void principal_free(void);
