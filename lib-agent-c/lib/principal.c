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

uint8_t *out;
uint8_t *error;
int out_len;
int error_len;

static RetPtr_u8 principal_cb(const uint8_t *p, int len) {
    out = malloc(len);
    out_len=len;
    memcpy(out,p,len);

    return LIB_C_OK;
}

static RetPtr_u8 error_principal_cb(const uint8_t *p, int len) {
    error = malloc(len);
    error_len=len;
    memcpy(error,p,len);

    return LIB_C_OK;
}

int get_principal_management_canister(struct Principal *p) {
    principal_management_canister_wrap(*(RetPtr_u8)principal_cb);
    if (out == NULL) {
        return LIB_C_ERROR;
    }
    p->ptr = out;
    p->len = out_len;

    return LIB_C_OK;
}

int get_principal_anonymous(struct Principal *p) {
    principal_anonymous_wrap(*(RetPtr_u8)principal_cb);
    if (out == NULL) {
        return LIB_C_ERROR;
    }
    p->ptr = out;
    p->len = out_len;

    return LIB_C_OK;
}

int get_principal_self_authenticating(const uint8_t *pk, int pkLen,
                                      struct Principal *p) {

    principal_self_authenticating_wrap(pk, pkLen, *(RetPtr_u8)principal_cb);
    if (out == NULL) {
        return LIB_C_ERROR;
    }
    p->ptr = out;
    p->len = out_len;

    return LIB_C_OK;
}

int get_principal_from_bytes(const uint8_t *bytes, int bytesLen,
                             struct Principal *p) {

    principal_from_slice_wrap(bytes, bytesLen, *(RetPtr_u8)principal_cb);
    if (out == NULL) {
        return LIB_C_ERROR;
    }
    p->ptr = out;
    p->len = out_len;

    return LIB_C_OK;
}

int get_principal_try_from_bytes(const uint8_t *bytes, int bytesLen,
                                 struct Principal *p, struct Error *e) {

    ResultCode result = principal_try_from_slice_wrap(bytes, bytesLen,
                                                 *(RetPtr_u8)principal_cb,
                                                 *(RetPtr_u8)error_principal_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        p->ptr = NULL;
        p->len = 0;
        return LIB_C_ERROR;
    }

    p->ptr = out;
    p->len = out_len;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int get_principal_from_text(const char *text, struct Principal *p,
                            struct Error *e) {

    ResultCode result = principal_from_text_wrap(text, *(RetPtr_u8)principal_cb,
                                                 *(RetPtr_u8)error_principal_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        p->ptr = NULL;
        p->len = 0;
        return LIB_C_ERROR;
    }

    p->ptr = out;
    p->len = out_len;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int get_principal_to_text(const uint8_t *bytes, int bytesLen, struct Principal *p,
                          struct Error *e) {

    ResultCode result = principal_to_text_wrap(bytes, bytesLen,
                                          *(RetPtr_u8)principal_cb,
                                          *(RetPtr_u8)error_principal_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        p->ptr = NULL;
        p->len = 0;
        return LIB_C_ERROR;
    }

    p->ptr = out;
    p->len = out_len;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

void principal_free(void) {
    free(out);
    free(error);
}
