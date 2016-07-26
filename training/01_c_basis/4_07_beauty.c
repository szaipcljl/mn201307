#include <stdio.h>

int main(int argc, const char *argv[])
{
	int num;
	int i;
	int sum = 0;

	for(num = 1;num <= 1000;num ++){
		sum = 0;

		for(i = 1;i <= num / 2; i ++){
			if(num % i == 0){
				sum += i;
			}
		}

		if(sum == num)
			printf("%d\n",num);
	}

	return 0;
}
