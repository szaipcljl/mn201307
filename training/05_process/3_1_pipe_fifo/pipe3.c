#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int fd[2];
	int status;
	pipe(fd);

	pid_t pid;

	pid = fork();

	if(pid == -1) {

	}
	else if(pid == 0) {
		close(fd[0]);

		write(fd[1],"hello",5);
		while(1);
	} else {
		close(fd[0]);
		close(fd[1]);
		wait(&status);
		printf("%x\n",status);
	}
	return 0;
}

//写端存在：
//如果端不存在:发送信号SIGPIPE给子进程
