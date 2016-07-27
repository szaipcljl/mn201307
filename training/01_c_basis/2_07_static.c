#include <stdio.h>

/*
 * static 作用:
   1.限制存储域：不存储在栈区，存储于静态存储区
   static int i;

   2.限制作用域：只能在本文件中使用
   static fun( )
   */

void fun()
{
	static int a = 10,b = 20,c = 0;

	c = c + a + b;
	printf("c = %d\n",c);
}

int main(int argc, const char *argv[])
{
	fun();
	fun();
	fun();

	return 0;
}
