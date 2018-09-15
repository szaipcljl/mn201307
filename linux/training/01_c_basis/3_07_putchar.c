#include <stdio.h>

int main(int argc, const char *argv[])
{
	int ch;
	char a = 'w';

	ch = putchar(97);
	printf("putchar(97) return:%d\n",ch);

	printf("putchar(10):\n");
	putchar(10);

	printf("putchar('a'):\n");
	putchar('a');

	printf("putchar(\'b\' + 1):\n");
	putchar('b' + 1);

	//	putchar(102409);
	printf("putchar(10):\n");
	putchar(10);

	printf("putchar(a + 1):\n");
	putchar(a + 1);

	printf("putchar(10):\n");
	putchar(10);

	return 0;
}
