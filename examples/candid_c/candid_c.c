/*******************************************************************************
*   (c) 2018 - 2023 Zondax AG
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zondax_ic.h"
#include "helper_c.h"
#include "agent_c.h"

Error error;

// Function pointers used to get the return from rust lib
static void error_cb(const uint8_t *p, int len, void *) {

    if (error.ptr != NULL) {
        free((void *)error.ptr);
        error.ptr = NULL;
    }

    error.ptr = malloc(len);
    error.len = len;

    memcpy((void *) error.ptr, p, len);
}

int main(void) {
    printf("+++++++++ Testing exported Candid Args Core Functions +++++++++\n");
    
    RetError ret_error;
    ret_error.call = error_cb;

    // Anonymous Principal
    const uint8_t p[] = {4};
    
    //Diferent representations for same IDLArgs
    const char *text_args = "(true, principal \"2vxsx-fae\", -12 : int32)";
    const uint8_t bytes_args[] = {68, 73, 68, 76, 0, 3, 126, 104, 117, 1, 1, 1, 4, 244, 255, 255, 255};

    // Create a simple array of IdlValues 
    IDLValue *element_1 = idl_value_with_bool(true);
    IDLValue *element_2 = idl_value_with_principal(p, 1, &ret_error);
    IDLValue *element_3 = idl_value_with_int32(-12);
    const IDLValue *elems[] = {element_1, element_2, element_3};
    
    // Create IDLArgs with a array of IDLValues
    IDLArgs *args = idl_args_from_vec(elems, 3);
    
    // Translate IDLArgs to Text an test string
    CText *text = idl_args_to_text(args);
    if (!strcmp(text_args, (const char *)ctext_str(text))) {
        printf(" Test 1: Text from IDLArgs is Valid\n");
    } else {
        printf(" Test 1: Error.\n");
    }
    // Reconsturct args from previous text with the Ctext specific functions
    IDLArgs *args_new = idl_args_from_text(ctext_str(text), &ret_error);

    // Test Expected Bytes using reconstructed args it is expected that it matched
    // the original used args
    CBytes *bytes = idl_args_to_bytes(args_new, &ret_error);
    if (!memcmp(cbytes_ptr(bytes), bytes_args, 17)) {
        printf(" Test 2: Bytes from IDLArgs are Valid\n");
    } else {
        printf(" Test 2: Error.\n");
    }

    // Get Vector of IDLValues from IDLArgs and access first value
    CIDLValuesVec *val_vec = idl_args_to_vec(args_new);

    // Get length of IDLValue vec
    int vec_len = cidlval_vec_len(val_vec);

    // Get first element of vec 
    const IDLValue *vec0 = cidlval_vec_value(val_vec, 0);
    if (vec_len == 3 && bool_from_idl_value(vec0, NULL)) {
        printf(" Test 3: First Element of IDLValue Array is Valid\n");
    } else {
        printf(" Test 3: Error.\n");
    }

    cidlval_vec_destroy(val_vec);
    cbytes_destroy(bytes);
    ctext_destroy(text);
    idl_args_destroy(args);
    idl_args_destroy(args_new);

    printf("+++++++++ Testing exported Candid Values Core Functions +++++++++\n");

    //Create IDLValue with Nat8 and extract the nat8 from it
    IDLValue *val = idl_value_with_nat8(5);
    uint8_t nat = 0;

    bool is_valid = nat8_from_idl_value(val, &nat);

    if ( is_valid && nat == 5) {
        printf(" Test 1: Valid Nat8\n");
    } else {
        printf(" Test 1: Error.\n");
    }

    // It is also possible to test only if a number is the expected type by sending
    // a null pointer
    is_valid = nat8_from_idl_value(val, NULL);

    // Create IDLValue with Int8 and extract the Int8 from it
    idl_value_destroy(val);
    val = idl_value_with_int8(-5);
    int8_t i = 0;

    is_valid = int8_from_idl_value(val, &i);

    if ( is_valid && i == -5) {
        printf(" Test 2: Valid Int8\n");
    } else {
        printf(" Test 2: Error.\n");
    }

    // Create IDLValue with Float32 and extarct the Float32 from it
    idl_value_destroy(val);
    val = idl_value_with_float32(1.2);

    float f = 0;
    is_valid = float32_from_idl_value(val, &f);
    if ( is_valid && f == 1.2f) {
        printf(" Test 3: Valid float32\n");
    } else {
        printf(" Test 3: Error.\n");
    }

    // Create IDLValue with bool and extarct the bool from it
    idl_value_destroy(val);
    val = idl_value_with_bool(true);

    bool b = false;
    is_valid = bool_from_idl_value(val, &b);
    if ( is_valid && b == true) {
        printf(" Test 4: Valid bool\n");
    } else {
        printf(" Test 4: Error.\n");
    }

    // Create IDLValue with number and extarct the number from it
    idl_value_destroy(val);
    val = idl_value_with_number("123", &ret_error);

    CText *t = number_from_idl_value(val);
    // ctext_len does not include the /0
    if ( ctext_len(t) == 3 && !strcmp("123", ctext_str(t))) {
        printf(" Test 5: Valid number\n");
    } else {
        printf(" Test 5: Error.\n");
    }

    // Create IDLValue with text and extarct the text from it
    idl_value_destroy(val);
    val = idl_value_with_text("zondax", &ret_error);

    ctext_destroy(t);
    t = text_from_idl_value(val);
    if ( ctext_len(t) == 6 && !strcmp("zondax", ctext_str(t))) {
        printf(" Test 6: Valid Text\n");
    } else {
        printf(" Test 6: Error.\n");
    }

    // Create IDLValue with principal and extarct the principal from it
    idl_value_destroy(val);
    val = idl_value_with_principal(p, 1, &ret_error);

    CPrincipal *id = principal_from_idl_value(val);
    if ( id->len == 1 && id->ptr[0] == 4) {
        printf(" Test 7: Valid Principal\n");
    } else {
        printf(" Test 7: Error.\n");
    }

    // Create IDLValue with service and extarct the service from it
    idl_value_destroy(val);
    val = idl_value_with_service(p, 1, &ret_error);

    principal_destroy(id);
    id = service_from_idl_value(val);
    if ( id->len == 1 && id->ptr[0] == 4) {
        printf(" Test 8: Valid Service\n");
    } else {
        printf(" Test 8: Error.\n");
    }

    // Create IDLValue with variant and extarct the variant from it
    idl_value_destroy(val);
    val = idl_value_with_variant("title1",idl_value_with_nat8(2),33);

    CVariant *var = variant_from_idl_value(val);
    uint64_t code = cvariant_code(var);
    const uint8_t *key = cvariant_id(var);
    const IDLValue *var_val = cvariant_idlvalue(var);
    is_valid = nat8_from_idl_value(var_val, &nat);
    if ( code == 33 && !strcmp("title1", (const char *)key) && nat == 2) {
        printf(" Test 9: Valid Variant\n");
    } else {
        printf(" Test 9: Error.\n");
    }

    // Create IDLValue with func and extract the func form it 
    idl_value_destroy(val);
    val = idl_value_with_func(p,1,"zondax");

    CFunc *func = func_from_idl_value(val);

    principal_destroy(id);
    id = cfunc_principal(func);
    const char* func_name = cfunc_string(func);
    if ( id->len == 1 && id->ptr[0] == 4 && !strcmp("zondax", func_name)) {
        printf(" Test 10: Valid Func\n");
    } else {
        printf(" Test 10: Error.\n");
    }

    // Create IDLValue with record and extract the record form it 
    const char* keys[] = {
        "key1",
        "key2",
    };
    const IDLValue* vals[] = {
    idl_value_with_nat8(2),
    idl_value_with_null(),
    };

    idl_value_destroy(val);
  val = idl_value_with_record(keys, 2, vals, 2, false);

  CRecord *rec = record_from_idl_value(val);
  IDLValue *rec_value = crecord_take_val(rec, 0);
  is_valid = nat8_from_idl_value(crecord_take_val(rec, 0), &nat);
  idl_value_destroy(rec_value);

  CText *rec_field = crecord_take_key(rec, 1);
  if (is_valid && nat == 2 && !strcmp("key2", ctext_str(rec_field))) {
    printf(" Test 11: Valid Record\n");
  } else {
    printf(" Test 11: Error.\n");
  }
  ctext_destroy(rec_field);

  idl_value_destroy(val);
  ctext_destroy(t);
  principal_destroy(id);
  cvariant_destroy(var);
  idl_value_destroy((IDLValue *)var_val);
  cfunc_destroy(func);
  crecord_destroy(rec);

  return 0;
}
