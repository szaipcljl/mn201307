#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>

pid_t pid;

void child_handler(int signo)
{
	FILE *fp;
	time_t t;
	struct tm *ptm;
	int ch;
	int count = 0;
	fp = fopen("log","a+");

	while((ch = fgetc(fp)) != EOF)
	{
		if(ch == '\n')
			count ++;
	}

	if(signo == SIGUSR1)
	{
		t = time(NULL);
		ptm = localtime(&t);

		fprintf(fp,"%d,%d;%d:%d\n",++count,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
		printf("%d,%d;%d:%d\n",count ++,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

		fflush(fp);
		sleep(1);
	}

	if(signo == SIGINT)
	{
		printf("count = %d\n",count);
	}

	if(signo == SIGUSR2)
	{
		printf("close\n");
		exit(0);
	}
}

void father_handler(int signo)
{
	if(signo == SIGTSTP)
	{
		kill(pid,SIGUSR2);
		wait(NULL);
		exit(0);

	}
}


int main(int argc, const char *argv[])
{
	char buf[100];
	pid = fork();

	if(pid == -1)
	{

	}

	else if(pid == 0)
	{

		signal(SIGUSR1,child_handler);
		signal(SIGUSR2,child_handler);
		signal(SIGINT,child_handler);
		signal(SIGTSTP,SIG_IGN);
	}

	else
	{
		signal(SIGTSTP,father_handler);
		signal(SIGINT,SIG_IGN);

		gets(buf);
		while(1)
		{
			if(strcmp(buf,"begin") == 0)
				kill(pid,SIGUSR1);
		}
	}

	while(1)
	{
		pause();
	}
	return 0;
}
