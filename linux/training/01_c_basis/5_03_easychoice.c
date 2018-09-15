#include <stdio.h>

/*简单选择*/
int main(int argc, const char *argv[])
{
	int a[5] = {123,5,9,18,6};

	int i,j;
	int min;
	int index,t;

	for(i = 0;i < 4;i ++){
		min = a[i];
		index = i;

		for(j = i + 1;j < 5;j ++){
			if(a[j] < min){
				min = a[j];
				index = j;
			}
		}

		t = a[i];
		a[i] = a[index];
		a[index] = t;

	}

	for(i = 0;i < 5;i ++){
		printf("%d\n",a[i]);
	}

	return 0;
}
