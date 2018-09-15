#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a;
	char b;
	char c;
	char d = '\n';

	printf("input a:\n");
	scanf("%d",&a);

	printf("input:\n");
	b = getchar(); //上一个输入的回车
	scanf("%c",&c);
	
	printf("a = %d\n", a);
	printf("d = %d, d = %c\n", d,d);
	printf("b = %c\n", b);
	printf("c = %d\n", c);

	return 0;
}
