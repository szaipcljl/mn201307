#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>

#include "signal_handler.h"
#include "timer.h"

#include "fault_detector/cmr_fault_detector.h"
#include "diagnostic.h"
#include "debug.h"

int signal_write_fd = -1;
int signal_read_fd = -1;

// test function
static void prompt_info(void)
{
	char msg[] = "time is running out.\n";
	int len;

	len = strlen(msg);
	write(STDERR_FILENO, msg, len);
}

// the actual handler
#ifdef DAEMON_DEBUG
static void handle_signal()
#else
static void handle_signal(struct cmr_fault_detector *cmr_ft_dtect)
#endif
{
	char buf[32];
	read(signal_read_fd, buf, sizeof(buf));

	//TODO:
#ifdef DAEMON_DEBUG
	prompt_info(); //test
#else
	if (buf[0] == VSYNC_LOST[0]) {
		printf("notify from vsync_lost\n");
		//TODO:get mutex
		cmr_ft_dtect->vsync_lost = 1;
		//TODO:release mutex
	} else {
		//do nothing
		printf("[%s] time expires\n", __func__);
	}

	cmr_ft_diagnostic_work(cmr_ft_dtect);
#endif
}

static void SIGALRM_handler(int arg)
{
	if (TEMP_FAILURE_RETRY(write(signal_write_fd, "1", 1)) == -1) {
		printf("write(signal_write_fd) failed: %s\n", strerror(errno));
	}
}

void init_sigaction(void (*fn)())
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_handler = fn;
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, NULL);
}

void signal_handler_init()
{
	// Create a signalling mechanism for SIGALRM.
	int s[2];
	if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, s) == -1) {
		printf("socketpair failed: %s\n", strerror(errno));
		exit(1);
	}

	signal_write_fd = s[0];
	signal_read_fd = s[1];

	// Write to signal_write_fd if we catch SIGALRM.
	init_sigaction(SIGALRM_handler);

	register_epoll_handler(signal_read_fd, handle_signal);
}
