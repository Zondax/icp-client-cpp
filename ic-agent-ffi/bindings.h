#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * The anonymous identity.
 *
 * The caller will be represented as [`Principal::anonymous`], or `2vxsx-fae`.
 */
typedef struct AnonymousIdentity AnonymousIdentity;

/**
 * A Basic Identity which sign using an ED25519 key pair.
 */
typedef struct BasicIdentity BasicIdentity;

/**
 * A Request ID.
 */
typedef struct RequestId RequestId;

/**
 * A cryptographic identity based on the Secp256k1 elliptic curve.
 *
 * The caller will be represented via [`Principal::self_authenticating`], which contains the SHA-224 hash of the public key.
 */
typedef struct Secp256k1Identity Secp256k1Identity;

typedef struct PrincipalWrapper {
  uint8_t bytes[32];
} PrincipalWrapper;

/**
 * Principal wrapper
 */
typedef struct PrincipalResult {
  struct PrincipalWrapper *principal;
  char *error;
} PrincipalResult;

/**
 * AnonymousId wrapper structure
 */
typedef struct AnonymousIdentityWrapper {
  struct AnonymousIdentity *identity;
} AnonymousIdentityWrapper;

/**
 * BasicId wrapper structure
 */
typedef struct BasicIdentityWrapper {
  struct BasicIdentity *identity;
} BasicIdentityWrapper;

typedef struct BasicIdentityResult {
  struct BasicIdentityWrapper *identity;
  char *error;
} BasicIdentityResult;

/**
 * Secp256k1 wrapper structure
 */
typedef struct Secp256k1IdentityWrapper {
  struct Secp256k1Identity *identity;
} Secp256k1IdentityWrapper;

typedef struct Secp256k1IdentityResult {
  struct Secp256k1IdentityWrapper *identity;
  char *error;
} Secp256k1IdentityResult;

/**
 * Creates a new RequestId from a SHA-256 hash.
 */
struct RequestId *request_id_new(const uint8_t (*hash)[32]);

/**
 * Free created RequestId
 */
void request_id_free(struct RequestId *client);

/**
 * Returns the SHA-256 hash this ID is based on.
 */
const uint8_t *request_id_as_slice(struct RequestId *ptr);

/**
 * Construct a Principal of the IC management canister
 */
struct PrincipalWrapper *principal_management_canister(void);

/**
 * Construct a self-authenticating ID from public key
 */
struct PrincipalWrapper *principal_self_authenticating(const uint8_t *public_key,
                                                       int public_key_len);

/**
 * Construct an anonymous ID
 */
struct PrincipalWrapper *principal_anonymous(void);

/**
 * Construct a Principal from a slice of bytes.
 */
struct PrincipalWrapper *principal_from_slice(const uint8_t *bytes, int bytes_len);

/**
 * Construct a Principal from a slice of bytes.
 */
struct PrincipalResult principal_try_from_slice(const uint8_t *bytes, int bytes_len);

/**
 * Parse a Principal from text representation.
 */
struct PrincipalResult principal_from_text(const char *text);

/**
 * Return the textual representation of Principal.
 */
struct PrincipalResult principal_to_text(const uint8_t *bytes, int bytes_len);

void principal_free(struct PrincipalWrapper *wrapper);

void principal_result_free(struct PrincipalResult result);

/**
 * The anonymous identity.
 */
struct AnonymousIdentityWrapper *identity_anonymous(void);

/**
 * Create a BasicIdentity from reading a PEM Content
 */
struct BasicIdentityResult identity_basic_from_pem(const char *pem_data);

/**
 * Create a BasicIdentity from a KeyPair from the ring crate.
 * Creates an identity from a PEM certificate.
 */
struct Secp256k1IdentityResult identity_secp256k1_from_pem(const char *pem_data);

/**
 * Creates an identity from a private key.
 */
struct Secp256k1IdentityWrapper *identity_secp256k1_from_private_key(const char *private_key,
                                                                     uintptr_t pk_len);
