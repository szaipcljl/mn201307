#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, const char *argv[])
{

	int fd = open("/dev/xc7027_roi", O_RDWR);
	if (fd < 0) {
		printf("Open /dev/xc7027_roi failed\n");
		return -1;
	}
	printf("video device fd : %d\n",fd);

	return 0;
}
