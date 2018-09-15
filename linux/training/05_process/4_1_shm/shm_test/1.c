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
	}

	while(1)
	{

		fgets(p->buf,10,stdin);
		//写共享内存
		//发信号给对方
		//挂起
	}
	return 0;
}
