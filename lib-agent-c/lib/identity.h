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

#define ANONYMOUD_ID_LEN 1

struct Identity {
  const void **identity;
  IdentityType type;
};

struct Signature {
  const uint8_t *ptr;
  int len;
};

struct PublicKey {
  const uint8_t *ptr;
  int len;
};

int get_identity_anonymous(struct Identity *id);
int get_identity_basic_from_pem(const char *pem, struct Identity *id,
                                struct Error *e);
int get_identity_basic_from_key_pair(const uint8_t *publicKey,
                                     const uint8_t *privateKeySeed,
                                     struct Identity *id, struct Error *e);
int get_identity_secp256k1_from_pem(const char *pem, struct Identity *id,
                                    struct Error *e);
int get_identity_secp256k1_from_privateKey(const char *privateKey,
                                           uintptr_t pkLen,
                                           struct Identity *id);
int identity_sender(struct Identity *id, struct Principal *principal,
                    struct Error *e);
int identity_sign(const uint8_t *bytes, int bytesLen, struct Identity *id,
                  struct PublicKey *publicKey, struct Signature *signature,
                  struct Error *e);
void identity_free(void);
