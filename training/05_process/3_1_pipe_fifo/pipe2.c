#include<stdio.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	int fd[2];
	char buf[1024 * 8];
	pipe(fd);

	pid_t pid;

	pid = fork();

	if(pid == -1)
	{

	}
	else if(pid == 0)
	{
		close(fd[1]);

		while(1);
	//	while(1)
	//	{
	//		read(fd[0],buf,sizeof(buf);
	//	}
	}
	else
	{
		close(fd[0]);//写端

		while(1)
		{
			write(fd[1],buf,sizeof(buf));
			printf("write the data %d\n",sizeof(buf));
		}

	}
	return 0;
}

//写端固定：
//读端存在：如果管道有空间，一直输入数据，如果管道没有空间阻塞
