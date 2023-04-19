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

#define ANONYMOUS_ID_LEN 1

// Structures to save returns from function pointer and use throughout the code
struct Error error;
struct Principal principal;
struct Identity id;
struct Text text;

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len) {
    error.ptr = malloc(len);
    error.len=len;
    memcpy((void *)error.ptr, p, len);
}

static void principal_cb(const uint8_t *p, int len) {
    principal.ptr = malloc(len);
    principal.len=len;
    memcpy((void *)principal.ptr, p, len);
}

static void text_cb(const uint8_t *p, int len) {
    text.ptr = malloc(len);
    text.len=len;
    memcpy((void *)text.ptr, p, len);
}

int main(void) {

    printf("Hello ICP! \n");

    // Canister info from hello world deploy example
    const char *id_text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
    const char *did_file = "./examples/hello-icp/rust_hello_backend.did";
    const char *url = "http://127.0.0.1:4943";
    const char *method = "greet";
    const char *method_args = "(\"World\")";

    // Get did file content
    char *did_content = get_did_file_content(did_file);

    // Compute principal id from text
    principal_from_text(id_text, principal_cb, error_cb);

    // Compute anonymous id
    id.ptr = malloc(ANONYMOUS_ID_LEN);
    identity_anonymous(id.ptr);
    id.type = Anonym;

    // Create Agent
    const struct FFIAgent *agent_ptr = malloc(1000);
    ResultCode result = agent_create(url, &id, &principal, did_content, &agent_ptr, error_cb);
    if(result < 0) {
        printf("%s\n",error.ptr);
        return Err;
    }

    // Send Update call
    const void **update_ret=malloc(30);
    result = agent_update(agent_ptr, method, method_args, update_ret, error_cb);
    if(result < 0) {
        printf("%s\n",error.ptr);
        return Err;
    }

    // Translante idl result
    idl_args_to_text(*update_ret, text_cb);

    // Result Management
    if(result < 0) {
        printf("%s\n",error.ptr);
        return Err;
    }
    printf("%s\n",text.ptr);
    
    // Free Memory
    free(did_content);
    free((void *)error.ptr);
    free((void *)principal.ptr);
    free(id.ptr);
    free((void *)agent_ptr);
    free(update_ret);
    return 0;
}
