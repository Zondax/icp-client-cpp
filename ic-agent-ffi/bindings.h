#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
typedef void (*UnsizedCallBack_u8)(const uint8_t*, int);

struct RequestId *request_id_new(const uint8_t (*hash)[32]);

void request_id_free(struct RequestId *client);

const uint8_t *request_id_as_slice(struct RequestId *ptr);

Principal *principal_management_canister(void);

Principal *principal_self_authenticating(const uint8_t *public_key, int public_key_len);

Principal *principal_anonymous(void);

Principal *principal_from_slice(const uint8_t *bytes, int bytes_len);

ResultCode principal_try_from_bytes(const uint8_t *bytes,
                                    int bytes_len,
                                    UnsizedCallBack_u8 ret_cb,
                                    UnsizedCallBack_u8 err_cb);

ResultCode principal_from_text(const char *text,
                               UnsizedCallBack_u8 ret_cb,
                               UnsizedCallBack_u8 err_cb);
