#include<stdio.h>
#include<signal.h>

void fun(int signo)
{
	if(signo == SIGUSR1)
		printf("sig\n");
}

int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();

	if(pid == -1)
	{
	}
	else if(pid == 0)
	{
		kill(getppid(),SIGUSR1);
		while(1);
	}
	else
	{
		signal(SIGUSR1,fun);
		while(1);
	}
	return 0;
}
