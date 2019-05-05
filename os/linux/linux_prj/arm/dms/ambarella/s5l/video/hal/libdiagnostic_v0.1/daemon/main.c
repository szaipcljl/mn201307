#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>

#include "signal_handler.h"
#include "timer.h"

#include "fault_detector/cmr_fault_detector.h"
#include "debug.h"


#define MSEC_to_USEC(x)			((x)*1000)

extern int signal_write_fd;
extern int signal_read_fd;

extern int fd_iav;
struct cmr_fault_detector cmr_ft_dtect;

static int epoll_fd = -1;

void register_epoll_handler(int fd, void (*fn)())
{
	struct  epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = (void (*)(int))(fn);
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		printf("epoll_ctl failed: %s\n", strerror(errno));
	}
}

void init_time(void)
{
	struct itimerval value;

	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = MSEC_to_USEC(100);
	value.it_interval = value.it_value;

	setitimer(ITIMER_REAL, &value, NULL);
}

int main(int argc, const char *argv[])
{
	int ret;
	pthread_t tid[2];

	ret = cmr_ft_detector_init(&cmr_ft_dtect);
	if (ret < 0) {
		printf("cmr_ft_detector_init failed \n");
		return -1;
	}
	fd_iav = cmr_ft_dtect.cmr_dev.iav_fd;

	signal_handler_init();

	cmr_ft_dtect.signal_write_fd = signal_write_fd;
	cmr_ft_dtect.signal_read_fd = signal_read_fd;

	ret = cmr_guarder_run(&tid[0], &signal_write_fd);
	if (ret != 0) {
		printf("[%s]: cmr_guarder_run failed\n", __func__);
		goto fail1;
	}

	epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd == -1) {
		printf("epoll_create1 failed: %s\n", strerror(errno));
		exit(1);
	}


	init_time();

	while (1) {
		int timeout = -1;
		struct epoll_event ev;

		int nr = TEMP_FAILURE_RETRY(epoll_wait(epoll_fd, &ev, 1, timeout));
		if (nr == -1) {
			printf("epoll_wait failed: %s\n", strerror(errno));
		} else if (nr == 1) {
#ifdef DAEMON_DEBUG
			((void (*)()) ev.data.ptr)();
#else
			((void (*)(struct cmr_fault_detector *)) ev.data.ptr)(&cmr_ft_dtect);
#endif
		} else {
			// do nothing
		}
	}

	exit(0);

fail1:
	cmr_ft_detector_exit(&cmr_ft_dtect);
	return -1;
}
