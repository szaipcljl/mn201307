#ifndef _UTIL_H
#define _UTIL_H

#include <chrono>
#include <string>

#define COLDBOOT_DONE "/dev/.coldboot_done"

int mkdir_recursive(const char *pathname, mode_t mode);
void sanitize(char *p);
int make_dir(const char *path, mode_t mode);

#if 0
class Timer {
 public:
  Timer() : start_(boot_clock::now()) {
  }

  double duration_s() const {
    typedef std::chrono::duration<double> double_duration;
    return std::chrono::duration_cast<double_duration>(boot_clock::now() - start_).count();
  }

  int64_t duration_ms() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(boot_clock::now() - start_).count();
  }

 private:
  boot_clock::time_point start_;
};
#endif

// A std::chrono clock based on CLOCK_BOOTTIME.
class boot_clock {
 public:
  typedef std::chrono::nanoseconds duration;
  typedef std::chrono::time_point<boot_clock, duration> time_point;
  static constexpr bool is_steady = true;

  static time_point now();
};

//void panic() __attribute__((__noreturn__));
void panic();

bool read_file(const std::string& path, std::string* content);
#endif
