
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>


#include"initdependence.h"

///k log
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
typedef __typeof(sizeof(int)) size_t;



namespace android {
namespace base {

bool ReadFdToString(int fd, std::string* content) {
  content->clear();

  char buf[BUFSIZ];
  ssize_t n;
  while ((n = TEMP_FAILURE_RETRY(read(fd, &buf[0], sizeof(buf)))) > 0) {
    content->append(buf, n);
  }
  return (n == 0) ? true : false;
}






bool WriteStringToFd(const std::string& content, int fd) {
  const char* p = content.data();
  size_t left = content.size();
  while (left > 0) {
    ssize_t n = TEMP_FAILURE_RETRY(write(fd, p, left));
    if (n == -1) {
      return false;
    }
    p += n;
    left -= n;
  }
  return true;
}



void StringAppendF(std::string* dst, const char* format, ...) {
#if 0	
  va_list ap;
  va_start(ap, format);
  StringAppendV(dst, format, ap);
  va_end(ap);
#endif
}


std::string  StringPrintf(const char* format, ...) {
#if 0
  va_list ap;
  va_start(ap, format);
  string result;
  StringAppendV(&result, format, ap);
  va_end(ap);
  return result;
#endif 
}







}
}




///
///  $$$   klog
///


static int klog_fd = -1;
static int klog_level = KLOG_DEFAULT_LEVEL;

int klog_get_level(void) {
    return klog_level;
}

void klog_set_level(int level) {
    klog_level = level;
}

void klog_init(void) {
    if (klog_fd >= 0) return; /* Already initialized */

    klog_fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
    if (klog_fd >= 0) {
        return;
    }

    static const char* name = "/dev/__kmsg__";
    if (mknod(name, S_IFCHR | 0600, (1 << 8) | 11) == 0) {
        klog_fd = open(name, O_WRONLY | O_CLOEXEC);
        unlink(name);
    }
}








size_t
strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}



