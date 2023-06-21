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

#define CHECK_ERROR(e)              \
        if (e.len > 0) {            \
            printf("%s\n", e.ptr);   \
            return ERR;              \
        }                           \

Error error;
CIdentity id_anonym;
CIdentity id_secp26k1;

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
    // Canister info from hello world deploy example
    const char *id_text = "rdmx6-jaaaa-aaaaa-aaadq-cai";
    const char *did_file = "../examples/icp-app/rdmx6-jaaaa-aaaaa-aaadq-cai.did";
    const char *url = "https://ic0.app";

    // Get did file content
    long file_size = get_did_file_size(did_file);
    char *did_content = malloc(file_size);
    get_did_file_content(did_file, file_size, did_content);

    // Compute principal id from text
    CPrincipal *principal = principal_from_text(id_text,error_cb);
    CHECK_ERROR(error);

    //compute id
    CIdentity id = {0};
    anonymous_identity(&id);

    // Create Agent 1
    FFIAgent *agent_1 = agent_create(url, &id_anonym, principal, did_content, error_cb);
    CHECK_ERROR(error);
    
    // Create IDL argument
    IDLValue* element_1 = idl_value_with_nat64(1974211);
    const IDLValue* elems[] = {element_1};
    IDLArgs* idl_args_ptr = idl_args_from_vec(elems, 1);

    // IDL Args
    IDLArgs *call_1 = agent_query(agent_1, "lookup", idl_args_ptr,error_cb);
    CHECK_ERROR(error);

    // Print arg in text
    CText *text = idl_args_to_text(call_1);
    printf("%s\n", ctext_str(text));

    // Take IDLValues Vec
    CIDLValuesVec *vec = idl_args_to_vec(call_1);

    // Get First idlvalue from idlargs and make it a vec of idlvalue
    const IDLValue *val = cidlval_vec_value(vec, 0);
    CIDLValuesVec *vec2 = vec_from_idl_value(val);

    // Get First element of vector
    const IDLValue *val2 = cidlval_vec_value(vec2, 0);

    // Get record and print first key element
    CRecord *rec = record_from_idl_value(val2);
    printf("%s\n",crecord_get_key(rec,0));

    // Free Memory
    free(did_content);
    free((void *) error.ptr);
    principal_destroy(principal);
    agent_destroy(agent_1);
    idl_args_destroy(idl_args_ptr);
    idl_args_destroy(call_1);
    idl_value_destroy((IDLValue *)val);
    idl_value_destroy((IDLValue *)val2);
    ctext_destroy(text);
    cidlval_vec_destroy(vec);
    cidlval_vec_destroy(vec2);
    crecord_destroy(rec);
}
