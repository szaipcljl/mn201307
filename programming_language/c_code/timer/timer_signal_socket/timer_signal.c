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


#define MSEC_to_USEC(x)			((x)*1000)

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


	epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd == -1) {
		printf("epoll_create1 failed: %s\n", strerror(errno));
		exit(1);
	}

	signal_handler_init();

	init_time();

	while (1) {
		int timeout = -1;
		struct epoll_event ev;

		int nr = TEMP_FAILURE_RETRY(epoll_wait(epoll_fd, &ev, 1, timeout));
		if (nr == -1) {
			printf("epoll_wait failed: %s\n", strerror(errno));
		} else if (nr == 1) {
			((void (*)()) ev.data.ptr)();
		} else {
			// do nothing
		}
	}

	exit(0);
}
