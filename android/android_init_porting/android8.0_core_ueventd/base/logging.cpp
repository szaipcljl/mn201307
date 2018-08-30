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
#include "android-base/logging.h"

namespace android {
namespace base {


void InitLogging(char* argv[], LogFunction&& logger, AbortFunction&& aborter) {
#if 0
  SetLogger(std::forward<LogFunction>(logger));
  SetAborter(std::forward<AbortFunction>(aborter));

  if (gInitialized) {
    return;
  }

  gInitialized = true;

  // Stash the command line for later use. We can use /proc/self/cmdline on
  // Linux to recover this, but we don't have that luxury on the Mac/Windows,
  // and there are a couple of argv[0] variants that are commonly used.
  if (argv != nullptr) {
    std::lock_guard<std::mutex> lock(LoggingLock());
    ProgramInvocationName() = basename(argv[0]);
  }

  const char* tags = getenv("ANDROID_LOG_TAGS");
  if (tags == nullptr) {
    return;
  }

  std::vector<std::string> specs = Split(tags, " ");
  for (size_t i = 0; i < specs.size(); ++i) {
    // "tag-pattern:[vdiwefs]"
    std::string spec(specs[i]);
    if (spec.size() == 3 && StartsWith(spec, "*:")) {
      switch (spec[2]) {
        case 'v':
          gMinimumLogSeverity = VERBOSE;
          continue;
        case 'd':
          gMinimumLogSeverity = DEBUG;
          continue;
        case 'i':
          gMinimumLogSeverity = INFO;
          continue;
        case 'w':
          gMinimumLogSeverity = WARNING;
          continue;
        case 'e':
          gMinimumLogSeverity = ERROR;
          continue;
        case 'f':
          gMinimumLogSeverity = FATAL_WITHOUT_ABORT;
          continue;
        // liblog will even suppress FATAL if you say 's' for silent, but that's
        // crazy!
        case 's':
          gMinimumLogSeverity = FATAL_WITHOUT_ABORT;
          continue;
      }
    }
    LOG(FATAL) << "unsupported '" << spec << "' in ANDROID_LOG_TAGS (" << tags
               << ")";
  }
#endif
}

}  // namespace base
}  // namespace android
