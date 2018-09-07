/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_BASE_FILE_H
#define ANDROID_BASE_FILE_H

#include <sys/stat.h>
#include <string>

namespace android {
namespace base {

bool ReadFdToString(int fd, std::string* content);

bool WriteFully(int fd, const void* data, size_t byte_count);

#if !defined(_WIN32)
bool Realpath(const std::string& path, std::string* result);
bool Readlink(const std::string& path, std::string* result);
#endif

}  // namespace base
}  // namespace android
#endif
