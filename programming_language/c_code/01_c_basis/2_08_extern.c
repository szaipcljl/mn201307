#include <stdio.h>

int main(int argc, const char *argv[])
{

#if 1
	extern int a; //  引用本文中函数后面定义的全局变量，不加extern无法引用后面的a，执行到return 0;结束
#else
	int a;
#endif

	printf("a = %d\n",a);

	return 0;
}

int a = 10; //被引用
