#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Identity {
    const void **identity;
    IdentityType type;
};

int getAnonymousIdentity(struct Identity *id);
