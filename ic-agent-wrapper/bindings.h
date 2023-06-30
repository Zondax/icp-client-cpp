#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C"{
#endif

typedef struct IDLValue IDLValue;
typedef struct IDLArgs IDLArgs;

/**
 * Enum for Identity Types
 */
typedef enum IdentityType {
  Anonym = 0,
  Basic = 1,
  Secp256k1 = 2,
} IdentityType;

/**
 * Structure that holds Rust u8 Vector
 */
typedef struct CBytes CBytes;

/**
 * Struture that holds Func IDLValue with CText String and a Principal id
 */
typedef struct CFunc CFunc;

/**
 * Struture that holds a vector of IDLValues
 */
typedef struct CIDLValuesVec CIDLValuesVec;

/**
 * Structure that holds the result of identity sign with pubkey and signature result
 */
typedef struct CIdentitySign CIdentitySign;

/**
 * Struture that holds a Record IDLValue with a Vector of keys (CText) and Vector of IDLValues
 */
typedef struct CRecord CRecord;

/**
 * Structure that holds Rust string
 */
typedef struct CText CText;

/**
 * Struture that holds Variant IDLValue with id:(Vec(u8)), IDLValue and u64 code
 */
typedef struct CVariant CVariant;

/**
 * Struture that holds the information related to a specific agent
 */
typedef struct FFIAgent FFIAgent;

typedef struct CPrincipal {
  uint8_t *ptr;
  uintptr_t len;
} CPrincipal;

/**
 * CallBack Ptr creation with size and len
 */
typedef void (*RetPtr_u8)(const uint8_t*, int, void*);

typedef struct RetError {
  void *user_data;
  RetPtr_u8 call;
} RetError;

/**
 * @brief Free allocated memory
 *
 * @param _ptr Pointer to IDLValue Array
 */
void idl_value_destroy(IDLValue *_ptr);

/**
 * @brief Free allocated CText
 *
 * @param ptr CText structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void ctext_destroy(struct CText *_ptr);

/**
 * @brief Get pointer to Ctext content
 *
 * @param ptr CText structure pointer
 * @return Pointer to the content allocated in rust
 */
const char *ctext_str(const struct CText *ptr);

/**
 * @brief Get Ctext content length
 *
 * @param bytes CText structure pointer
 * @return Ctext content length
 */
uintptr_t ctext_len(const struct CText *ptr);

/**
 * @brief Free allocated CBytes
 *
 * @param ptr CBytes structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void cbytes_destroy(struct CBytes *_ptr);

/**
 * @brief Get pointer to CBytes content
 *
 * @param ptr CBytes structure pointer
 * @return Pointer to the content allocated in rust
 */
const uint8_t *cbytes_ptr(const struct CBytes *ptr);

/**
 * @brief Get CBytes content length
 *
 * @param bytes CBytes structure pointer
 * @return CBytes content length
 */
uintptr_t cbytes_len(const struct CBytes *ptr);

/**
 * @brief Free allocated CIDLValuesVec
 *
 * @param ptr CIDLValuesVec structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void cidlval_vec_destroy(struct CIDLValuesVec *ptr);

/**
 * @brief Get pointer to IDLValue vector
 *
 * @param ptr CIDLValuesVec structure pointer
 * @return Pointer to the content allocated in rust
 */
const IDLValue *cidlval_vec(const struct CIDLValuesVec *ptr);

/**
 * @brief Get pointer to IDLValue at specific index
 *
 * @param ptr CIDLValuesVec structure pointer
 * @param index to specific index
 * @return Pointer to IDLValue
 */
const IDLValue *cidlval_vec_value(const struct CIDLValuesVec *ptr, uintptr_t index);

/**
 * @brief Get pointer to IDLValue at specific index
 *
 * @param ptr CIDLValuesVec structure pointer
 * @param index to specific index
 * @return Pointer to IDLValue
 * @note This gives ownership of the requested value to the caller, who is in charge
 * of free the memory.
 */
const IDLValue *cidlval_vec_value_take(struct CIDLValuesVec *ptr, uintptr_t index);

/**
 * @brief Get CIDLValuesVec length
 *
 * @param ptr CIDLValuesVec structure pointer
 * @return Number of IDLValues inside the vector
 */
uintptr_t cidlval_vec_len(const struct CIDLValuesVec *ptr);

/**
 * @brief Free allocated CRecord
 *
 * @param ptr CRecord structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void crecord_destroy(struct CRecord *_ptr);

/**
 * @brief Get pointer to CRecord keys vector
 *
 * @param ptr CRecord structure pointer
 * @return Pointer to the content of Keys vector
 */
const uint8_t *crecord_keys(const struct CRecord *ptr);

/**
 * @brief Get pointer to a CRecord Key at specific index
 *
 * @param ptr CRecord structure pointer
 * @param index to specific index
 * @return Pointer to CRecord Key
 *
 * @note Ownership is transfered to the caller
 */
struct CText *crecord_take_key(struct CRecord *ptr, uintptr_t index);

/**
 * @brief Get CRecord Key Vector length
 *
 * @param ptr CRecord structure pointer
 * @return Number of Keys inside the CRecord Keys vector
 */
uintptr_t crecord_keys_len(const struct CRecord *ptr);

/**
 * @brief Get pointer to CRecord Values vector
 *
 * @param ptr CRecord structure pointer
 * @return Pointer to the content of Values vector
 */
const IDLValue *crecord_vals(const struct CRecord *ptr);

/**
 * @brief Get pointer to a CRecord Value at specific index
 *
 * @param ptr CRecord structure pointer
 * @param index to specific index
 * @return Pointer to CRecord Value
 *
 * @note Ownership is trasfered to the caller
 */
IDLValue *crecord_take_val(struct CRecord *ptr, uintptr_t index);

/**
 * @brief Get CRecord Values Vector length
 *
 * @param ptr CRecord structure pointer
 * @return Number of Values inside the CRecord Values vector
 */
uintptr_t crecord_vals_len(const struct CRecord *ptr);

/**
 * @brief Free allocated CFunc
 *
 * @param ptr CFunc structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void cfunc_destroy(struct CFunc *_ptr);

/**
 * @brief Get String from Func IDL Value
 *
 * @param ptr CFunc structure pointer
 * @return Pointer to the String content of Func
 */
const char *cfunc_string(const struct CFunc *ptr);

/**
 * @brief Get String length from Func IDL Value
 *
 * @param ptr CFunc structure pointer
 * @return String length
 */
uintptr_t cfunc_string_len(const struct CFunc *ptr);

/**
 * @brief Get Principal from Func IDL Value
 *
 * @param ptr CFunc structure pointer
 * @return Pointer to the CPrincipal content of Func
 */
struct CPrincipal *cfunc_principal(const struct CFunc *ptr);

/**
 * @brief Free allocated CVariant
 *
 * @param ptr CVariant structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void cvariant_destroy(struct CVariant *_ptr);

/**
 * @brief Get Id from Variant IDL Value
 *
 * @param ptr CVariant structure pointer
 * @return Pointer to the ID content of Variant
 */
const uint8_t *cvariant_id(const struct CVariant *ptr);

/**
 * @brief Get ID length from Variant IDL Value
 *
 * @param ptr CVariant structure pointer
 * @return Id length
 */
uintptr_t cvariant_id_len(const struct CVariant *ptr);

/**
 * @brief Get IDLValue from Variant IDL Value
 *
 * @param ptr CVariant structure pointer
 * @return Pointer to the IDLValue content of Variant
 */
IDLValue *cvariant_idlvalue(const struct CVariant *ptr);

/**
 * @brief Get Code from Variant IDL Value
 *
 * @param ptr CVariant structure pointer
 * @return Code of Variant
 */
uint64_t cvariant_code(const struct CVariant *ptr);

/**
 * @brief Free allocated CIdentitySign
 *
 * @param ptr CIdentitySign structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void cidentitysign_destroy(struct CIdentitySign *_ptr);

/**
 * @brief Get Pubkey from CIdentitySign
 *
 * @param ptr CIdentitySign structure pointer
 * @return Pointer to the Public Key returned by the signing process
 */
const uint8_t *cidentitysign_pubkey(const struct CIdentitySign *ptr);

/**
 * @brief Get Pubkey Length
 *
 * @param ptr CIdentitySign structure pointer
 * @return Size of the returned Pubkey
 */
uintptr_t cidentitysign_pubkey_len(const struct CIdentitySign *ptr);

/**
 * @brief Get Signature from CIdentitySign
 *
 * @param ptr CIdentitySign structure pointer
 * @return Pointer to the Signature returned by the signing process
 */
const uint8_t *cidentitysign_sig(const struct CIdentitySign *ptr);

/**
 * @brief Get Signature Length
 *
 * @param ptr CIdentitySign structure pointer
 * @return Size of the returned Signature
 */
uintptr_t cidentitysign_sig_len(const struct CIdentitySign *ptr);

/**
 * @brief Creates a FFIAgent instance to be used on the remaining agent functions
 *
 * @param path Pointer to array of bytes
 * @param identity Pointer to identity
 * @param id_type Identity Type
 * @param canister_id Pointer to Principal Canister Id
 * @param canister_id_len Length of Principal ID
 * @param did_content Content of .did file
 * @param error_ret CallBack to get error
 * @return Pointer to FFIAgent structure
 * If the function returns a NULL pointer the user should check
 * The error callback, to attain the error
 */
struct FFIAgent *agent_create_wrap(const char *path,
                                   const void *identity,
                                   enum IdentityType id_type,
                                   const uint8_t *canister_id,
                                   int canister_id_len,
                                   const char *did_content,
                                   struct RetError *error_ret);

/**
 * @brief Calls and returns the information returned by the status endpoint of a replica
 *
 * @param agent_ptr Pointer to FFI structure that holds agent info
 * @param error_ret CallBack to get error
 * @return Pointer to FFIAgent structure
 * If the function returns a NULL CText the user should check
 * The error callback, to attain the error
 */
struct CText *agent_status_wrap(const struct FFIAgent *agent_ptr, struct RetError *error_ret);

/**
 * @brief Calls and returns a query call to the canister
 *
 * @param agent_ptr Pointer to FFI structure that holds agent info
 * @param method Pointer service/method name from did information
 * @param method_args Pointer to the arguments required by method
 * @param error_ret CallBack to get error
 * @return Pointer to IDLArgs
 */
IDLArgs *agent_query_wrap(const struct FFIAgent *agent_ptr,
                          const char *method,
                          const char *method_args,
                          struct RetError *error_ret);

/**
 * @brief Calls and returns a update call to the canister
 *
 * @param agent_ptr Pointer to FFI structure that holds agent info
 * @param method Pointer service/method name from did information
 * @param method_args Pointer to the arguments required by method
 * @param error_ret CallBack to get error
 * @return Pointer to IDLArgs
 * If the function returns a NULL CText the user should check
 * The error callback, to attain the error
 */
IDLArgs *agent_update_wrap(const struct FFIAgent *agent_ptr,
                           const char *method,
                           const char *method_args,
                           struct RetError *error_ret);

/**
 * @brief Free allocated Agent
 *
 * @param agent_ptr Pointer to FFI structure that holds agent info
 */
void agent_destroy(struct FFIAgent *_agent);

/**
 * @brief Creates and empty IDLArgs
 *
 * @return An IDLArgs object containing an empty list IDLValues
 */
IDLArgs *empty_idl_args(void);

/**
 * @brief Push a new IDLValue into values list
 * @param args The IDLArgs instance where `value` would be added.
 * @param value that is going to be pushed into
 * the list.
 * @note: This takes ownership of the passed value
 */
void idl_args_push_value(IDLArgs *args, IDLValue *value);

/**
 * @brief Translate IDLArgs to text
 *
 * @param idl_args Pointer to IdlArgs
 * @return pointer to a CText structure that can be used to access the result from rust
 * If the function returns a NULL IDLArgs the user should check
 * The error callback, to attain the error
 */
struct CText *idl_args_to_text(const IDLArgs *idl_args);

/**
 * @brief Get IDLArgs from text
 *
 * @param text Pointer to text content
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLArgs Structure
 * If the function returns a NULL IDLArgs the user should check
 * The error callback, to attain the error
 */
IDLArgs *idl_args_from_text(const char *text, struct RetError *error_ret);

/**
 * @brief Translate IDLArgs to bytes array
 *
 * @param idl_args Pointer to IdlArgs
 * @return pointer to a CBytes structure that can be used to access the result from rust
 * If the function returns a NULL IDLArgs the user should check
 * The error callback, to attain the error
 */
struct CBytes *idl_args_to_bytes(const IDLArgs *idl_args, struct RetError *error_ret);

/**
 * @brief Translate IDLArgs from a byte representation
 *
 * @param bytes Pointer to bytes content
 * @param bytes_len Length of byte array
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLArgs Structure
 * If the function returns a NULL IDLArgs the user should check
 * The error callback, to attain the error
 */
IDLArgs *idl_args_from_bytes(const uint8_t *bytes, int bytes_len, struct RetError *error_ret);

/**
 * @brief Create IDLArgs from an IDLValue Array(C)/Vector(Rust)
 *
 * @param elems Pointer to array of IDLValues, take in account rust will take
 * ownership of the memory where this array is stored and free it once it comes out of
 * the function scope. So the user should not use this array after calling
 * @param elems_len Number of IDLValues in the array
 * @return pointer to a IDLArgs structure
 */
IDLArgs *idl_args_from_vec(const IDLValue *const *elems, int elems_len);

/**
 * @brief Create IDLValues array from IDLArgs
 *
 * @param ptr Pointer to IDLArgs Array
 * @return Pointer to the Array of IDLValues , i.e. CIDLValuesVec struture
 */
struct CIDLValuesVec *idl_args_to_vec(const IDLArgs *ptr);

/**
 * @brief Number of elements in IDLArgs
 *
 * @param ptr Pointer to IDLArgs Array
 * @return Number of IDLValues in IDLArgs
 */
uintptr_t idl_args_len(const IDLArgs *ptr);

/**
 * @brief Free allocated memory
 *
 * @param _ptr Pointer to IDLArgs Array
 */
void idl_args_destroy(IDLArgs *_ptr);

/**
 * Format Text to IDLValue text format
 */
IDLValue *idl_value_format_text(const char *text, struct RetError *error_ret);

/**
 * @brief Test if IDLValues are equal
 *
 * @param idl_1 Pointer to IDLValue
 * @param idl_2 Pointer to IDLValue
 * @return Bool result true if IDLValues are equal
 */
bool idl_value_is_equal(const IDLValue *idl_1, const IDLValue *idl_2);

/**
 * @brief Create IDLValue with nat
 *
 * @param nat pointer to nat as string
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLValue Structure
 * If the function returns a NULL IDLValue the user should check
 * The error callback, to attain the error
 */
IDLValue *idl_value_with_nat(const char *nat, struct RetError *error_ret);

/**
 * @brief Create IDLValue with nat8
 *
 * @param val nat8 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_nat8(uint8_t val);

/**
 * @brief Get nat8 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value nat8 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool nat8_from_idl_value(const IDLValue *idl, uint8_t *value);

/**
 * @brief Create IDLValue with nat16
 *
 * @param val nat16 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_nat16(uint16_t val);

/**
 * @brief Get nat16 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value nat16 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool nat16_from_idl_value(const IDLValue *idl, uint16_t *value);

/**
 * @brief Create IDLValue with nat32
 *
 * @param val nat32 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_nat32(uint32_t val);

/**
 * @brief Get nat32 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value nat32 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool nat32_from_idl_value(const IDLValue *idl, uint32_t *value);

/**
 * @brief Create IDLValue with nat64
 *
 * @param val nat64 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_nat64(uint64_t val);

/**
 * @brief Get nat64 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value nat64 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool nat64_from_idl_value(const IDLValue *idl, uint64_t *value);

/**
 * @brief Create IDLValue with int8
 *
 * @param val int8 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_int8(int8_t val);

/**
 * @brief Get int8 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value int8 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool int8_from_idl_value(const IDLValue *idl, int8_t *value);

/**
 * @brief Create IDLValue with int16
 *
 * @param val int16 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_int16(int16_t val);

/**
 * @brief Get int16 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value int16 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool int16_from_idl_value(const IDLValue *idl, int16_t *value);

/**
 * @brief Create IDLValue with int32
 *
 * @param val int32 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_int32(int32_t val);

/**
 * @brief Get int32 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value int32 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool int32_from_idl_value(const IDLValue *idl, int32_t *value);

/**
 * @brief Create IDLValue with int64
 *
 * @param val int64 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_int64(int64_t val);

/**
 * @brief Get int64 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value int64 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool int64_from_idl_value(const IDLValue *idl, int64_t *value);

/**
 * @brief Create IDLValue with float32
 *
 * @param val float32 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_float32(float val);

/**
 * @brief Get float32 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value float32 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool float32_from_idl_value(const IDLValue *idl, float *value);

/**
 * @brief Create IDLValue with float64
 *
 * @param val float64 value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_float64(double val);

/**
 * @brief Get float64 from IDLValue
 *
 * @param idl IDLValue pointer
 * @param value float64 value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool float64_from_idl_value(const IDLValue *idl, double *value);

/**
 * @brief Create IDLValue with bool
 *
 * @param val bool value
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_bool(bool val);

/**
 * @brief Get float32 from bool
 *
 * @param idl IDLValue pointer
 * @param value bool value
 * @return boolean value that indicates if idlValue is in fact of this type
 * User can send value NULL if the objective is only to test the type of idl
 */
bool bool_from_idl_value(const IDLValue *idl, bool *value);

/**
 * @brief Create IDLValue with null
 *
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_null(void);

/**
 * @brief Get null from value
 *
 * @param idl IDLValue pointer
 * @return boolean value that indicates if idlValue is in fact of this type
 */
bool idl_value_is_null(const IDLValue *idl);

/**
 * @brief Create IDLValue with none
 *
 * @return Pointer to the IDLValue Structure
 */
IDLValue *idl_value_with_none(void);

/**
 * @brief Create IDLValue with text
 *
 * @param text Text pointer
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLValue Structure
 * If the function returns a NULL IDLValue the user should check
 * The error callback, to attain the error
 */
IDLValue *idl_value_with_text(const char *text, struct RetError *error_ret);

/**
 * @brief Get Text from IDLValue
 *
 * @param ptr Pointer to IDLValue
 * @return pointer to a CText structure that can be used to access the result from rust
 */
struct CText *text_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create IDLValue with Principal
 *
 * @param principal Pointer to Principal bytes
 * @param principal_len Length Principal array of bytes
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLValue Structure
 * If the function returns a NULL IDLValue the user should check
 * The error callback, to attain the error
 */
IDLValue *idl_value_with_principal(const uint8_t *principal,
                                   int principal_len,
                                   struct RetError *error_ret);

/**
 * @brief Principal from IDLValue
 *
 * @param ptr IDLValue structure pointer
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *principal_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create IDLValue with Service
 *
 * @param principal Pointer to Principal bytes
 * @param principal_len Length Principal array of bytes
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLValue Structure
 * If the function returns a NULL IDLValue the user should check
 * The error callback, to attain the error
 */
IDLValue *idl_value_with_service(const uint8_t *principal,
                                 int principal_len,
                                 struct RetError *error_ret);

/**
 * @brief Service from IDLValue
 *
 * @param ptr IDLValue structure pointer
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *service_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create IDLValue with Service
 *
 * @param number Pointer number represented as a string
 * @param error_ret CallBack to get error
 * @return Pointer to the IDLValue Structure
 * If the function returns a NULL IDLValue the user should check
 * The error callback, to attain the error
 */
IDLValue *idl_value_with_number(const char *number, struct RetError *error_ret);

/**
 * @brief Get Number from IDLValue
 *
 * @param ptr IDLValue structure pointer
 * @return Pointer to CText Structure where number string is saved
 */
struct CText *number_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create Opt IDLValue
 *
 * @param number Pointer to IDLValue (ownership is taken)
 * @return Pointer to the Opt IDLValue Structure
 */
IDLValue *idl_value_with_opt(IDLValue *value);

/**
 * @brief Get IDLValue from Opt
 *
 * @param ptr Pointer to Opt IDLValue
 * @return Pointer to IDLValue Structure
 */
IDLValue *opt_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create Reserved IDLValue
 *
 * @return Pointer to the Reserved IDLValue Structure
 */
IDLValue *idl_value_with_reserved(void);

/**
 * @brief Create IDLValue Vec from an IDLValue Array(C)/Vector(Rust)
 *
 * @param elems Pointer to array of IDLValues, take in account rust will take
 * ownership of the memory where this array is stored and free it once it comes out of
 * the function scope. So the user should not use this array after calling
 * @param elems_len Number of IDLValues on the array
 * @return Pointer to IDLValue Structure
 */
IDLValue *idl_value_with_vec(const IDLValue *const *elems, int elems_len);

/**
 * @brief Get IDLValue Array(C)/Vector(Rust) from Vec IDLValue
 *
 * @param ptr Pointer to IDLValue structure
 * @return Pointer to Array of IDLValues , CIDLValuesVec
 */
struct CIDLValuesVec *vec_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create IDLValue with array of keys and values where each pair represents an IDLField
 *
 * @param keys Pointer to array of keys
 * @param keys_len Number of Keys
 * @param vals Pointer to array of IDLValues
 * @param vals_len Number of Values
 * @param keys_are_ids If set to true, keys are expected to be [u8; 4] and will
 * be read as LE u32 and the resulting record will have unnamed/id labels based
 * on the value of the keys
 *
 * Take in account rust will take
 * ownership of the memory where this array is stored and free it once it comes out of
 * the function scope.
 * So the user should not use this array after calling this function
 *
 * @return Pointer to IDLValue Structure
 */
IDLValue *idl_value_with_record(const char *const *keys,
                                int keys_len,
                                const IDLValue *const *vals,
                                int vals_len,
                                bool keys_are_ids);

/**
 * @brief Get Record from IDLValue
 *
 * @param ptr Pointer to IDLValue Structure
 * @return Pointer to CRecord structure where user can access array of keys and IDLValues
 */
struct CRecord *record_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create Variant IDLValue with key, IDValue and code
 *
 * @param key Pointer to key
 * @param val Pointer to IDLValues, take in account rust will take
 * ownership of the memory where this value is stored and free it once it comes out of
 * the function scope. So the user should not use this array after calling
 * @param code Variant code
 * @return Pointer to IDLValue Structure
 */
IDLValue *idl_value_with_variant(const char *key, const IDLValue *val, uint64_t code);

/**
 * @brief Get Variant from IDLValue
 *
 * @param ptr Pointer to IDLValue Structure
 * @return Pointer to Variant structure where user can access key, value and code
 */
struct CVariant *variant_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create Func IDLValue with Principal and Function Name
 *
 * @param bytes Principal array of bytes
 * @param bytes_len Length of Principal
 * @param func_name Function name string
 * @return Pointer to IDLValue Structure
 */
IDLValue *idl_value_with_func(const uint8_t *bytes, int bytes_len, const char *func_name);

/**
 * @brief Get Func from IDLValue
 *
 * @param ptr Pointer to IDLValue Structure
 * @return Pointer to CFunc structure where user can Principal and Function Name
 */
struct CFunc *func_from_idl_value(const IDLValue *ptr);

/**
 * @brief Create Anonymous Identity
 *
 * @return Void pointer to Anonymous Identity
 * This function can be used by the user but its intent to be used in a C
 * friendly function that returns a CIdentity Structure
 */
void *identity_anonymous(void);

/**
 * @brief Create a BasicIdentity from reading a PEM Content
 *
 * @param pem_data Pointer to Pem file data
 * @param error_ret CallBack to get error
 * @return Void pointer to Basic Identity
 * This function can be used by the user but its intent to be used in a C
 * friendly function that returns a CIdentity Structure
 * If the function returns a NULL pointer the user should check
 * The error callback, to attain the error
 */
void *identity_basic_from_pem(const char *pem_data, struct RetError *error_ret);

/**
 * @brief Create a BasicIdentity from a KeyPair from the ring crate
 *
 * @param public_key Pointer to public key
 * @param private_key_seed Pointer to a private key seed
 * @param error_ret CallBack to get error
 * @return Void pointer to Basic Identity
 * This function can be used by the user but its intent to be used in a C
 * friendly function that returns a CIdentity Structure
 * If the function returns a NULL pointer the user should check
 * The error callback, to attain the error
 */
void *identity_basic_from_key_pair(const uint8_t *public_key,
                                   const uint8_t *private_key_seed,
                                   struct RetError *error_ret);

/**
 * @brief Creates an Secp256k1 identity from a PEM file
 *
 * @param pem_data Pointer to Pem file data
 * @param error_ret CallBack to get error
 * @return Void pointer to Secp256k1 Identity
 * This function can be used by the user but its intent to be used in a C
 * friendly function that returns a CIdentity Structure
 * If the function returns a NULL pointer the user should check
 * The error callback, to attain the error
 */
void *identity_secp256k1_from_pem(const char *pem_data, struct RetError *error_ret);

/**
 * @brief Create a Secp256k1 from a KeyPair from the ring crate
 *
 * @param private_key Pointer to a private key
 * @param pk_len Private key length
 * @return Void pointer to Secp256k1 Identity
 * This function can be used by the user but its intent to be used in a C
 * friendly function that returns a CIdentity Structure
 */
void *identity_secp256k1_from_private_key(const char *private_key, uintptr_t pk_len);

/**
 * @brief Returns a sender, ie. the Principal ID that is used to sign a request.
 * Only one sender can be used per request.
 *
 * @param id_ptr Pointer to identity. This function does not take ownership
 * of the passed identity.
 * @param idType Identity Type
 * @param error_ret CallBack to get error
 * @return Void pointer to CPrincipal structure
 */
struct CPrincipal *identity_sender(void *id_ptr,
                                   enum IdentityType idType,
                                   struct RetError *error_ret);

/**
 * @brief Sign a blob, the concatenation of the domain separator & request ID,
 * creating the sender signature
 *
 * @param bytes Pointer to blob content
 * @param bytes_len Length of blob
 * @param id_ptr Pointer to identity. This function does not take ownership of the passed
 * identity.
 * @param idType Identity Type
 * @param error_ret CallBack to get error
 * @return Pointer to the signature
 */
struct CIdentitySign *identity_sign(const uint8_t *bytes,
                                    int bytes_len,
                                    const void *id_ptr,
                                    enum IdentityType idType,
                                    struct RetError *error_ret);

/**
 * @brief Free allocated Memory
 *
 * @param identity Identity pointer
 * @param idType Identity Type
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void identity_destroy(void *identity, enum IdentityType idType);

/**
 * @brief Construct a Principal of the IC management canister
 *
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *principal_management_canister(void);

/**
 * @brief Construct a self-authenticating ID from public key
 *
 * @param public_key Pointer to array that holds public key
 * @param public_key_len Length of public_key
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *principal_self_authenticating(const uint8_t *public_key, int public_key_len);

/**
 * @brief Construct an anonymous ID
 *
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *principal_anonymous(void);

/**
 * @brief Construct a Principal from a array of bytes.
 *
 * @param bytes Pointer to array of bytes
 * @param bytes_len Length of array of bytes
 * @return Pointer to CPrincipal structure
 */
struct CPrincipal *principal_from_slice(const uint8_t *bytes, int bytes_len);

/**
 * @brief Try to Construct a Principal from a array of bytes.
 *
 * @param bytes Pointer to array of bytes
 * @param bytes_len Length of array of bytes
 * @param error_ret CallBack to get error
 * @return Pointer to CPrincipal structure
 * If the function returns a NULL CPrincipal the user should check
 * The error callback, to attain the error
 */
struct CPrincipal *principal_try_from_slice(const uint8_t *bytes,
                                            int bytes_len,
                                            struct RetError *error_ret);

/**
 * @brief Construct a Principal from text representation.
 *
 * @param text Pointer to text representation
 * @param error_ret CallBack to get error
 * @return Pointer to CPrincipal structure
 * If the function returns a NULL CPrincipal the user should check
 * The error callback, to attain the error
 */
struct CPrincipal *principal_from_text(const char *text, struct RetError *error_ret);

/**
 * @brief Return the textual representation of Principal.
 *
 * @param bytes Principal in bytes
 * @param bytes_len Length of array of bytes
 * @param error_ret CallBack to get error
 * @return Pointer to CPrincipal structure will hold the text
 * If the function returns a NULL CPrincipal the user should check
 * The error callback, to attain the error
 */
struct CPrincipal *principal_to_text(const uint8_t *bytes,
                                     int bytes_len,
                                     struct RetError *error_ret);

/**
 * @brief Free allocated Memory
 *
 * @param bytes CPrincipal structure pointer
 * Rust code will deal the memory allocation but the user should guarantee
 * The memory is free when isn't needed anymore
 */
void principal_destroy(struct CPrincipal *ptr);

/**
 * @brief Creates a new RequestId from a SHA-256 hash.
 *
 * @param bytes Pointer to hash content
 * @param bytes_len Length of hash
 * @return Void pointer to CText structure
 */
struct CText *request_id_new(const uint8_t *bytes, int bytes_len);

#ifdef __cplusplus
}
#endif
