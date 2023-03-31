#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Return for FFI functions
 */
enum ResultCode {
  /**
   * Ok
   */
  Ok = 0,
  /**
   * Error
   */
  Err = -1,
};
typedef int32_t ResultCode;

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
 * A cryptographic identity based on the Secp256k1 elliptic curve.
 *
 * The caller will be represented via [`Principal::self_authenticating`], which contains the SHA-224 hash of the public key.
 */
typedef struct Secp256k1Identity Secp256k1Identity;

/**
 * Ptr creation with size and len
 */
typedef void (*RetPtr_u8)(const uint8_t*, int);

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
void request_id_new(const uint8_t *bytes, int bytes_len, RetPtr_u8 request_id);

/**
 * Construct a Principal of the IC management canister
 */
void principal_management_canister(RetPtr_u8 principal);

/**
 * Construct a self-authenticating ID from public key
 */
void principal_self_authenticating(const uint8_t *public_key,
                                   int public_key_len,
                                   RetPtr_u8 principal);

/**
 * Construct an anonymous ID
 */
void principal_anonymous(RetPtr_u8 principal);

/**
 * Construct a Principal from a slice of bytes.
 */
void principal_from_slice(const uint8_t *bytes, int bytes_len, RetPtr_u8 principal);

/**
 * Construct a Principal from a slice of bytes.
 */
ResultCode principal_try_from_slice(const uint8_t *bytes,
                                    int bytes_len,
                                    RetPtr_u8 principal_ret,
                                    RetPtr_u8 error_ret);

/**
 * Parse a Principal from text representation.
 */
ResultCode principal_from_text(const char *text, RetPtr_u8 principal_ret, RetPtr_u8 error_ret);

/**
 * Return the textual representation of Principal.
 */
ResultCode principal_to_text(const uint8_t *bytes,
                             int bytes_len,
                             RetPtr_u8 principal_ret,
                             RetPtr_u8 error_ret);

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
