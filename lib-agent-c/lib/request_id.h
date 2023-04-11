/*******************************************************************************
 *   (c) 2018 - 2022 Zondax AG
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
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
struct RequestId {
  const uint8_t *ptr;
  int len;
};

/**
 * @brief Get the Requested Id from sha256 hash
 *
 * @param hash Sha256 hash
 * @param hash_len Hash size
 * @param id Return pointer to requested id
 * @return Return 0 in case of success
 */
int get_requested_id(const uint8_t *hash, int hash_len, struct RequestId *id);

void request_id_free(void);
