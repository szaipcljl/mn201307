#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

pid_t pid;

void child_handler(int signo)
{

	if(signo == SIGINT)
	{
		kill(getppid(),SIGUSR1);
	}

	if(signo == SIGQUIT)
	{
		kill(getppid(),SIGUSR2);
	}
	if(signo == SIGUSR1)
	{
		printf("please get off the bus\n");
		exit(0);
	}
}

void father_handler(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("let`s gogogo\n");
	}
	if(signo == SIGUSR2)
	{
		printf("stop the bus\n");
	}
	if(signo == SIGTSTP)
	{
		kill(pid,SIGUSR1);
		wait(NULL);
		exit(0);
	}

}

int main(int argc, const char *argv[])
{
	//	pid_t pid;
	pid = fork();
	char buf[64];
	if(pid == -1)
	{

	}
	else if(pid == 0)
	{
		signal(SIGTSTP,SIG_IGN);
		signal(SIGINT,child_handler);
		signal(SIGQUIT,child_handler);
		signal(SIGUSR1,child_handler);
	}
	else
	{
		signal(SIGINT,SIG_IGN);
		signal(SIGQUIT,SIG_IGN);
		signal(SIGUSR1,father_handler);
		signal(SIGUSR2,father_handler);
		signal(SIGTSTP,father_handler);
	}

	while(1)
		pause();
	return 0;
}
