#include <stdio.h>

/* 1.函数调用之前一定要声明
 * 2.当函数没有传参时，void 不可省略
 * 3.返回值类型不要大于 int ，以免影响效率
 * 4.传参总个数及总体积均不能大于 4个int 类型
 *   在arm下 前四个参数会在寄存器中传输, 超过四个要使用堆栈空间，产生多次拷贝 影响效率
 *   arm 返回值 默认用 r0寄存器 传输，
 *   超过一个 int 大小时 可能使用两个寄存器，或直接使用堆栈
 * 5.当传参个数大于 4个 或体积较大时 ，考虑用指针传参
 *
 * */

int add(int a,int b);

int add(int a,int b)
{
	int c;
	c = a+ b;
	return c;
}


void fun(int a)
{
	a = a + 1;
	return ;
}

void fun2() //不定参数  ，不要去使用
{
	return ;
}


void fun3(void)
{
	return ;
}


int main(int argc, const char *argv[])
{
	int a;

	a = add(2,3);
	printf("ans = %d\n",a);

	fun3();
	fun2();
	fun2(2,2,2);

	return 0;
}

