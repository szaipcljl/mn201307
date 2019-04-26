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

static int signal_write_fd = -1;
static int signal_read_fd = -1;

// test function
static void prompt_info(void)
{
	char msg[] = "time is running out.\n";
	int len;

	len = strlen(msg);
	write(STDERR_FILENO, msg, len);
}

// the actual handler
static void handle_signal()
{
	char buf[32];
	read(signal_read_fd, buf, sizeof(buf));

	//TODO: 
	prompt_info(); //test
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
