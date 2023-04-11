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

#include "idl.h"

uint8_t *out;
int out_len;

static RetPtr_u8 text_cb(const uint8_t *p, int len) {
    out = malloc(len);
    out_len=len;
    memcpy(out,p,len);

    return LIB_C_OK;
}

int idl_args_to_text(const void *args, struct Text *text){
    
    idl_args_to_text_wrap(args,*(RetPtr_u8)text_cb);
    if (out == NULL) {
        return LIB_C_ERROR;
    }
    text->ptr = out;
    text->len = out_len;

    return LIB_C_OK;
}

void idl_free(void) {
    free(out);
}

