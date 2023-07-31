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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "zondax_ic.h"

#define ERR -1
#define OK 0

typedef struct {
    const uint8_t *ptr;
    int len;
} Error;
typedef struct {
    char *ptr;
    IdentityType type;
} CIdentity;

int get_did_file_content(const char *didFilePath, long file_size, char* buffer);
long get_did_file_size(const char *didFilePath);

#ifdef __cplusplus
}
#endif
