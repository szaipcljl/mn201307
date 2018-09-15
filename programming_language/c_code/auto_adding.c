#include <stdio.h>

char *c = "hello";
char buf[6];

int main(int argc, const char *argv[])
{
	char *new_str = buf;
	while(*c) {
		printf("*c=%c\t", *c);

		*(new_str++) = *(c++);
		//*new_str++ = *c++;
	}
	buf[6] = '\0';

	printf("buf=%s\n", buf);

	return 0;
}

