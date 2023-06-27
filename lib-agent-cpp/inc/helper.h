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
#ifndef HELPER_H
#define HELPER_H

#include <filesystem>
#include <vector>

#define ERR -1
#define OK 0

std::uintmax_t did_file_content(const std::string& didFilePath,
                                std::vector<char>& buffer);
std::uintmax_t did_file_size(const std::string& didFilePath);
#endif
