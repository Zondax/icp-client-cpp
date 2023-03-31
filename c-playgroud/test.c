#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include <string.h>

#define PRINCIPAL_ANSWER_SIZE 29
uint8_t *out;
RetPtr_u8 principal(const uint8_t *p, int len) {
    printf("len:%d\n", len);
    out = malloc(len);
    memcpy(out,p,len);
}

int main(void) {
    principal_anonymous(principal);

    printf("anonym: %d\n", out[0]);

    principal_management_canister(principal);

    printf("manage: %d\n", out[0]);

    uint8_t pk[32] = {
        0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 
        0xaa, 0x11, 0xaa, 0x11, 0xaa, 0xaa, 0x11, 0xaa, 0x11, 
        0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 
        0x11, 0x11, 0xaa, 0x11, 0xaa};

    principal_self_authenticating(pk,32,principal);

    for (int i = 0; i< PRINCIPAL_ANSWER_SIZE; i++) {
        printf("0x%x, ", out[i]);
    }

    return 0;
}
