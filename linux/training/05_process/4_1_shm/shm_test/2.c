#include<stdio.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<errno.h>
#include<stdlib.h>
typedef struct
{
	pid_t pid;
	char buf[64];
}MSG;

int main(int argc, const char *argv[])
{
	key_t key;

//	char buf[64];
	MSG *p;
	key = ftok(".",'a');

	int shmid;

	if((shmid = shmget(key,sizeof(MSG),0664 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		if(errno == EEXIST)//共享内存已经存在
		{
			shmid = shmget(key,sizeof(MSG),0664);
			p = (MSG *)shmat(shmid,NULL,0);

			//读1.c进程号
			//写自己的进程号
			//发信号给1.c
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
	}

	while(1)
	{
		//挂起
		//唤醒后读共享内存
		//发信号给对方
		printf("%s",p->buf);

	}
	return 0;
}
