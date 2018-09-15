#include<stdio.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int main(int argc, const char *argv[])
{
	key_t key;

	char buf[64];
	char *p;
	key = ftok(".",'a');

	int shmid;

	shmid = shmget(key,sizeof(buf),0664 | IPC_CREAT);

	p = (char *)shmat(shmid,NULL,0);

//	fgets(p,10,stdin);

	printf("%s",p);
	return 0;
}
