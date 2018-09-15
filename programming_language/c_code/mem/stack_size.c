#include <stdio.h>

/**
 * 栈地址是从高到低，所以用第一个减去最后一个地址，得到栈空间(8M)
 * 使用ulimit -s可得到8192, 与程序结果一致
 */

int main(int argc, const char *argv[])
{
	long addr_start = 0x7fff88b6e758;
	long addr_end = 0x7fff88370798;

	float stack_size = (float)(addr_start - addr_end)*1.0/1024/1024;
	printf("stack size is %f M\n", stack_size);

	return 0;
}


