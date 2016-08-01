#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a[4];
	int b[0];  //数组大小为零，不分配存储空间

	printf("sizeof(a): %ld\n",sizeof(a));
	printf("sizeof(b): %ld\n",sizeof(b));
	printf("b addr: %p\n",b);

	return 0;
}


