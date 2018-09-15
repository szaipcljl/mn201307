#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define TYPE 100

typedef struct 
{
	long type;
	char buf[64];
}MSG;

int main(int argc, const char *argv[])
{
	key_t key;

	key = ftok(".",'a');

	MSG mymsg;
	int msgid;

	int len = sizeof(MSG) - sizeof(long);
	msgid = msgget(key,0664 | IPC_CREAT);

	while(1)
	{
		msgrcv(msgid,&mymsg,len,TYPE,0);

		printf("read from snd.c %s",mymsg.buf);
	}
	return 0;
}
