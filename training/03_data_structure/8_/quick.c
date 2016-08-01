#include <stdio.h>

void print_data(int *data)
{
	int i;

	for(i = 0;i < 8;i++){
		printf(" %d ",data[i]);
	}
	printf("\n");

	return ;
}

/*由小到大排序
*/
int quick_once(int *a,int head,int tail)
{
	int key;

	key = a[head];  //
	while(head < tail){
		while(a[tail] >= key && head < tail) //从尾部开始找到小的或
			//tail指到head退出循环
			tail --;
		a[head] = a[tail];

		while(a[head] <= key && head < tail) //从头部开始找到大的或
			//没有退出
			head ++;
		a[tail] = a[head];
	}

	a[tail] = key;

	return head;
}


void quick(int *a,int head,int tail)
{
	int mid;

	if(head >= tail)  //头和尾指向同一位置时，不再排序
		return ;

	//1.进行一次排序，确定一个值的位置
	mid = quick_once(a,head,tail);
	print_data(a);

	//2.拆分无序子集合
	//3.排序两个无序子集合
	quick(a,head,mid - 1);
	quick(a,mid + 1,tail);

	return ;
}

int main(int argc, const char *argv[])
{
	int a[] = {5,6,3,8,2,9,1,7};

	print_data(a);

	quick(a,0,sizeof(a) / sizeof(int) - 1);

	print_data(a);

	return 0;
}
