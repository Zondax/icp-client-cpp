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

namespace zondax {

// declare move constructor
IdlArgs::IdlArgs(IdlArgs&& o) noexcept {
  ptr = o.ptr;
  o.ptr = nullptr;
}

// declare move assignment
IdlArgs& IdlArgs::operator=(IdlArgs&& o) noexcept {
  // check they are not the same object
  if (&o == this) return *this;

  if (ptr != nullptr) idl_args_destroy(ptr);

  ptr = o.ptr;

  o.ptr = nullptr;

  return *this;
}

IdlArgs::IdlArgs(IDLArgs* argsPtr) : ptr(argsPtr){};

IdlArgs::IdlArgs(std::vector<zondax::IdlValue> values) {
  ptr = empty_idl_args();
  // Convert vector of IdlValue pointers to an array of const pointers
  for (int i = 0; i < values.size(); ++i) {
    // pass value.ptr to rust, note that rust takes ownership of it.
    idl_args_push_value(ptr, values[i].getPtr());
    // set this ptr alias that now is pointed to memory owned by rust,
    // to nullptr, so that at the end of this scope, the destructor
    // would see it is null, avoiding a potential double-free error.
    values[i].resetValue();
  }
}

IdlArgs::IdlArgs(std::vector<uint8_t> bytes) {
  ptr = idl_args_from_bytes(bytes.data(), bytes.size(), nullptr);
}

IdlArgs::IdlArgs(std::string text) {
  ptr = idl_args_from_text(text.c_str(), nullptr);
}

std::string IdlArgs::getText() {
  CText* cText = idl_args_to_text(ptr);
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
  CBytes* cBytes = idl_args_to_bytes(ptr, nullptr);
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

  CIDLValuesVec* cVec = idl_args_to_vec(ptr);
  std::vector<zondax::IdlValue> vec;

  for (uintptr_t i = 0; i < cidlval_vec_len(cVec); ++i) {
    const IDLValue* idlValue = cidlval_vec_value(cVec, i);
    vec.push_back(zondax::IdlValue(idlValue));
  }

  // Free the allocated CIDLValuesVec
  cidlval_vec_destroy(cVec);
  return vec;
}

IDLArgs* IdlArgs::getPtr() const { return ptr; }

IdlArgs::~IdlArgs() {
  if (ptr != nullptr) idl_args_destroy(ptr);
}

}  // namespace zondax
