#include<stdio.h>
#include<signal.h>

int main(int argc, const char *argv[])
{
#if 0
	kill(getpid(),SIGKILL);
	while(1);
#endif

	pid_t pid;

	pid = fork();

	if(pid == -1)
	{

	}

	else if(pid == 0)
	{
	//	kill(getppid(),SIGKILL);
		while(1);
	}

	else
	{
		kill(pid,SIGKILL);
		wait(NULL);
		printf("abcd\n");
		while(1);
	}
	return 0;
}
