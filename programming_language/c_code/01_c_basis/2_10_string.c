#include <stdio.h>

int main(int argc, const char *argv[])
{

#if 1
	char str[] = "hello";

	*(str + 1) = 'w';
	printf("%s\n",str);
#else

	char *p = "hello";

	*(p + 1) = 'w'; //Segmentation fault 
	printf("%s\n",p);

#endif

	return 0;
}
