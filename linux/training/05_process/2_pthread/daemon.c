#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	pid_t pid;
	int fdmax;
	int fd;

	pid = fork();

	if(pid == -1){

	}else if(pid == 0){
		setsid();//创建会话，如果当前进程是会话组长，调用失败
		chdir("/");
		umask(0);

		fdmax = getdtablesize();

		for(fd = 0;fd < fdmax;fd ++)
			close(fd);

		while(1);
	}else{
		exit(0);
	}

	return 0;
}
