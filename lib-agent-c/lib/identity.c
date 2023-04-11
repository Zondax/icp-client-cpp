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

void *id_tmp;
uint8_t *out;
uint8_t *error;
uint8_t *sig;
uint8_t *pk;

int out_len;
int error_len;
int sig_len;
int pk_len;

static RetPtr_u8 sig_cb(const uint8_t *p, int len) {
    sig = malloc(len);
    sig_len=len;
    memcpy(sig,p,len);

    return LIB_C_OK;
}

static RetPtr_u8 pk_cb(const uint8_t *p, int len) {
    pk = malloc(len);
    pk_len=len;
    memcpy(pk,p,len);

    return LIB_C_OK;
}

static RetPtr_u8 principal_id_cb(const uint8_t *p, int len) {
    out = malloc(len);
    out_len=len;
    memcpy(out,p,len);

    return LIB_C_OK;
}

static RetPtr_u8 error_id_cb(const uint8_t *p, int len) {
    error = malloc(len);
    error_len=len;
    memcpy(error,p,len);

    return LIB_C_OK;
}

static void init_id_tmp(int len){
    id_tmp = malloc(len);
}

int get_identity_anonymous(struct Identity *id) {
    init_id_tmp(ANONYMOUD_ID_LEN);
    identity_anonymous_wrap(id_tmp);
    id->identity=id_tmp;
    id->type = Anonym;
    return LIB_C_OK;
}

int get_identity_basic_from_pem(const char *pem, struct Identity *id,
                                struct Error *e) {

    ResultCode result = identity_basic_from_pem_wrap(pem, id->identity,
                                                *(RetPtr_u8)error_id_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }

    id->type = Basic;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int get_identity_basic_from_key_pair(const uint8_t *publicKey,
                                     const uint8_t *privateKeySeed,
                                     struct Identity *id, struct Error *e) {

    ResultCode result = identity_basic_from_key_pair_wrap(publicKey, privateKeySeed,
                                                          id->identity,
                                                          *(RetPtr_u8)error_id_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }

    id->type = Basic;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int get_identity_secp256k1_from_pem(const char *pem, struct Identity *id,
                                    struct Error *e) {

    ResultCode result = identity_secp256k1_from_pem_wrap(pem, id->identity,
                                                         *(RetPtr_u8)error_id_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }

    id->type = Secp256k1;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int get_identity_secp256k1_from_privateKey(const char *privateKey, uintptr_t pkLen,
                                           struct Identity *id) {

    identity_secp256k1_from_private_key_wrap(privateKey, pkLen, id->identity);

    if (id->identity == NULL) return LIB_C_ERROR;

    id->type = Secp256k1;
    return LIB_C_OK;
}

int identity_sender(struct Identity *id, struct Principal *principal,
                    struct Error *e) {

    ResultCode result = identity_sender_wrap(id->identity, id->type,
                                             *(RetPtr_u8)principal_id_cb,
                                             *(RetPtr_u8)error_id_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }

    id->type = Secp256k1;
    principal->ptr = out;
    principal->len = out_len;
    e->ptr = NULL;
    e->len = 0;

    return LIB_C_OK;
}

int identity_sign(const uint8_t *bytes, int bytesLen, struct Identity *id,
                 struct PublicKey *publicKey, struct Signature *signature,
                 struct Error *e) {

    ResultCode result = identity_sign_wrap(bytes, bytesLen, id->identity, id->type,
                                           *(RetPtr_u8)pk_cb, *(RetPtr_u8)sig_cb,
                                           *(RetPtr_u8)error_id_cb);

    if (result == LIB_C_ERROR) {
        e->ptr = error;
        e->len = error_len;
        return LIB_C_ERROR;
    }

    publicKey->ptr = pk;
    publicKey->len = pk_len;
    signature->ptr = sig;
    signature->len = sig_len;
    e->ptr = NULL;
    e->len = 0;
    return LIB_C_OK;
}

void identity_free(void) {
    free(sig);
    free(pk);
    free(out);
    free(error);
    free(id_tmp);
}
