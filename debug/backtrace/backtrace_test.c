#include <stdio.h>
#include <string.h>
/*#include <fcntl.h>*/
/*#include <unistd.h>*/
/*#include <sys/types.h>*/
/*#include <sys/stat.h>*/

#define BACKTRACE_OPEN

#ifdef BACKTRACE_OPEN

#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>

#define PRINT_DEBUG

#define MAX_BACKTRACE_LEVEL 10
#define BACKTRACE_LOG_NAME "backtrace.log"


static void show_reason(int sig, siginfo_t *info, void *secret)
{
	void *array[MAX_BACKTRACE_LEVEL];

	size_t size;

#ifdef PRINT_DEBUG
	char **strings;
	size_t i;

	size = backtrace(array, MAX_BACKTRACE_LEVEL);
	strings = backtrace_symbols(array, size);

	printf("### Obtain %zd stack frames.\n", size);

	for(i = 0; i < size; i++)
		printf("### %s\n", strings[i]);

	free(strings);

#else
	int fd = open(BACKTRACE_LOG_NAME, O_CREAT | O_WRONLY);

	size = backtrace(array, MAX_BACKTRACE_LEVEL);

	backtrace_symbols_fd(array, size, fd);

	close(fd);
#endif

	exit(0);
}

#endif

void die()
{
	char *str1;
	char *str2;
	char *str3;
	char *str4 = NULL;

	strcpy(str4, "ab");
}



void let_it_die()
{
	die();
}

int main(int argc, char **argv)
{
#ifdef BACKTRACE_OPEN
	struct sigaction act;

	act.sa_sigaction = show_reason;

	sigemptyset(&act.sa_mask);

	act.sa_flags = SA_RESTART | SA_SIGINFO;


	sigaction(SIGSEGV, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGFPE, &act, NULL);
	sigaction(SIGILL, &act, NULL);
	sigaction(SIGBUS, &act, NULL);
	sigaction(SIGABRT, &act, NULL);
	sigaction(SIGSYS, &act, NULL);
#endif

	let_it_die();

	return  0;
}
