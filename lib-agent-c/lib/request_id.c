#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "request_id.h"
#include "bindings.h"
#include <string.h>

uint8_t *out;
int out_len;

RetPtr_u8 request_id(const uint8_t *p, int len) {
    out = malloc(len);
    out_len=len;
    memcpy(out,p,len);
    return 0;
}

int getRequestedId(const uint8_t *hash, int hash_len, struct RequestId* id) {
    
    request_id_new(hash, hash_len, *(RetPtr_u8)request_id);
    if (out == NULL) {
        return -1;
    }
    id->ptr = out;
    id->len = out_len;

    return 0;
}
