/*
 * Generic GPIO beeper test app
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

int main(int argc, const char *argv[])
{
	int fd;
	int version;
	int ret;
	struct input_event ev;

	fd = open("/dev/input/event0", O_RDWR);
	if (fd < 0) {
		printf("open file failed\n");
		exit(1);
	}

	//ioctl(fd, EVIOCGVERSION, &version);
	//printf("evdev driver version is 0x%x: %d.%d.%d\n",
			//version, version>>16, (version>>8) & 0xff, version & 0xff);

	ev.type = EV_SND;
	ev.code = SND_BELL;
	ev.value = 0x1;
	printf("#mn-usr: type: 0x%x,code: 0x%d, value: 0x%d\n", ev.type, ev.code, ev.value);

	ret = write(fd, &ev, sizeof(struct input_event));
	if (ret < 0) {
		printf("1: write event error!\n");
		exit(1);
	}

	ev.value = 0x0;
	ret = write(fd, &ev, sizeof(struct input_event));
	if (ret < 0) {
		printf("2: write event error!\n");
		exit(1);
	}

	sleep(2);
	close(fd);

	return 0;
}
