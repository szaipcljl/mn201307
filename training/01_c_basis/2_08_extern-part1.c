#include <stdio.h>

extern int b;//引用外部文件全局变量
//与3_04_extern-part2-static.c一起编译实验

int main(int argc, const char *argv[])
{
	printf("b = %d\n", b);

	return 0;
}
