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
#ifndef IDL_VALUE_H
#define IDL_VALUE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include "principal.h"
#include "idl_value_utils.h"


extern "C" {
#include "zondax_ic.h"
}

namespace zondax::idl_value {

class IdlValue {  
private:
    IDLValue *ptr;

public:

    // Disable copies, just move semantics
    IdlValue(const IdlValue &args) = delete;
    void operator=(const IdlValue&) = delete;

    // declare move constructor
    IdlValue(IdlValue &&o) noexcept;
    // declare move assignment
    IdlValue& operator=(IdlValue &&o) noexcept;

    // Create IdlValues from types
    IdlValue(IDLValue *ptr);
    IdlValue();
    explicit IdlValue(uint8_t val);
    explicit IdlValue(uint16_t val);
    explicit IdlValue(uint32_t val);
    explicit IdlValue(uint64_t val);
    explicit IdlValue(int8_t val);
    explicit IdlValue(int16_t val);
    explicit IdlValue(int32_t val);
    explicit IdlValue(int64_t val);
    explicit IdlValue(float val);
    explicit IdlValue(double val);
    explicit IdlValue(bool val);
    explicit IdlValue(std::string text);
    explicit IdlValue(zondax::principal::Principal principal, bool type = true); // true create principal false: service
    IdlValue FromNull(void);
    IdlValue FromNone(void);
    IdlValue FromReserved(void);
    IdlValue FromNumber(std::string number);
    IdlValue FromOpt(IdlValue *value);
    IdlValue FromVec(const std::vector<const IdlValue *> &elems);
    IdlValue FromRecord(const std::vector<std::string> &keys, const std::vector<const IdlValue *> &elems);
    IdlValue FromVariant(std::string key, IdlValue *val, uint64_t code);
    IdlValue FromFunc(std::vector<uint8_t> vector, std::string func_name);

    // Get types
    bool getInt8(int8_t *val);
    bool getInt16(int16_t *val);
    bool getInt32(int32_t *val);
    bool getInt64(int64_t *val);
    bool getNat8(uint8_t *val);
    bool getNat16(uint16_t *val);
    bool getNat32(uint32_t *val);
    bool getNat64(uint64_t *val);
    bool getFloat32(float *val);
    bool getFloat64(double *val);
    bool getBool(bool *val);
    std::string getText();
    zondax::principal::Principal getPrincipal();
    zondax::principal::Principal getService();
    std::string getNumber();
    IdlValue getOpt();
    std::vector<IdlValue> getVec();
    // zondax::idl_value_utils::Record getRecord();
    // zondax::idl_value_utils::Variant getVariant();
    zondax::idl_value_utils::Func getFunc();

   IDLValue* getPtr() const;
   ~IdlValue();
};
}


#endif
