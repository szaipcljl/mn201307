#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a;

	printf("input a number:\n");
	scanf("%d",&a);

	switch(a)
	{
	case 1:
		printf("a = 1\n");
		break;
	case 2:
		printf("a = 2\n");
		break;
	default:
		printf("other\n");
		break;

	}
	return 0;
}
