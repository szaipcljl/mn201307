#ifndef ANDROID_BASE_LOGGING_H
#define ANDROID_BASE_LOGGING_H

#include <functional>

namespace android {
namespace base {

enum LogSeverity {
  VERBOSE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL_WITHOUT_ABORT,
  FATAL,
};


enum LogId {
  DEFAULT,
  MAIN,
  SYSTEM,
};

using LogFunction = std::function<void(LogId, LogSeverity, const char*, const char*,
                                       unsigned int, const char*)>;
using AbortFunction = std::function<void(const char*)>;
void StderrLogger(LogId, LogSeverity, const char*, const char*, unsigned int, const char*);

void DefaultAborter(const char* abort_message);

#if defined(__linux__)
void KernelLogger(android::base::LogId, android::base::LogSeverity severity,
                  const char* tag, const char*, unsigned int, const char* msg) {
#if 0
  // clang-format off
  static constexpr int kLogSeverityToKernelLogLevel[] = {
      [android::base::VERBOSE] = 7,              // KERN_DEBUG (there is no verbose kernel log
                                                 //             level)
      [android::base::DEBUG] = 7,                // KERN_DEBUG
      [android::base::INFO] = 6,                 // KERN_INFO
      [android::base::WARNING] = 4,              // KERN_WARNING
      [android::base::ERROR] = 3,                // KERN_ERROR
      [android::base::FATAL_WITHOUT_ABORT] = 2,  // KERN_CRIT
      [android::base::FATAL] = 2,                // KERN_CRIT
  };
  // clang-format on
  static_assert(arraysize(kLogSeverityToKernelLogLevel) == android::base::FATAL + 1,
                "Mismatch in size of kLogSeverityToKernelLogLevel and values in LogSeverity");

  static int klog_fd = TEMP_FAILURE_RETRY(open("/dev/kmsg", O_WRONLY | O_CLOEXEC));
  if (klog_fd == -1) return;

  int level = kLogSeverityToKernelLogLevel[severity];

  // The kernel's printk buffer is only 1024 bytes.
  // TODO: should we automatically break up long lines into multiple lines?
  // Or we could log but with something like "..." at the end?
  char buf[1024];
  size_t size = snprintf(buf, sizeof(buf), "<%d>%s: %s\n", level, tag, msg);
  if (size > sizeof(buf)) {
    size = snprintf(buf, sizeof(buf), "<%d>%s: %zu-byte message too long for printk\n",
                    level, tag, size);
  }

  iovec iov[1];
  iov[0].iov_base = buf;
  iov[0].iov_len = size;
  TEMP_FAILURE_RETRY(writev(klog_fd, iov, 1));
#endif
}
#endif

// Configure logging based on ANDROID_LOG_TAGS environment variable.
// We need to parse a string that looks like
//
//      *:v jdwp:d dalvikvm:d dalvikvm-gc:i dalvikvmi:i
//
// The tag (or '*' for the global level) comes first, followed by a colon and a
// letter indicating the minimum priority level we're expected to log.  This can
// be used to reveal or conceal logs with specific tags.
#ifdef __ANDROID__
#define INIT_LOGGING_DEFAULT_LOGGER LogdLogger()
#else
#define INIT_LOGGING_DEFAULT_LOGGER StderrLogger
#endif
void InitLogging(char* argv[],
                 LogFunction&& logger = INIT_LOGGING_DEFAULT_LOGGER,
                 AbortFunction&& aborter = DefaultAborter);
#undef INIT_LOGGING_DEFAULT_LOGGER

}  // namespace base
}  // namespace android

#endif
