#pragma once

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

typedef struct FFIAgent FFIAgent;

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
 */
ResultCode identity_basic_from_key_pair(const uint8_t *public_key,
                                        const uint8_t *private_key_seed,
                                        const void **identity_ret,
                                        RetPtr_u8 error_ret);

/**
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

/**
 * Returns a sender, ie. the Principal ID that is used to sign a request.
 * Only one sender can be used per request.
 */
ResultCode identity_sender(const void **id_ptr,
                           enum IdentityType idType,
                           RetPtr_u8 principal_ret,
                           RetPtr_u8 error_ret);

/**
 * Sign a blob, the concatenation of the domain separator & request ID,
 * creating the sender signature.>
 */
ResultCode identity_sign(const uint8_t *bytes,
                         int bytes_len,
                         const void **id_ptr,
                         enum IdentityType idType,
                         RetPtr_u8 pubkey_ret,
                         RetPtr_u8 sig_ret,
                         RetPtr_u8 error_ret);

/**
 * Creates a FFIAgent instance to be used on the remaining agent functions
 */
ResultCode agent_create_wrap(const char *path,
                             const void *identity,
                             enum IdentityType id_type,
                             const uint8_t *canister_id_content,
                             int canister_id_len,
                             const char *did_content,
                             const struct FFIAgent **agent_ptr,
                             RetPtr_u8 error_ret);

/**
 * Calls and returns the information returned by the status endpoint of a replica.
 */
ResultCode agent_status_wrap(const struct FFIAgent *agent_ptr,
                             RetPtr_u8 status_ret,
                             RetPtr_u8 error_ret);

/**
 * Calls and returns a query call to the canister.
 */
ResultCode agent_query_wrap(const struct FFIAgent *agent_ptr,
                            const char *method,
                            const char *method_args,
                            const void **ret,
                            RetPtr_u8 error_ret);

/**
 * Calls and returns a update call to the canister.
 */
ResultCode agent_update_wrap(const struct FFIAgent *agent_ptr,
                             const char *method,
                             const char *method_args,
                             const void **ret,
                             RetPtr_u8 error_ret);

void idl_args_to_text(const void *idl_args, RetPtr_u8 ret_cb);
