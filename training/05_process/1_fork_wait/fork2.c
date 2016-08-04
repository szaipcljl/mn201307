#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();

	if(pid == -1){

	}else if(pid == 0){
		printf("child:pid:%d,PID:%d,PPID:%d\n",pid,getpid(),getppid());
	}else{
		sleep(1);
		printf("father:pid:%d,PID:%d,PPID:%d\n",pid,getpid(),getppid());
	}

	while(1);

	return 0;
}
