#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, const char *argv[])
{
	int pipefd[2];
	int n;
	char buf[64];
	pipe(pipefd); //see Linux Programmer's Manual

	pid_t pid;

	pid = fork();

	if(pid == -1) {

	} else if (pid == 0) {//读端/* Child reads from pipe */
		close(pipefd[1]);	/* Close unused write end */

		n = read(pipefd[0],buf,sizeof(buf) - 1);

		printf("n = %d\n",n);
		buf[n] = '\0';
		printf("%s\n",buf);

		close(pipefd[0]);
		_exit(EXIT_SUCCESS);
	} else { //写端pipefd[1]/* Parent writes to pipe */
		close(pipefd[0]);	/* Close unused read end */
		//close(pipefd[1]);
		write(pipefd[1],"hello",5);

		close(pipefd[1]);	  /* Reader will see EOF */

		wait(NULL);		  /* Wait for child */
		exit(EXIT_SUCCESS);
	}

	return 0;
}

//读端存在：
//写端存在：如果写端有数据输入，写多少读端读多少; 如果写端没有数据输入，读端阻塞
//写端不存在：读端直接返回0
