#include <stdio.h>

/*剔除100~200之间能被3整除的数字，其余打印出来*/

int main(int argc, const char *argv[])
{
	int n = 100;
	int m;

#if 1
	for(n = 100;n <= 200;n ++){
		if(n % 3 == 0)
			continue;

		printf("%d\n",n);
	}
#endif

	while(n <= 200){
		m = n ++;
		if(m % 3 == 0)
			continue;
		printf("%d\n",m);
	}

	return 0;
}
