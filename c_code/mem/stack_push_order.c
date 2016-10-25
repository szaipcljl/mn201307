#include <stdio.h>

/*
 * 局部变量入栈的顺序
 * 不同的平台有不同的顺序
 * */
int main()
{
	int a;
	char s1[1024] = {0};
	char s2[1024] = {0};
	char ch1;
	char ch2;
	char *p1, *p2;
	int b, c, d, e ,f;

	printf("&a: %p\n",&a);
	printf("&s1: %p\n",s1);
	printf("&s2: %p\n",s2);
	printf("&ch1: %p\n",&ch1);
	printf("&ch2: %p\n",&ch2);
	printf("&p1: %p\n",&p1);
	printf("&p2: %p\n",&p2);
	printf("&b: %p\n",&b);
	printf("&c: %p\n",&c);
	printf("&d: %p\n",&d);
	printf("&e: %p\n",&e);
	printf("&f: %p\n",&f);

	return 0;
}
