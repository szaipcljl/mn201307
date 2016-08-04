#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	printf("hello\n");
	printf("linux ");

#if 0
	exit(0);
#else
	_exit(3); //不打印"linux "
#endif
	printf("world\n");
}
