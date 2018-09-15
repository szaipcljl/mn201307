#include <stdio.h>

int main(int argc, const char *argv[])
{
	int i,j;
	int sum = 0;

	for(j = 0;j < 100;j ++)
	{
		for(i = 0;i < 100;i ++)
		{
			sum += i + j;

			if(sum > 50)
#if 1
				break; //break 只能跳出1层循环
#else
			goto loop; //内层循环直接跳出用goto
#endif
		}
		//printf("*******\n");
		//break;
	}

loop:
	printf("sum = %d\n",sum);
	return 0;
}
