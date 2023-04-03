#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "identity.h"
#include "bindings.h"
#include <string.h>


int getAnonymousIdentity(struct Identity *id) {
    identity_anonymous(id->identity);
    id->type = Anonym;
    return 0;
}
