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

#include "android-base/strings.h"

#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

namespace android {
namespace base {
// These cases are probably the norm, so we mark them extern in the header to
// aid compile time and binary size.
template std::string Join(const std::vector<std::string>&, char);
template std::string Join(const std::vector<const char*>&, char);
template std::string Join(const std::vector<std::string>&, const std::string&);
template std::string Join(const std::vector<const char*>&, const std::string&);

}  // namespace base
}  // namespace android
