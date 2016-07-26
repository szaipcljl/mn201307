#include <stdio.h>

int main(int argc, const char *argv[])
{
	int score;

	printf("input scord(0~100)\n");
	if(scanf("%d",&score) != 1){
		printf("error\n");
		return -1;
	}

	if(score < 0 || score > 100){
		printf("error1: the score is higher or lower than the specified scope\n");
		return -1;
	}

	if(score >= 90 && score <= 100)
		puts("A");
	else if(score >= 80 && score < 90)
		puts("B");
	else if(score >= 70 && score < 80)
		puts("C");
	else if(score >= 60 && score < 70)
		puts("D");
	else
		puts("NO PASS");

	return 0;
}
