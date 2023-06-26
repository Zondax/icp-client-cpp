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
#include "helper.h"

#include <filesystem>
#include <fstream>
#include <iostream>

std::uintmax_t did_file_content(const std::string& didFilePath,
                                std::vector<char>& outBuf) {
  auto file_size = did_file_size(didFilePath);

  if (file_size > 0) {
    outBuf.resize(file_size);
  }

  std::ifstream file(didFilePath, std::ios::binary);

  file.read(&outBuf[0], file_size);

  if (!file) {
    std::cerr << "Error reading file: " << didFilePath
              << " \ncould only read: " << file.gcount() << " bytes"
              << std::endl;
    return 0;
  }

  auto read = file.gcount();

  file.close();

  return read;
}

std::uintmax_t did_file_size(const std::string& didFilePath) {
  std::uintmax_t filesize = std::filesystem::file_size(didFilePath);
  return filesize;
}
