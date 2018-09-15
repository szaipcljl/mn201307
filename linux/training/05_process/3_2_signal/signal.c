#include<stdio.h>
#include<signal.h>

void fun(int signo)
{
//	if(signo == SIGINT)
//		printf("abcd\n");
//	if(signo == SIGKILL)
//		printf("kill\n");
//	if(signo == SIGQUIT)
//		printf("1234\n");
//	if(signo == SIGUSR1)
//		printf("sigusr1\n");
}

int main(int argc, const char *argv[])
{
	signal(SIGINT,fun);
//	signal(SIGQUIT,fun);
	signal(SIGQUIT,SIG_IGN);

//	kill(getpid(),SIGKILL);

//	signal(SIGKILL,SIG_IGN);

//	kill(getpid(),SIGUSR1);
//	signal(SIGUSR1,fun);
	printf("hello\n");
	while(1)
	{
		pause();
	}
	return 0;
}
