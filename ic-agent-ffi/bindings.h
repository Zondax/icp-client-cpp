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
 * A callback used to give the unsized value to caller.
 */
typedef void (*PtrCallBack_u8)(const uint8_t*, int);

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
Principal *principal_management_canister(void);

/**
 * Construct a self-authenticating ID from public key
 */
void principal_self_authenticating(const uint8_t *public_key,
                                   int public_key_len,
                                   PtrCallBack_u8 principal_ret);

/**
 * Construct an anonymous ID
 */
void principal_anonymous(PtrCallBack_u8 principal_ret);

/**
 * Construct a Principal from a slice of bytes.
 */
void principal_from_slice(const uint8_t *bytes, int bytes_len, PtrCallBack_u8 principal_ret);

/**
 * Construct a Principal from a slice of bytes.
 */
ResultCode principal_try_from_slice(const uint8_t *bytes,
                                    int bytes_len,
                                    PtrCallBack_u8 ret_cb,
                                    PtrCallBack_u8 err_cb);

/**
 * Parse a Principal from text representation.
 */
ResultCode principal_from_text(const char *text, PtrCallBack_u8 ret_cb, PtrCallBack_u8 err_cb);

/**
 * Return the textual representation of Principal.
 */
ResultCode principal_to_text(Principal *ptr, PtrCallBack_u8 ret_cb, PtrCallBack_u8 err_cb);
