#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const char *pathname = "/sys/bus/hdaudio/devices/hdaudioC0D0/type";

int main(int argc, const char *argv[])
{
	int fd;
	int ret;
	char buf[100];
	memset(buf, 0, 100);


	fd = open(pathname, 0664);
	if (fd < 0) {
		printf("failed to open %s\n", pathname);
		return -1;
	}

	ret = read(fd, buf, 50);
	if (ret < 0) {
		printf("failed to read\n");
		close(fd);
		return -1;
	}

	printf("%s\n", buf);
	return 0;
}
