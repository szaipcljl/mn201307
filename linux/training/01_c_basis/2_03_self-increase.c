#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 10;
	int b = 20;
	float c = 1.1;

	//	10 ++; //不能对常量进行自增自减操作
	//	(a + b) ++; //不能对表达式自增自减操作

	c ++;

	printf("c = %f\n", c);
	//printf("c = %f, c ++ = %f\n", c, c ++); //与先算哪个有关

	return 0;
}
