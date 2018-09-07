#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>


#ifndef INIT_DEPENDENCE_H_H
#define INIT_DEPENDENCE_H_H

#define FS_MGR_MNTALL_DEV_NON_DEFAULT_FILE_ENCRYPTED 5

#define FS_MGR_MNTALL_DEV_DEFAULT_FILE_ENCRYPTED 4
#define FS_MGR_MNTALL_DEV_NEEDS_RECOVERY 3
#define FS_MGR_MNTALL_DEV_NEEDS_ENCRYPTION 2
#define FS_MGR_MNTALL_DEV_MIGHT_BE_ENCRYPTED 1
#define FS_MGR_MNTALL_DEV_NOT_ENCRYPTED 0
#define FS_MGR_MNTALL_FAIL -1

#define ANDROID_SOCKET_ENV_PREFIX	"ANDROID_SOCKET_"
#define ANDROID_SOCKET_DIR		"/dev/socket"


#define PROP_NAME_MAX 32
#define PROP_VALUE_MAX 92
#define PATH_MAX        1024

struct fstab_rec {
    char *blk_device;
    char *mount_point;
    char *fs_type;
    unsigned long flags;
    char *fs_options;
    int fs_mgr_flags;
    char *key_loc;
    char *verity_loc;
    long long length;
    char *label;
    int partnum;
    int swap_prio;
    unsigned int zram_size;
};

typedef unsigned short security_class_t;



///
/// $$$ klog
///
#define KLOG_ERROR_LEVEL   3
#define KLOG_WARNING_LEVEL 4
#define KLOG_NOTICE_LEVEL  5
#define KLOG_INFO_LEVEL    6
#define KLOG_DEBUG_LEVEL   7

#define KLOG_DEFAULT_LEVEL  3  /* messages <= this level are logged */


#if 0
#define TEMP_FAILURE_RETRY(exp) ({         \
	typeof (exp) _rc;                      \
	do {                                   \
    	_rc = (exp);                       \
	} while (_rc == -1 && errno == EINTR); \
	_rc; })
#endif
namespace android {
namespace base {

extern bool ReadFdToString(int fd, std::string* content);
extern bool WriteStringToFd(const std::string& content, int fd);
extern void StringAppendF(std::string* dst, const char* format, ...);
extern std::string  StringPrintf(const char* format, ...);

}
}




extern int klog_get_level(void);
extern void klog_set_level(int level);
extern void klog_init(void);
extern size_t strlcat(char *dst, const char *src, size_t siz);

#endif
	
