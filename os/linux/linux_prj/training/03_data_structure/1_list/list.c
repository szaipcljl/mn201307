#include <stdio.h>
#include <stdlib.h>

#define N 8

typedef struct {
	int data[N];
	int last;
}list_t;

list_t *creat_list()
{
	list_t *lp;

	lp = malloc(sizeof(list_t));
	lp->last = -1;

	printf("[create list]\n");

	return lp;
}


/*
 * 功能：  向顺序表插入一个元素
 * 返回值：插入成功返回 0；失败返回 -1；
 * 参数：  1.容器的地址
 * 		   2.插入的下标位置
 * 		   3.插入的数据
 * */

int ins_list(list_t *lp, int index, int data)
{
	int i;

	if(lp->last >= N - 1) //不能插入错误检查(lp参数): 什么时候可以插入
		return -1;

	if(index < 0 || index > lp->last + 1) //(index参数): 什么位置可以插入
		return -1;


	//如何插入
	//1.将插入位置下面的元素依次向下移动
	//2.将要插入的数据写入到指定位置
	//3.last ++；

	for(i = lp->last; i >= index; i --){
		lp->data[i + 1] = lp->data[i];
	}

	lp->data[index] = data;
	lp->last ++;
	printf("[ins list]\n");


	return 0;
}

int del_list(list_t *lp,int index)
{
	int i;

	if(-1 == lp->last) //什么时候可以删除
		return -1;

	if(index < 0 || index > lp->last) //哪些位置可以删除
		return -1;

	for(i = index; i < lp->last; i ++){ //如何删除
		lp->data[i] = lp->data[i + 1];
	}
	lp->last --;
	printf("[del list]\n");

	return 0;
}

void print_list(list_t *lp)
{
	int i;

	for(i = 0;i <= lp->last;i ++){
		printf(" %d ",lp->data[i]);
	}

	printf("\n");
	printf("[print_list]: last = %d\n",lp->last);

	return ;
}

int main(int argc, const char *argv[])
{

	list_t *lp;
	int i;

	lp = creat_list();



	for(i = 7; i >= 0;i --){
		ins_list(lp,0,i);
	}
	print_list(lp);

	del_list(lp,5);
	print_list(lp);

	del_list(lp,7);
	print_list(lp);

	return 0;
}
