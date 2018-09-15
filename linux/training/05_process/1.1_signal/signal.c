#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void f()
{
	printf("THIS MESSAGE WAS SENT BY PARENT PROCESS..\n");
}

int main(int argc, const char *argv[])
{
	int i,childid,status=1,c=1;
	signal(SIGUSR1,f); //setup the signal value

	i = fork(); //better if it was: while((i=fork)==-1);
	if (i) {
		printf("Parent: This is the PARENT ID == %d\n",getpid());
		sleep(3);
		printf("Parent: Sending signal..\n");

		kill(i, SIGUSR1); //send the signal

		//status is the return code of the child process
		wait(&status);
		printf("Parent is over..status == %d\n",status);

		//WIFEXITED return non-zero if child exited normally 
		printf("Parent: WIFEXITED(status) == %d\n", WIFEXITED(status));

		//WEXITSTATUS get the return code
		printf("Parent: The return code WEXITSTATUS(status) == %d\n", WEXITSTATUS(status));

	} else {
		printf("Child: This is the CHILD ID == %d\n", getpid());
		while (c<5) {
			sleep(1);
			printf("CHLID TIMER: %d\n",c);
			c++;
		}
		printf("Child is over..\n");
		exit(2);
	}
}

