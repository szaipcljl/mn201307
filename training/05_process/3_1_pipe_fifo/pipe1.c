#include<stdio.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	int fd[2];
	int n;
	char buf[64];
	pipe(fd);

	pid_t pid;

	pid = fork();

	if(pid == -1)
	{

	}
	else if(pid == 0)
	{
		close(fd[0]);
	//	close(fd[1]);
		write(fd[1],"hello",5);
	}
	else
	{
		close(fd[1]);//读端

		n = read(fd[0],buf,sizeof(buf) - 1);

		printf("n = %d\n",n);
		buf[n] = '\0';
		printf("%s\n",buf);
	}
	return 0;
}

//读端固定：
//写端存在：如果有数据输入，写多少读多少，如果没有数据输入，阻塞
//写端不存在：读端直接返回0
