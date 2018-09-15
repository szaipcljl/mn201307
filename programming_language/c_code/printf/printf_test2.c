#include <stdio.h>
#include <unistd.h> //usleep, sleep

int main(int argc, const char *argv[])
{
	int i = 5;

	while (i--) {
		usleep(500 * 1000);
		printf("\e[23D %02x", i);
		fflush(stdout);
	}
	printf("\e[23D");

	return 0;
}
