#include <stdio.h>

int main(int argc, const char *argv[])
{
	int i = 10;
	int sum = 0;

	do{
		sum += i;
		i ++;
	}while(i <= -1);//至少执行一次
	printf("%d\n",sum);

	return 0;
}
