#include <stdio.h>

/*冒泡排序*/

#define N 5

int main(int argc, const char *argv[])
{
	int a[N] = {123,5,9,18,6};

	int i,j;

	for(i = 0;i < sizeof(a) / sizeof(a[0]) - 1;i ++){
		for(j = 0;j < sizeof(a) / sizeof(a[0]) - 1 - i;j ++){
			if(a[j] > a[j + 1]){
				a[j] ^= a[j + 1];
				a[j + 1] ^= a[j];
				a[j] ^= a[j + 1];
			}
		}
	}

	for(i = 0;i < sizeof(a) / sizeof(a[0]);i ++){
		printf("%d\n",a[i]);
	}

	return 0;
}
