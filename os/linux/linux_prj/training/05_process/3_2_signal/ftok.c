#include<stdio.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<sys/types.h>
int main(int argc, const char *argv[])
{
	key_t key;

	key = ftok("./ftok.c",1);

	printf("%#x\n",key);

	struct stat info;
	stat("./ftok.c",&info);

	printf("st_dev:%lx,st_ino:%lx\n",(unsigned long)info.st_dev,(unsigned long)info.st_ino);
	return 0;
}
