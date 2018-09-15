#include <stdio.h>

#define M 3.0e-23
#define N 950

int main(int argc, const char *argv[])
{
	int n;
	double sum;

	printf("Please input n:");
	scanf("%d",&n);

	sum = n * N / M;

	printf("sum = %e\n",sum);

	return 0;
}
