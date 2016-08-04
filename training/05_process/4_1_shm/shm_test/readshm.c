#include<stdio.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<errno.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

typedef struct
{
	pid_t pid;
	char buf[64];
}MSG;

void fun(int sig)
{

}

int main(int argc, const char *argv[])
{
	key_t key;
	pid_t peerpid;
//	char buf[64];
	MSG *p;
	key = ftok(".",'a');

	signal(SIGUSR1,fun);
	int shmid;

	if((shmid = shmget(key,sizeof(MSG),0664 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		if(errno == EEXIST)//共享内存已经存在
		{
			shmid = shmget(key,sizeof(MSG),0664);
			p = (MSG *)shmat(shmid,NULL,0);

			peerpid = p->pid;
			p->pid = getpid();
			kill(peerpid,SIGUSR1);
		}
		else
		{
			perror("shmget");
			exit(1);
		}
	}
	else//当前进程创建并且打开了共享内存
	{
		p = (MSG *)shmat(shmid,NULL,0);

		//写自己进程号
		//挂起
		//读取2.c进程号
		//
		p->pid = getpid();
		pause();
		peerpid = p->pid;
	}

	while(1)
	{
		pause();

		if(strncmp(p->buf,"quit",4) == 0)
			break;
		printf("%s",p->buf);
		kill(peerpid,SIGUSR1);
	}
	shmdt(p);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
