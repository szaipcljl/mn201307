#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>


#define TYPEA 100
#define TYPEB 200

typedef struct 
{
	long type;
	char buf[64];
}MSG;

void fun(int signo)
{
	if(signo == SIGUSR1)
		exit(0);
}

int main(int argc, const char *argv[])
{
	key_t key;
	
	pid_t pid;
	key = ftok(".",'a');
	
	MSG mymsg;
	int msgid;
	
	int len = sizeof(MSG) - sizeof(long);
	msgid = msgget(key,0664 | IPC_CREAT);
	
	signal(SIGUSR1,fun);

	pid = fork();

	if(pid == -1)
	{

	}
	else if(pid == 0)//发200
	{
		while(1)
		{
			fgets(mymsg.buf,sizeof(mymsg.buf),stdin);
			mymsg.type = TYPEB;

			msgsnd(msgid,&mymsg,len,0);
			if(strncmp(mymsg.buf,"quit",4) == 0)
			{
				kill(getppid(),SIGUSR1);
				exit(0);
			}
		}
	}
	else //收100
	{
		while(1)
		{
			msgrcv(msgid,&mymsg,len,TYPEA,0);

			printf("read from clienta.c %s",mymsg.buf);

			if(strncmp(mymsg.buf,"quit",4) == 0)
			{
				kill(pid,SIGUSR1);
				wait(NULL);

				msgctl(msgid,IPC_RMID,NULL);
				exit(0);
			}
		}
	}
	return 0;
}
