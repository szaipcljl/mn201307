#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
int main(int argc, const char *argv[])
{
	int fd;

//	mkfifo("./fifo",0664);

	if (mkfifo("./fifo",0664) == -1) {
		if (errno == EEXIST)	{
			fd = open("./fifo",O_WRONLY);
		} else {
			perror("open");
			exit(1);
		}
	} else {
		fd = open("./fifo",O_WRONLY);
	}
//	fd = open("./fifo",O_RDWR);

	write(fd,"hello",5);
	close(fd);
	return 0;
}
