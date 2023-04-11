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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

char* get_did_file_content(const char *didFilePath) {
    FILE *file = fopen(didFilePath, "rb");
    if (file == NULL) {
        return NULL;  // error opening file
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the file contents
    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;  // error allocating memory
    }

    // Read the file contents into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    // Close the file and return the buffer
    fclose(file);
    return buffer;
}
