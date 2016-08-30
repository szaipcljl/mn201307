#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>

int main(int argc, const char *argv[])
{
	int fd;
	char buf[10];
#if 0
	if((fd = open("1.txt",O_RDONLY)) == -1)
	{
		perror("open");
		return -1;
	}
#endif
#if 1
	if((fd = open("./1.txt",O_RDWR | O_CREAT | O_APPEND,0664)) == -1)
	{
		perror("open");
		return -1;
	}
#endif
#if 0
	if((fd = open("./1.txt",O_RDWR | O_CREAT | O_TRUNC | O_EXCL,0664)) == -1)
	{
		perror("open");
		printf("errno = %d\n",errno);
		return -1;
	}
#endif
#if 0
	ssize_t n;

	n = read(fd,buf,sizeof(buf));
	printf("%d\n",n);

	buf[n] = '\0';
	printf("%s\n",buf);
#endif

	//	write(fd,"abc",3);
#if 0
	while((n = read(fd,buf,sizeof(buf))) > 0)
	{
		write(fd1,buf,n);
	}
#endif
	close(fd);
	return 0;
}
