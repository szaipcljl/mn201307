#ifndef _INIT_UEVENTD_H_
#define _INIT_UEVENTD_H_
#include <cutils/list.h>
#include <sys/types.h>

enum devname_src_t {
    DEVNAME_UNKNOWN = 0,
    DEVNAME_UEVENT_DEVNAME,
    DEVNAME_UEVENT_DEVPATH,
};

struct ueventd_subsystem {
    struct listnode slist;

    const char *name;
    const char *dirname;
    devname_src_t devname_src;
};

int ueventd_main(int argc, char **argv);

#endif
