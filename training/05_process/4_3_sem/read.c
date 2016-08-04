#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/shm.h>

#define READ 0  //代表某种资源
#define WRITE 1 // 代表另一种资源
union semun
{
	int val;
};

int main(int argc, const char *argv[])
{
	key_t key;
	int semid,shmid;
	union semun myun;
	char *p;
	struct sembuf sem;
	if((key = ftok(".",'a')) < 0)
	{
		perror("fail to ftok");
		return -1;
	}
	if((semid = semget(key,2,IPC_CREAT | IPC_EXCL| 0666)) < 0)
	{
		if(errno == EEXIST)
		{
			semid = semget(key,2,0666); //2代表有两种资源
		}
		else
		{
			perror("fail to semget");
			return -1;
		}
	}
	else
	{
		myun.val = 0;//初始化资源个数
		semctl(semid,READ,SETVAL,myun); //类似于sem_init

		myun.val = 1;//初始化另一个资源个数
		semctl(semid,WRITE,SETVAL,myun);//初始化1编号资源个数为1个
	}

	if((shmid = shmget(key,1024,IPC_CREAT | 0666)) < 0)
	{
		perror("fail to shmget");
		return -1;
	}

	p = (char *)shmat(shmid,NULL,0);

	while(1)
	{
		//p操作（操作的资源是编号0的资源）p(READ)
		sem.sem_num = READ;
		sem.sem_op = -1;
		sem.sem_flg = 0;


		semop(semid,&sem,1);//操作的是0号资源的个数
		printf("read from shm:%s",p);
		if(strncmp(p,"quit",4) == 0)
			break;

		//v(WRITE);
		sem.sem_num = WRITE;
		sem.sem_op = 1;
		sem.sem_flg = 0;

		semop(semid,&sem,1);
	}
	shmdt(p);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,2,IPC_RMID);

	return 0;
}
