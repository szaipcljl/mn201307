#include <stdio.h>

/*
 * 存储模式:
 *
 大端模式： (低地址存放高字节) 数据的高字节保存在内存的低地址中，低字节保存在内存的高地址中，
 --类似于把数据当作字符串顺序处理：地址由小向大增加，而数据从高位往低位放；这和我们的阅读习惯一致。

 小端模式： (低地址存放低字节-双低) 数据的高字节保存在内存的高地址中，低字节保存在内存的低地址中，
 --将地址的高低和数据位权有效地结合起来，高地址部分权值高，低地址部分权值低。
 */

int main(int argc, const char *argv[])
{
#if 0
	int a;
	int *p;
	char *cp;

	p = &a;
	cp = (char *)&a; //低地址单元
	a = 0x12345678;

	printf("*p = %x\n",*p);
	printf("*cp = %x\n",*cp);

	if(0x78 == *cp)
		printf("little endian\n");
	else
		printf("big endian\n");

#else
	//测试编译器是大端模式还是小端模式：

	short int x;
	char x0,x1;

	x = 0x1122;

	x0 = ((char *)&x)[0]; //低地址单元
	x1 = ((char *)&x)[1]; //高地址单元

	printf("x = %x\n",x);
	printf("x0 = %x\nx1 = %x\n",x0,x1);
	if(0x22 == x0)
		printf("little endian\n");
	else
		printf("big endian\n");
#endif

	return 0;
}
