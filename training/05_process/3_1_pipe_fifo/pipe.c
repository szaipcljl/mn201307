#include<stdio.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	int fd[2];
	int count = 0;
	pipe(fd);

//	close(fd[0]);
	while(1)
	{
		write(fd[1],"a",1);

		count ++;

		printf("%dbytes\n",count);
	}
	return 0;
}
