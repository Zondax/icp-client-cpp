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
#include "bindings.h"
#include "helper.h"
#include "agent.h"
#include "identity.h"

Error error;

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len) {
    if (error.ptr != NULL) {
        free((void *)error.ptr);
    }
    error.ptr = malloc(len);
    error.len = len;
    memcpy((void *) error.ptr, p, len);
}

int main(void) {
    printf("+++++++++ Testing exported Principal Core Functions +++++++++\n");

    // Get Principal Management Canister
    CPrincipal *p = principal_management_canister();

    if (p->len == 0) {
        printf(" Test 1: Valid Principal Management.\n");
    } else {
        printf(" Test 1: Error.\n");
    }

    // Get Anonymous Principal
    principal_destroy(p);
    p = principal_anonymous();

    if (p->len == 1 && p->ptr[0] == 4) {
        printf(" Test 2: Valid Principal Anonymous.\n");
    } else {
        printf(" Test 2: Error.\n");
    }

    // Get SelfAuthenticating Principal
    unsigned char publicKey[] =
    {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00,
    };

    int publicKey_len = sizeof(publicKey) / sizeof(publicKey[0]);

    unsigned char bytesExpected[] =
    {
        0x2f, 0x8e, 0x47, 0x38, 0xf9, 0xd7, 0x68, 0x16, 0x82, 0x99, 0x85, 0x41, 0x52, 0x67,
        0x86, 0x38, 0x07, 0xd3, 0x7d, 0x20, 0x6a, 0xd9, 0x0f, 0xea, 0x72, 0xbf, 0x9d, 0xcf,
        0x02,
    };

    int bytesExpected_len = sizeof(bytesExpected) / sizeof(bytesExpected[0]);

    principal_destroy(p);
    p = principal_self_authenticating(publicKey, publicKey_len);

    int result = memcmp(bytesExpected, p->ptr, bytesExpected_len);

    if (p->len == bytesExpected_len && !result) {
        printf(" Test 3: Valid Principal Self Authenticating.\n");
    } else {
        printf(" Test 3: Error.\n");
    }

    // Get Principal from slice of bytes
    unsigned char slice[] = { 0x22 };

    principal_destroy(p);
    p = principal_from_slice(slice, 1);

    if (p->len == 1 && p->ptr[0] == 0x22) {
        printf(" Test 4.0: Valid Principal from bytes.\n");
    } else {
        printf(" Test 4.0: Error.\n");
    }

    // IF we send a byte array bigger than 29 an error is expected
    unsigned char bytes[] =
    {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
        0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00,
    };

    principal_destroy(p);
    p = principal_try_from_slice(bytes, 32, error_cb);
    if (error.ptr != NULL) {
        printf(" Test 4.1: Error is expected : %s\n", error.ptr);
    } else {
        printf(" Test 4.1: Text Failed an Error was expected\n");
    }

    // Get principal from Text , testing with anonymous principal
    const char *text = "2vxsx-fae";
    
    principal_destroy(p);
    p = principal_from_text(text, error_cb);
    
    if (p->len == 1 && p->ptr[0] == 4) {
        printf(" Test 5: Valid Principal from text.\n");
    } else {
        printf(" Test 5: Error.\n");
    }

    // Get Text from principal, testing with anonymous principal
    unsigned char principal[] = { 4 };

    principal_destroy(p);
    p = principal_to_text(principal, 1, error_cb);
    
    if (!strcmp(text, (const char *)p->ptr)) {
        printf(" Test 6: Valid Text from Principal.\n");
    } else {
        printf(" Test 6: Error.\n");
    }

    principal_destroy(p);
    free((void *) error.ptr);
    return 0;
}
