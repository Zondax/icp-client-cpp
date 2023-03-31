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
 * A Request ID.
 */
typedef struct RequestId RequestId;

/**
 * Ptr creation with size and len
 */
typedef void (*RetPtr_u8)(const uint8_t*, int);

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
