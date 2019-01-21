#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, const char *argv[])
{
	int fd;

	if((fd = open("./1.txt",O_RDWR)) == -1)
	{
		perror("open");
		return -1;
	}

	off_t len_start,len_end,len;

	len_start = lseek(fd,0,SEEK_SET);
	lseek(fd,10,SEEK_SET);
	write(fd,"a",1);

	len_end = lseek(fd,0,SEEK_END);

	len = len_end - len_start;
	printf("%ld\n",len);

	return 0;
}
