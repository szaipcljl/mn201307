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
		fgets(mymsg.buf,sizeof(mymsg.buf),stdin);
		mymsg.type = TYPE;
		msgsnd(msgid,&mymsg,len,0);
	}
	return 0;
}
