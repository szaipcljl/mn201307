#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

static char msg[] = "time is running out.\n";
static int len;

void prompt_info(int signo)
{
	write(STDERR_FILENO, msg, len);
}

void init_sigaction(void)
{
	struct sigaction tact;

	tact.sa_handler = prompt_info;
	tact.sa_flags = 0;

	sigemptyset(&tact.sa_mask);
	sigaction(SIGALRM, &tact, NULL);
}

void init_time()
{
	struct itimerval value;

	value.it_value.tv_sec = 2;
	value.it_value.tv_usec = 0;
	value.it_interval = value.it_value;

	setitimer(ITIMER_REAL, &value, NULL);
}

int main(int argc, const char *argv[])
{
#define SLEEP_TIME 1

	len = strlen(msg);

	init_sigaction();
	init_time();

	while (1) {
		sleep(SLEEP_TIME);
		printf("[%ds have elapsed or a signal arrives which is not ignored.]\n", SLEEP_TIME);
	}

	exit(0);
}
