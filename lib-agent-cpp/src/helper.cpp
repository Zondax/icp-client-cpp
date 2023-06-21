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
#include <iostream>
#include <fstream>
#include <vector>
#include "helper.h"

int did_file_content(const std::string& didFilePath, long file_size, char* buffer) {
    std::ifstream file(didFilePath, std::ios::binary);
    if (!file) {
        return -1;  // error opening file
    }

    // Read the file contents into the buffer
    file.read(buffer, file_size);
    buffer[file_size] = '\0';

    // Close the file and return the buffer
    file.close();
    return 0;
}

long did_file_size(const std::string& didFilePath) {
    std::ifstream file(didFilePath, std::ios::binary | std::ios::ate);
    if (!file) {
        return 0;  // error opening file
    }

    // Get the file size
    long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    file.close();
    return file_size + 1;
}
