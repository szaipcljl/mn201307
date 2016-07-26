#include <stdio.h>

int main(int argc, const char *argv[])
{
	int i;
	int t = 1;
	int sum = 0;

	for(i = 1;i < 4;i ++)
	{
		t = t * i;
		sum += t;
	}

	printf("%d\n",sum);

	return 0;
}
