#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	pid_t pid;
	int i = 10;

	pid = fork();
	//On  success, the PID of the child process is returned in the parent, and 0 is returned in the child.

	printf("pid = %d\n",pid);
	if(pid == -1){
		perror("fork");
		return -1;
	}else if(pid == 0){
		printf("child\n");
		i ++;
		printf("i = %d\n",i);
		while(1);
	}else{
		sleep(1);
		printf("father\n");
		i ++;
		printf("i = %d\n",i);
		while(1);
	}

	return 0;
}
