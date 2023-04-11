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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/principal.h"
#include "lib/bindings.h"
#include "lib/helper.h"
#include "lib/identity.h"
#include "lib/agent.h"
#include "lib/idl.h"

struct Error error;

int main(void) {

    // Canister info from hello world deploy example
    const char *id_text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
    const char *did_file = "rust_hello_backend.did";
    const char *url = "http://127.0.0.1:4943\0";
    const char *method = "greet\0";
    const char *method_args = "(\"Zondax\")\0";
    
    // Get did file content
    char *did_content = get_did_file_content(did_file);

    // Compute principal id from text
    struct Principal canister_id;
    get_principal_from_text(id_text, &canister_id, &error);

    // Compute anonymous id
    struct Identity id;
    get_identity_anonymous(&id);

    // Create Agent
    const struct FFIAgent *agent_ptr = malloc(1000);
    agent_create(url, &id, &canister_id, did_content, &agent_ptr, &error);

    // Send Update call
    const void **ret=malloc(30);
    int result = agent_update(agent_ptr, method, method_args, ret, &error);

    // Translante idl result
    struct Text text;
    idl_args_to_text(*ret, &text);

    // Result Management
    printf("ResultCode %d\n",result);
    if(result < 0) {
        for(int i=0; i<error.len;i++) {
            printf("%c",error.ptr[i]);
        }
    } 

    for(int i=0; i<text.len;i++) {
        printf("%c",text.ptr[i]);
    }
    printf("\n");
    
    free(did_content);
    free(ret);
    free((void*)agent_ptr);
    return 0;
}
