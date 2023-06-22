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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Get Anonymous ID
 * 
 * @param id Identity struture pointer
 */
void anonymous_identity(CIdentity *id);

/**
 * @brief Get Basic identity using data from PEM file
 * 
 * @param pem_data Pem file content
 * @param id Identity struture pointer
 * @param error_cb returned error
 */
void basic_identity_from_pem(const char *pem_data, CIdentity *id,
                             RetError *error);

/**
 * @brief Get Basic Identity from pair of keys
 * 
 * @param public_key Public Key 32 bytes
 * @param private_key_seed Priovate key seed
 * @param id Identity struture pointer
 * @param error_cb returned error
 */
void basic_identity_from_key_pair(const uint8_t *public_key,
                                  const uint8_t *private_key_seed, CIdentity *id,
                                  RetError *error_ret);

/**
 * @brief Get Secp256k1 identity using data from PEM file
 * 
 * @param pem_data Pem file content
 * @param id Identity struture pointer
 * @param error_cb returned error
 */
void secp256k1_identity_from_pem(const char *pem_data, CIdentity *id,
                                 RetError *error);

/**
 * @brief Get Secp256k1 identity from private key
 * 
 * @param private_key Private Key content
 * @param pk_len 
 * @param id Identity struture pointer
 */
void secp256k1_identity_from_key_pair(const char *private_key, uintptr_t pk_len,
                                      CIdentity *id);

#ifdef __cplusplus
}
#endif
