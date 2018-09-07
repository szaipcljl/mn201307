#ifndef ANDROID_BASE_UNIQUE_FD_H
#define ANDROID_BASE_UNIQUE_FD_H
#include <unistd.h>

// Container for a file descriptor that automatically closes the descriptor as
// it goes out of scope.
//
//      unique_fd ufd(open("/some/path", "r"));
//      if (ufd.get() == -1) return error;
//
//      // Do something useful, possibly including 'return'.
//
//      return 0; // Descriptor is closed for you.
//
// unique_fd is also known as ScopedFd/ScopedFD/scoped_fd; mentioned here to help
// you find this class if you're searching for one of those names.
namespace android {
namespace base {

struct DefaultCloser {
  static void Close(int fd) {
    // Even if close(2) fails with EINTR, the fd will have been closed.
    // Using TEMP_FAILURE_RETRY will either lead to EBADF or closing someone
    // else's fd.
    // http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
    ::close(fd);
  }
};

template <typename Closer>
class unique_fd_impl final {
 public:
  unique_fd_impl() : value_(-1) {}

  explicit unique_fd_impl(int value) : value_(value) {}
  ~unique_fd_impl() { reset(); }

  unique_fd_impl(unique_fd_impl&& other) : value_(other.release()) {}
  unique_fd_impl& operator=(unique_fd_impl&& s) {
    reset(s.release());
    return *this;
  }

  void reset(int new_value = -1) {
    if (value_ != -1) {
      Closer::Close(value_);
    }
    value_ = new_value;
  }

  int get() const { return value_; }
  operator int() const { return get(); }

  int release() __attribute__((warn_unused_result)) {
    int ret = value_;
    value_ = -1;
    return ret;
  }

 private:
  int value_;

  unique_fd_impl(const unique_fd_impl&);
  void operator=(const unique_fd_impl&);
};

using unique_fd = unique_fd_impl<DefaultCloser>;

}  // namespace base
}  // namespace android
#endif
