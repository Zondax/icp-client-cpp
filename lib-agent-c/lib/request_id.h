#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct RequestId {
    const uint8_t* ptr;
    int len;
};

int getRequestedId(const uint8_t *hash, int hash_len, struct RequestId* id);
