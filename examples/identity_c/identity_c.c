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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zondax_ic.h"
#include "helper_c.h"
#include "agent_c.h"
#include "identity_c.h"

Error error;

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len, void *) {
    if (error.ptr != NULL) {
        free((void *)error.ptr);
    }
    error.ptr = malloc(len);
    error.len = len;
    memcpy((void *) error.ptr, p, len);
}

const char *BasicIdentityFile =
    "-----BEGIN PRIVATE KEY-----\n"
    "MFMCAQEwBQYDK2VwBCIEIL9r4XBKsg4pquYBHY6rgfzuBsvCy89tgqDfDpofXRBP\n"
    "oSMDIQBCkE1NL4X43clXS1LFauiceiiKW9NhjVTEpU6LpH9Qcw==\n"
    "-----END PRIVATE KEY-----";

const char *Secp256K1IdentityFile =
    "-----BEGIN EC PARAMETERS-----\n"
    "BgUrgQQACg==\n"
    "-----END EC PARAMETERS-----\n"
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MHQCAQEEIAgy7nZEcVHkQ4Z1Kdqby8SwyAiyKDQmtbEHTIM+WNeBoAcGBSuBBAAK\n"
    "oUQDQgAEgO87rJ1ozzdMvJyZQ+GABDqUxGLvgnAnTlcInV3NuhuPv4O3VGzMGzeB\n"
    "N3d26cRxD99TPtm8uo2OuzKhSiq6EQ==\n"
    "-----END EC PRIVATE KEY-----";

static const unsigned char PubKeyExpected[] = {
    0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00, 0x42, 0x90,
    0x4d, 0x4d, 0x2f, 0x85, 0xf8, 0xdd, 0xc9, 0x57, 0x4b, 0x52, 0xc5, 0x6a, 0xe8, 0x9c,
    0x7a, 0x28, 0x8a, 0x5b, 0xd3, 0x61, 0x8d, 0x54, 0xc4, 0xa5, 0x4e, 0x8b, 0xa4, 0x7f,
    0x50, 0x73
};

static const unsigned char SignatureExpected[] = {
    0x6d, 0x7a, 0x2f, 0x85, 0xeb, 0x6c, 0xc2, 0x18, 0x80, 0xc8, 0x3d, 0x9b, 0xb1, 0x70,
    0xe2, 0x4b, 0xf5, 0xd8, 0x9a, 0xa9, 0x96, 0x92, 0xb6, 0x89, 0xac, 0x9d, 0xe9, 0x5c,
    0x1e, 0x3e, 0x50, 0xdc, 0x98, 0x12, 0x2f, 0x94, 0x11, 0x2f, 0x6c, 0xc6, 0x6a, 0x0b,
    0xbf, 0xc0, 0x56, 0x5b, 0xdb, 0x87, 0xa9, 0xe2, 0x2c, 0x8e, 0x56, 0x94, 0x56, 0x12,
    0xde, 0xbf, 0x22, 0x4a, 0x3f, 0xdb, 0xf1, 0x03
};

const char* basic_expected = "emrl6-qe3wz-fh5ib-sx2r4-fbx46-6g4ql-5ro3g-zhbtm-nxdrq-q2oqo-jqe";
const char* secp256_expected = "t2kpu-6xt6l-tyb3d-rll2p-irv5c-no5nd-h6spj-jsetq-bmqdz-iap77-pqe";

int main(void) {
    RetError ret_error;
    ret_error.call = error_cb;

    printf("+++++++++ Testing exported Identity Core Functions +++++++++\n");
    
    CIdentity id = {0};

    // Create anonymous ID and sender so we verify with principal anonymous
    anonymous_identity(&id);
    // Use idenity sender to get principal ID
    CPrincipal *p = identity_sender(id.ptr, id.type, &ret_error);
    if (p->len == 1 && p->ptr[0] == 4) {
        printf(" Test 1: Valid Anonym identity.\n");
    } else {
        printf(" Test 1: Error.\n");
    }

    // Create Basic ID 
    basic_identity_from_pem(BasicIdentityFile, &id, &ret_error);

    principal_destroy(p);
    p = identity_sender(id.ptr, id.type, &ret_error);
    CPrincipal *p_text = principal_to_text(p->ptr,p->len,&ret_error);
    if (!strcmp((const char *)p_text->ptr, basic_expected)) {
        printf(" Test 2: Valid Basic from file identity.\n");
    } else {
        printf(" Test 2: Error.\n");
    }

    // Create Basic idenity again because identity_sender takes ownership of the memory
    // on the rust side so try to use the previous pointer might have an undefined behaviour
    basic_identity_from_pem(BasicIdentityFile, &id, &ret_error);
    CIdentitySign *sign = identity_sign(NULL, 0, id.ptr, id.type, &ret_error);
    const uint8_t *pubkey = cidentitysign_pubkey(sign);
    const uint8_t *signature = cidentitysign_sig(sign);

    if (!memcmp(pubkey, PubKeyExpected, cidentitysign_pubkey_len(sign)) 
        && !memcmp(signature, SignatureExpected, cidentitysign_sig_len(sign))) {
        printf(" Test 3: Valid Signature.\n");
    } else {
        printf(" Test 3: Error.\n");
    }

    // Create Secp256k1 ID 
    secp256k1_identity_from_pem(Secp256K1IdentityFile, &id, &ret_error);

    principal_destroy(p);
    p = identity_sender(id.ptr, id.type, &ret_error);

    principal_destroy(p_text);
    p_text = principal_to_text(p->ptr,p->len,&ret_error);
    if (!strcmp((const char *)p_text->ptr, secp256_expected)) {
        printf(" Test 4: Valid Secp256k1 from file identity.\n");
    } else {
        printf(" Test 4: Error.\n");
    }

    principal_destroy(p);
    principal_destroy(p_text);
    cidentitysign_destroy(sign);

    return 0;
}
