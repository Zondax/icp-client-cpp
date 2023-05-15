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
    Hello ICP! 
    ("Hello, World!")

DISCLAIMER: On Milestone 1 the team was focused on studying and understanding
the full scope of ICP network and also achieving a functional hello world 
example to test the acquired knowledge and prove the concept and usability of a
C wrapper for the rust agent.

For Milestone 2 the team will iterate over library code and structure, to improve
library usability. Namely, code consistency, memory management improvement and
removing the need for global variables.
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bindings.h"
#include "helper.h"
#include "agent.h"

#define ANONYMOUS_ID_LEN 1

// Structures to save returns from function pointer and use throughout the code
Error error;
Principal principal;
Identity id;
Text text;

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len) {
    error.ptr = malloc(len);
    error.len = len;
    memcpy((void *) error.ptr, p, len);
}

static void principal_cb(const uint8_t *p, int len) {
    principal.ptr = malloc(len);
    principal.len = len;
    memcpy((void *) principal.ptr, p, len);
}

static void text_cb(const uint8_t *p, int len) {
    text.ptr = malloc(len);
    text.len = len;
    memcpy((void *) text.ptr, p, len);
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
    long file_size = get_did_file_size(did_file);
    char *did_content = malloc(file_size);
    get_did_file_content(did_file, file_size, did_content);

    // Compute principal id from text
    principal_from_text(id_text, principal_cb, error_cb);

    // Compute anonymous id
    id.ptr = malloc(ANONYMOUS_ID_LEN);
    identity_anonymous(id.ptr);
    id.type = Anonym;

    // here agent_ptr points to memory allocated by malloc(1000 bytes)
    // but we do not need to allocate memory in C for that.
    // I don't think the struct keyword is necessary, FFIAgent is defined as a fully type 
    // in bindings.h 
    const FFIAgent *agent_ptr = malloc(1000);
    // this would be enough:
    // const FFIAgent *agent_ptr;
    // Create Agent
    ResultCode result = agent_create(url, &id, &principal, did_content, &agent_ptr, error_cb);
    // after this call agent_ptr points to memory allocated by rust, so it does not longer 
    // points to malloc(1000)
    if (result < 0) {
        printf("%s\n", error.ptr);
        return Err;
    }

    // Send Update call
    const void **update_ret = malloc(30);
    result = agent_update(agent_ptr, method, method_args, update_ret, error_cb);
    if (result < 0) {
        printf("%s\n", error.ptr);
        return Err;
    }

    // Translate idl result
    idl_args_to_text(*update_ret, text_cb);
    printf("%s\n", text.ptr);

    // Free Memory
    free(did_content);
    free((void *) error.ptr);
    free((void *) principal.ptr);
    free(id.ptr);
    // as agent_ptr not longer points to malloc(1000), that memory is not being freed(memory leak)
    // also we are trying to use C allocator by invoking free over agent_ptr, but that memory 
    // was allocated by rust, which might be using a different allocator that C, so this could cause 
    // an undefined behavior. ALWAYS use the right allocator, use rust for memory allocated by it, or C otherwise.
    // free((void *) agent_ptr); 
    
    // better option, FFIAgent *ptr is optimized as 
    // Option<Box<FFIAgent>> by the compiler, and rust 
    // automatically deletes the agent object
    agent_destroy(agent_ptr);

    free(update_ret);
    return 0;
}
// in general good.
// Nice Rust project that provides C bindings:
// https://github.com/wasmerio/wasmer/tree/master/lib/c-api (Personally recommend this one!!! to follow design and rust-C idioms)
