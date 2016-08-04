#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();

	if(pid == -1){

	}else if(pid == 0){
		printf("%d\n",getpid());
		printf("child1\n");
	}else{
		pid_t pid1;
		pid1 = fork();

		if(pid1 == -1){

		}else if(pid1 == 0){
			printf("child2\n");
		}else{
//			waitpid(-1,NULL,0);
//			waitpid(pid1,NULL,0);

			waitpid(pid1,NULL,WNOHANG);
			printf("father\n");
		}
	}

	while(1);

	return 0;
}
