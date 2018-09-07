/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <grp.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <iostream>

#include <android-base/stringprintf.h>

#include "keyutils.h"
#include "ueventd.h"
#include "util.h"
#include "watchdogd.h"

using android::base::StringPrintf;

const char *ENV[32];
static int epoll_fd = -1;

template <typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];  // NOLINT(readability/casting)

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define       _PATH_DEFPATH   "/sbin:/system/sbin:/system/bin:/system/xbin:/vendor/bin:/vendor/xbin"
#define AID_READPROC 3009     /* Allow /proc read access */


/* add_environment - add "key=value" to the current environment */
int add_environment(const char *key, const char *val)
{
    size_t n;
    size_t key_len = strlen(key);

    /* The last environment entry is reserved to terminate the list */
    for (n = 0; n < (arraysize(ENV) - 1); n++) {

        /* Delete any existing entry for this key */
        if (ENV[n] != NULL) {
            size_t entry_key_len = strcspn(ENV[n], "=");
            if ((entry_key_len == key_len) && (strncmp(ENV[n], key, entry_key_len) == 0)) {
                free((char*)ENV[n]);
                ENV[n] = NULL;
            }
        }

        /* Add entry if a free slot is available */
        if (ENV[n] == NULL) {
            char* entry;
            asprintf(&entry, "%s=%s", key, val);
            ENV[n] = entry;
            return 0;
        }
    }

    //LOG(ERROR) << "No env. room to store: '" << key << "':'" << val << "'";

    return -1;
}

static void install_reboot_signal_handlers() {
    // Instead of panic'ing the kernel as is the default behavior when init crashes,
    // we prefer to reboot to bootloader on development builds, as this will prevent
    // boot looping bad configurations and allow both developers and test farms to easily
    // recover.
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    sigfillset(&action.sa_mask);
    action.sa_handler = [](int) {
        // panic() reboots to bootloader
        panic();
    };
    action.sa_flags = SA_RESTART;
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGBUS, &action, nullptr);
    sigaction(SIGFPE, &action, nullptr);
    sigaction(SIGILL, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);
#if defined(SIGSTKFLT)
    sigaction(SIGSTKFLT, &action, nullptr);
#endif
    sigaction(SIGSYS, &action, nullptr);
    sigaction(SIGTRAP, &action, nullptr);
}

int main(int argc, char** argv) {
	if (!strcmp(basename(argv[0]), "ueventd")) {
		return ueventd_main(argc, argv);
	}

    if (!strcmp(basename(argv[0]), "watchdogd")) {
        return watchdogd_main(argc, argv);
    }

    if (REBOOT_BOOTLOADER_ON_PANIC) {
        install_reboot_signal_handlers();
    }

    add_environment("PATH", _PATH_DEFPATH);

    bool is_first_stage = (getenv("INIT_SECOND_STAGE") == nullptr);

    if (is_first_stage) {
        boot_clock::time_point start_time = boot_clock::now();

        // Clear the umask.
        umask(0);

        // Get the basic filesystem setup we need put together in the initramdisk
        // on / and then we'll let the rc file figure out the rest.
        mount("tmpfs", "/dev", "tmpfs", MS_NOSUID, "mode=0755");
        mkdir("/dev/pts", 0755);
        mkdir("/dev/socket", 0755);
        mount("devpts", "/dev/pts", "devpts", 0, NULL);
        #define MAKE_STR(x) __STRING(x)
        mount("proc", "/proc", "proc", 0, "hidepid=2,gid=" MAKE_STR(AID_READPROC));
        // Don't expose the raw commandline to unprivileged processes.
        chmod("/proc/cmdline", 0440);
        gid_t groups[] = { AID_READPROC };
        setgroups(arraysize(groups), groups);
        mount("sysfs", "/sys", "sysfs", 0, NULL);
        mount("selinuxfs", "/sys/fs/selinux", "selinuxfs", 0, NULL);
        mknod("/dev/kmsg", S_IFCHR | 0600, makedev(1, 11));
        mknod("/dev/random", S_IFCHR | 0666, makedev(1, 8));
        mknod("/dev/urandom", S_IFCHR | 0666, makedev(1, 9));

        // Now that tmpfs is mounted on /dev and we have /dev/kmsg, we can actually
        // talk to the outside world...
        //InitKernelLogging(argv);

		std::cout << "init first stage started!"<< std::endl;

#if 0 //rda600 not use /system /vendor /dom
		if (!DoFirstStageMount()) {
			//LOG(ERROR) << "Failed to mount required partitions early ...";
			panic();
		}
#endif

        //SetInitAvbVersionInRecovery();

#ifdef ENABLE_SELINUX_SUPPORT
        // Set up SELinux, loading the SELinux policy.
        selinux_initialize(true);

        // We're in the kernel domain, so re-exec init to transition to the init domain now
        // that the SELinux policy has been loaded.
        if (restorecon("/init") == -1) {
            PLOG(ERROR) << "restorecon failed";
            security_failure();
        }
#endif

        setenv("INIT_SECOND_STAGE", "true", 1);

        static constexpr uint32_t kNanosecondsPerMillisecond = 1e6;
        uint64_t start_ms = start_time.time_since_epoch().count() / kNanosecondsPerMillisecond;
		setenv("INIT_STARTED_AT", StringPrintf("%" PRIu64, start_ms).c_str(), 1);

        char* path = argv[0];
        char* args[] = { path, nullptr };
		std::cout << "execv(\"" << path << "\")! "<< std::endl;
        execv(path, args);

        // execv() only returns if an error happened, in which case we
        // panic and never fall through this conditional.
		std::cout << "execv(\"" << path << "\") failed";
#ifdef ENABLE_SELINUX_SUPPORT
        security_failure();
#endif
    }

    // At this point we're in the second stage of init.
    //InitKernelLogging(argv);
	std::cout << "init second stage started!"<< std::endl;

    // Set up a session keyring that all processes will have access to. It
    // will hold things like FBE encryption keys. No process should override
    // its session keyring.
    keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_SESSION_KEYRING, 1);

    // Indicate that booting is in progress to background fw loaders, etc.
    close(open("/dev/.booting", O_WRONLY | O_CREAT | O_CLOEXEC, 0000));

#ifdef ENABLE_PROPERTY_SUPPORT
	property_init();
#endif

    // If arguments are passed both on the command line and in DT,
    // properties set in DT always have priority over the command-line ones.
    //process_kernel_dt();
    //process_kernel_cmdline();

    // Propagate the kernel variables to internal variables
    // used by init as well as the current required properties.
#ifdef ENABLE_PROPERTY_SUPPORT
	export_kernel_boot_props();
#endif

    // Make the time that init started available for bootstat to log.
#ifdef ENABLE_PROPERTY_SUPPORT
	property_set("ro.boottime.init", getenv("init_started_at"));
#ifdef ENABLE_SELINUX_SUPPORT
	property_set("ro.boottime.init.selinux", getenv("init_selinux_took"));
#endif
#endif

    // Set libavb version for Framework-only OTA match in Treble build.
#ifdef ENABLE_AVB
	const char* avb_version = getenv("INIT_AVB_VERSION");
#ifdef ENABLE_PROPERTY_SUPPORT
	if (avb_version) property_set("ro.boot.avb_version", avb_version);
#endif
#endif

    // Clean up our environment.
    unsetenv("INIT_SECOND_STAGE");
    unsetenv("INIT_STARTED_AT");
#ifdef ENABLE_SELINUX_SUPPORT
    unsetenv("INIT_SELINUX_TOOK");
#endif
#ifdef ENABLE_AVB
	unsetenv("INIT_AVB_VERSION");
#endif

#ifdef ENABLE_SELINUX_SUPPORT
    // Now set up SELinux for second stage.
    selinux_initialize(false);
    selinux_restore_context();
#endif

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        //PLOG(ERROR) << "epoll_create1 failed";
        exit(1);
    }

	std::cout << "#mn: z!"<< std::endl;

#define DEBUG_CODE
#ifdef DEBUG_CODE
	pid_t pid;
	pid = fork();

	if (pid == -1) {
		perror("fork");
		return -1;
	} else if(pid == 0) {
		printf("child\n");

		char* const path = (char* const)"/sbin/ueventd";
		char* const args[] = { path, nullptr };
		std::cout << "execv(\"" << path << "\")! "<< std::endl;
		execv(path, args);
		printf("child2\n");
		while(1)
			sleep(10);
	} else {
		printf("father\n");
	}

	while (access(COLDBOOT_DONE, F_OK) != 0) {
		usleep(10000);
	}

#if 0

	//Bypass the login validation process and enter the shell interface directly
	sleep(10);
	std::cout << "#mn: here need sleep?"<< std::endl;

	char* const path = (char* const)"/bin/sh";
	char* const args[] = { path, nullptr };
	std::cout << "execv(\"" << path << "\")! "<< std::endl;
	execv(path, args);
	std::cout << "#mn: after sleep -2 !"<< std::endl;
#else

	// "/sbin/getty -L 115200 ttyLF0"
	char* const path = (char* const)"/sbin/getty";
	char* const args[] = { path, (char* const)"-L", (char* const)"115200", (char* const)"ttyLF0", nullptr };
	std::cout << "execv(\"" << path << "\")! "<< std::endl;
	execv(path, args);
	std::cout << "#mn: execv(/sbin/getty) !"<< std::endl;
#endif

	while(1)
		sleep(10);

#endif

    return 0;
}
