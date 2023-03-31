#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "principal.h"

int main(void) {
    struct Principal principal;
    getPrincipalAnonymous(&principal);
    printf("anonym tag: %x\n", principal.ptr[0]);
    printf("anonym len: %d\n", principal.len);
    return 0;
}
