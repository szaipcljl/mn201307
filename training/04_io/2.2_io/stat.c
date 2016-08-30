#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc, const char *argv[])
{
	struct stat info;

	//	stat("./1.txt",&info);

	//	printf("st_mode:%o\n",info.st_mode);
#if 0
	int fd;

	fd = open("./2.txt",O_RDWR);
	fstat(fd,&info);
	printf("st_mode:%o\n",info.st_mode);
	printf("st_ino:%ld\n",info.st_ino);
#endif
	lstat("./2.txt",&info);
	printf("st_ino:%ld\n",info.st_ino);

	return 0;
}
