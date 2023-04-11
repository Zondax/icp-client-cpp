#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include <string.h>

#define PRINCIPAL_ANSWER_SIZE 29

uint8_t *out;
uint8_t *principal;
int principal_len;
uint8_t *error;
uint8_t *str;
int str_len;
int error_len;
RetPtr_u8 returnPtr(const uint8_t *p, int len) {
    out = malloc(len);
    memcpy(out,p,len);
     return 0;
}

RetPtr_u8 principalPtr(const uint8_t *p, int len) {
    principal = malloc(len);
    memcpy(principal,p,len);
    principal_len = len;
     return 0;
}

RetPtr_u8 errorPtr(const uint8_t *p, int len) {
    error = malloc(len);
    memcpy(error,p,len);
    error_len=len;
    return 0;
}

RetPtr_u8 strPtr(const uint8_t *p, int len) {
    str = malloc(len);
    memcpy(str,p,len);
    str_len = len;
     return 0;
}

struct FFIAgent {
    char* ptr;
};

int main(void) {
    FILE *fileptr;
    char *didContent;
    long filelen;

    fileptr = fopen("rust_hello_backend.did", "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);

    didContent = (char *)malloc(filelen * sizeof(char));
    fread(didContent, filelen, 1, fileptr);
    fclose(fileptr);

    const char *text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
    principal_from_text_wrap(text, *(RetPtr_u8)principalPtr,*(RetPtr_u8)errorPtr);

    const char *path = "http://127.0.0.1:4943\0";

    const void *identity = (const void*)0x4;
    const struct FFIAgent* agent_ptr = malloc(1000);
    agent_create_wrap(path, identity, Anonym, principal, principal_len, didContent, &agent_ptr,*(RetPtr_u8)errorPtr);

    const void **ret=malloc(30);
    ResultCode test;
    test=agent_update_wrap(agent_ptr, "greet\0", "(\"Zondax\")\0", ret,*(RetPtr_u8)errorPtr);

    idl_args_to_text_wrap(*ret,*(RetPtr_u8)strPtr);

    printf("ResultCode %d\n",test);
    if(test < 0) {
        for(int i=0; i<error_len;i++) {
            printf("%c",error[i]);
        }
    } 
    printf("\n");

    for(int i=0; i<str_len;i++) {
        printf("%c",str[i]);
    }
    printf("\n");

    free(ret);
    free(didContent);
    free(principal);
    free(out);
    free(str);
    free(error);
    return 0;
}
