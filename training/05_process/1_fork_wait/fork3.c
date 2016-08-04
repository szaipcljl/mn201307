#include<stdio.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();

	if(pid == -1)
	{
		perror("fork");
		return -1;
	}

	else if(pid == 0)
	{
		pid_t pid1;

		pid1 = fork();

		if(pid1 == -1)
		{

		}
		else if(pid1 == 0)
		{
			printf("child son\n");
		}
		else 
		{
			printf("child\n");
		}
	}
	else 
	{
		printf("father\n");
	}
	while(1);
	return 0;
}
