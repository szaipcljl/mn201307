#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a;
	int b;
	int c;
	int n;

	printf("input 3 number:\n");

	if((n = scanf("%d%d%d",&a,&b,&c)) != 3)
	{
		printf("error: n = %d\n",n);
		return -1;
	}

	printf("n = %d\n",n);

	return 0;
}
