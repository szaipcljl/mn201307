#include <stdio.h>
/*
 * input:F
 * output:
 * F
 * FE
 * FED
 * FEDC
 * FEDCB
 * FEDCBA
 * */

int main(int argc, const char *argv[])
{
	char ch;

	printf("input a capital letter:\n");
	ch = getchar();
	if(ch < 'A' || ch > 'Z'){
		printf("error:please input a capital letter\n");
		return -1;
	}

	int i,j;

	for(i = 1;i <= ch - 'A' + 1;i ++)
	{
		for(j = i;j >= 1;j --)
		{
			putchar(ch - (i - j));
		}

		putchar(10);
	}
	return 0;
}
