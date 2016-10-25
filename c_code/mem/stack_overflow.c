#include <stdio.h>

/*
 * 让Linux的栈空间耗尽，然后出现core dumped，即栈溢出
 * */

void overFlow()
{
#if 1
	long i;
	printf("&i  : %p\n", &i);
	overFlow();
#else
	char i0[1024*1024*2] = {0};
	char i1[1024*1024*2] = {0};
	char i2[1024*1024*2] = {0};
	char i3[1024*1024*2] = {0};
	char i[1024*1024*2] = {0};
	printf("&i  : %p\n", &i);
#endif
}

int main(int argc, const char *argv[])
{
	overFlow();
	return 0;
}
