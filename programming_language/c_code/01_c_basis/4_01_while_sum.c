#include <stdio.h>

/* 1+2+3+...+10 */

int main(int argc, const char *argv[])
{
	int i = 10;
	int sum = 0;

	while(i > 0){ //while(1)
		sum += i;
		i --;
	}

	printf("sum = %d\n",sum);

	return 0;
}
