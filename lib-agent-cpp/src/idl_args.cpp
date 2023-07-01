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
#include "idl_args.h"

#include "idl_value.h"
#include "zondax_ic.h"

namespace zondax {

void IdlArgs::ensureNonEmpty() {
  if (ptr == nullptr) return;

  auto p = ptr.get();
  if (idl_args_len(p) == 0) {
    idl_args_push_value(p, IdlValue::null().getPtr().release());
  }
}

// declare move constructor
IdlArgs::IdlArgs(IdlArgs&& o) noexcept : ptr(std::move(o.ptr)) {}

// declare move assignment
IdlArgs& IdlArgs::operator=(IdlArgs&& o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  ptr = std::move(o.ptr);

  return *this;
}

IdlArgs::IdlArgs(IDLArgs* argsPtr) : ptr(argsPtr){};

IdlArgs::IdlArgs(std::vector<zondax::IdlValue>& values) {
  ptr.reset(empty_idl_args());
  // Convert vector of IdlValue pointers to an array of const pointers
  for (int i = 0; i < values.size(); ++i) {
    // pass value.ptr to rust, note that rust takes ownership of it.
    idl_args_push_value(ptr.get(), values[i].getPtr().release());
  }
}

IdlArgs::IdlArgs(std::vector<uint8_t> bytes) {
  ptr.reset(idl_args_from_bytes(bytes.data(), bytes.size(), nullptr));
}

IdlArgs::IdlArgs(std::string text) {
  ptr.reset(idl_args_from_text(text.c_str(), nullptr));
}

std::string IdlArgs::getText() {
  CText* cText = idl_args_to_text(ptr.get());
  const char* str = ctext_str(cText);
  uintptr_t len = ctext_len(cText);

  std::string text(str, len);
  // Free the allocated CText
  ctext_destroy(cText);
  return text;
}

std::vector<uint8_t> IdlArgs::getBytes() {
  if (ptr == nullptr) {
    std::cerr << "IDLArgs instance uninitialized" << std::endl;
    return std::vector<uint8_t>();
  }

  // TODO: Remove null and use proper callback if needed
  CBytes* cBytes = idl_args_to_bytes(ptr.get(), nullptr);
  const uint8_t* byte = cbytes_ptr(cBytes);
  uintptr_t len = cbytes_len(cBytes);

  std::vector<uint8_t> bytes(byte, byte + len);
  // Free the allocated CText
  cbytes_destroy(cBytes);
  return bytes;
}

std::vector<zondax::IdlValue> IdlArgs::getVec() {
  if (ptr == nullptr) {
    std::cerr << "IDLArgs instance uninitialized" << std::endl;
    return std::vector<zondax::IdlValue>();
  }

  CIDLValuesVec* cVec = idl_args_to_vec(ptr.get());
  std::vector<zondax::IdlValue> vec;

  for (uintptr_t i = 0; i < cidlval_vec_len(cVec); ++i) {
    const IDLValue* idlValue = cidlval_vec_value(cVec, i);
    vec.push_back(zondax::IdlValue(idlValue));
  }

  // Free the allocated CIDLValuesVec
  cidlval_vec_destroy(cVec);
  return vec;
}

std::unique_ptr<IDLArgs> IdlArgs::getPtr() { return std::move(ptr); }
}  // namespace zondax
