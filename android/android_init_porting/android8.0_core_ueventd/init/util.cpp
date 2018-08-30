
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <thread>

#include <android-base/file.h>
#include <android-base/unique_fd.h>

#include "util.h"

int make_dir(const char *path, mode_t mode)
{
    int rc;
#ifdef ENABLE_SELINUX_SUPPORT

    char *secontext = NULL;

    if (sehandle) {
        selabel_lookup(sehandle, &secontext, path, mode);
        setfscreatecon(secontext);
    }
#endif

    rc = mkdir(path, mode);

#ifdef ENABLE_SELINUX_SUPPORT
    if (secontext) {
        int save_errno = errno;
        freecon(secontext);
        setfscreatecon(NULL);
        errno = save_errno;
    }
#endif

    return rc;
}


int mkdir_recursive(const char *pathname, mode_t mode)
{
    char buf[128];
    const char *slash;
    const char *p = pathname;
    int width;
    int ret;
    struct stat info;

    while ((slash = strchr(p, '/')) != NULL) {
        width = slash - pathname;
        p = slash + 1;
        if (width < 0)
            break;
        if (width == 0)
            continue;
        if ((unsigned int)width > sizeof(buf) - 1) {
            //LOG(ERROR) << "path too long for mkdir_recursive";
            return -1;
        }
        memcpy(buf, pathname, width);
        buf[width] = 0;
        if (stat(buf, &info) != 0) {
            ret = make_dir(buf, mode);
            if (ret && errno != EEXIST)
                return ret;
        }
    }
    ret = make_dir(pathname, mode);
    if (ret && errno != EEXIST)
        return ret;
    return 0;
}


boot_clock::time_point boot_clock::now() {
  timespec ts;
  clock_gettime(CLOCK_BOOTTIME, &ts);
  return boot_clock::time_point(std::chrono::seconds(ts.tv_sec) +
                                std::chrono::nanoseconds(ts.tv_nsec));
}

/*
 * replaces any unacceptable characters with '_', the
 * length of the resulting string is equal to the input string
 */
void sanitize(char *s)
{
    const char* accept =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789"
            "_-.";

    if (!s)
        return;

    while (*s) {
        s += strspn(s, accept);
        if (*s) *s++ = '_';
    }
}

void panic() {
    //LOG(ERROR) << "panic: rebooting to bootloader";
    //DoReboot(ANDROID_RB_RESTART2, "reboot", "bootloader", false);
}


bool read_file(const std::string& path, std::string* content) {
    content->clear();

    android::base::unique_fd fd(
        TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC)));
    if (fd == -1) {
        return false;
    }

    // For security reasons, disallow world-writable
    // or group-writable files.
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        //PLOG(ERROR) << "fstat failed for '" << path << "'";
        return false;
    }
    if ((sb.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
        //LOG(ERROR) << "skipping insecure file '" << path << "'";
        return false;
    }

    return android::base::ReadFdToString(fd, content);
}
