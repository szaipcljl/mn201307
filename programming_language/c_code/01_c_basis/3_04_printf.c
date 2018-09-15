#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 171;
	unsigned int b = 10;

	char c = 'y';
	char *s = "hello world";

	float d = 1.1;
	float f = 12345678.123;

	printf("a = %d\n",a);
	printf("a = %i\n",a);
	printf("a = %X\n",a);
	printf("a = %o\n",a);

	printf("b = %u\n",b);
	printf("c = %c\n",c);

	printf("s: %s\n",s);

	printf("d = %f\n",d);

	printf("f = %e\n",f);
	printf("f = %E\n",f);

	printf("&a: %p\n",&a);

	return 0;
}
