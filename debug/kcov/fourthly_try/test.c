#include <stdio.h>
#include <string.h>
#include "trace.h"

static int fun2(void)
{
	printf("fun2\n");
	return 0;
}

static void fun1(int a)
{
	fun2();
	printf("fun1: a[%d]\n", a);
}

int main(void)
{
	int a = 10;
	fun1(a);
	return 0;
}
