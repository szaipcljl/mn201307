#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, const char *argv[])
{
	int fd;
	int n;
	char buf[64];

	//	mkfifo("./fifo",0664);

	if (mkfifo("./fifo", 0664)  == -1) {
		if(errno == EEXIST) {//有名管道已经被创建，这里直接打开有名管道

			fd = open("./fifo",O_RDONLY);
		} else {
			perror("mkfifo");
			exit(1);
		}
	} else {//当前进程创建的有名管道

		fd = open("./fifo",O_RDONLY);
	}
	//	fd = open("./fifo",O_RDONLY);

	n = read(fd,buf,sizeof(buf) - 1);
	buf[n] = '\0';

	printf("%s\n",buf);
	return 0;
}
