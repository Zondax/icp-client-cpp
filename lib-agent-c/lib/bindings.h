#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum IdentityType {
  /**
   * anonym
   */
  Anonym = 0,
  /**
   * basic
   */
  Basic = 1,
  /**
   * secp256k1
   */
  Secp256k1 = 2,
} IdentityType;

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
 * Ptr creation with size and len
 */
typedef void (*RetPtr_u8)(const uint8_t*, int);

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
 * Dummy
 */
enum IdentityType identity_type(enum IdentityType id_type);

/**
 * The anonymous identity.
 */
void identity_anonymous(const void **identity_ret);

/**
 * Create a BasicIdentity from reading a PEM Content
 */
ResultCode identity_basic_from_pem(const char *pem_data,
                                   const void **identity_ret,
                                   RetPtr_u8 error_ret);

/**
 * Create a BasicIdentity from a KeyPair from the ring crate.
 * Creates an identity from a PEM certificate.
 */
ResultCode identity_secp256k1_from_pem(const char *pem_data,
                                       const void **identity_ret,
                                       RetPtr_u8 error_ret);

/**
 * Creates an identity from a private key.
 */
void identity_secp256k1_from_private_key(const char *private_key,
                                         uintptr_t pk_len,
                                         const void **identity_ret);
