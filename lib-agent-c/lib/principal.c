#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "principal.h"
#include "bindings.h"
#include <string.h>

uint8_t *out;
int out_len;

RetPtr_u8 principal(const uint8_t *p, int len) {
    out = malloc(len);
    out_len=len;
    memcpy(out,p,len);
    return 0;
}

int getPrincipalManagementCanister(struct Principal* management) {
    
    principal_management_canister(*(RetPtr_u8)principal);
    if (out == NULL) {
        return -1;
    }
    management->ptr = out;
    management->len = out_len;

    return 0;
}

int getPrincipalAnonymous(struct Principal* management) {
    
    principal_anonymous(*(RetPtr_u8)principal);
    if (out == NULL) {
        return -1;
    }
    management->ptr = out;
    management->len = out_len;

    return 0;
}
