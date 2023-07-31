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
/*******************************************************************************
* Running Hello world example
1. Deploy local Hello world canister
    1.1 In a separate directory create new hello world canister:
        > dfx new --type=rust rust_hello
    1.2 Change to your project directory:
        > cd rust_hello
    1.3 Start the local execution environment:
        > dfx start --background
    1.4 Register, build, and deploy the canister:
        > dfx deploy

2. Use IC-C agent to interact with local canister
    2.1 Inside IC-C folder configure Project and generate makefile:
        > cmake .
    2.2Compile and link project:
        > cmake --build .
    2.3 Run hello_world example:
        > ./hello_icp
The example sends the text "World" to the available canister service "greet",
the response is represented inside the () :
    ("Hello, World!")
    ("Hello, zondax!")
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

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len, void *) {
    if (error.ptr != NULL) {
        free((void *)error.ptr);
    }
    error.ptr = malloc(len);
    error.len = len;
    memcpy((void *) error.ptr, p, len);
}

int main(void) {
    RetError ret_error;
    ret_error.call = error_cb;

    // Canister info from hello world deploy example
    const char *id_text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
    const char *did_file = "../examples/hello-icp/rust_hello_backend.did";
    const char *url = "http://127.0.0.1:4943";
    const char *method = "greet";

    // Get did file content
    long file_size = get_did_file_size(did_file);
    char *did_content = malloc(file_size);
    get_did_file_content(did_file, file_size, did_content);

    // Compute principal id from text
    CPrincipal *principal = principal_from_text(id_text,&ret_error);
    CHECK_ERROR(error);

    //compute id
    CIdentity id = {0};
    anonymous_identity(&id);

    // Create an IDLArg argument from a IDLValue
    const char * arg1 = "world";
    IDLValue *element_1 = idl_value_with_text(arg1, &ret_error);
    CHECK_ERROR(error);
    const IDLValue* elems[] = {element_1};
    IDLArgs *idl_args_ptr_1 = idl_args_from_vec(elems, 1);

    // Create Agent 1
    FFIAgent *agent_1 = agent_create(url, &id, principal, did_content, &ret_error);
    CHECK_ERROR(error);

    // Send query call to agent 1
    IDLArgs *call_1 = agent_query(agent_1, method, idl_args_ptr_1, &ret_error);
    CHECK_ERROR(error);

    //Translate IdlArg to text
    CText *text_1 = idl_args_to_text(call_1);

    // Create an IDLArg argument from a IDLValue
    const char * arg2 = "zondax";
    IDLValue *element_2 = idl_value_with_text(arg2, &ret_error);
    CHECK_ERROR(error);
    const IDLValue* elems2[] = {element_2};
    IDLArgs* idl_args_ptr_2 = idl_args_from_vec(elems2, 1);

    // Create Agent 2
    FFIAgent *agent_2 = agent_create(url, &id, principal, did_content, &ret_error);
    CHECK_ERROR(error);

    // Send query call to agent 2
    IDLArgs *call_2 = agent_query(agent_2, method, idl_args_ptr_2, &ret_error);
    CHECK_ERROR(error);

    //Translate IdlArg to text
    CText *text_2 = idl_args_to_text(call_2);

    // Print Results
    printf("%s\n", ctext_str(text_1));
    printf("%s\n", ctext_str(text_2));

    // Free Memory
    free(did_content);
    free((void *) error.ptr);
    principal_destroy(principal);
    agent_destroy(agent_1);
    agent_destroy(agent_2);
    idl_args_destroy(idl_args_ptr_1);
    idl_args_destroy(idl_args_ptr_2);
    idl_args_destroy(call_2);
    idl_args_destroy(call_1);
    ctext_destroy(text_1);
    ctext_destroy(text_2);

    return 0;
}
