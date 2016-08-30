#include <stdio.h>

int main(int argc, const char *argv[])
{
	int count;

	for(count = 0;;count ++){
		printf("%0128d",count); //128个字节1k,缓存满打印
		usleep(100000);
	}
	return 0;
}
