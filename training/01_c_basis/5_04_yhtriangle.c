#include <stdio.h>

/*
 * 杨辉三角
   1
   1 1
   1 2 1
   1 3 3 1
   */
int main(int argc, const char *argv[])
{
	int i,j;

	int a[10][10];


	for(i = 0;i < 10;i ++){
		a[i][0] = 1;
		a[i][i] = 1;
	}

	for(i = 2;i < 10;i ++){
		for(j = 1;j < i;j ++){
			a[i][j] = a[i - 1][j] + a[i - 1][j - 1];
		}
	}

	for(i = 0;i < 10;i ++){
		for(j = 0;j <= i;j ++){
			printf("%d\t",a[i][j]);
		}
		printf("\n");
	}

	return 0;
}
