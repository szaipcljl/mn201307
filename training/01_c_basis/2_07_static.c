#include <stdio.h>

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
