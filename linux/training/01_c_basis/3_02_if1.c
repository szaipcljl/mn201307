#include <stdio.h>

/*
  y = 2 * x - 1,  x > 10
  y = 2 * x + 1,  0 < x <= 10
  y = 0,   其他
  */

int main(int argc, const char *argv[])
{
	int x,y;

	printf("input x:");
	scanf("%d",&x);

	if(x > 10){
		y = 2 * x - 1;
	}else if(x > 0 && x <= 10){
		y = 2 * x + 1;
	}
	else
		y = 0;

	printf("%d\n",y);

	return 0;
}
