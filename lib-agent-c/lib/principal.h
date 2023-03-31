#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Principal {
    const uint8_t* ptr;
    int len;
};

int getPrincipalManagementCanister(struct Principal* management);
int getPrincipalAnonymous(struct Principal* management);
