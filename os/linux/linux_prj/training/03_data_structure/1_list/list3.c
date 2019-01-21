#include <stdio.h>
#include <stdlib.h>


typedef int data_t;

typedef struct {
	int last;
	int size;
	data_t data[0]; //利用数组越界不检查错误的特性
}list_t;

list_t *creat_list(int size)
{
	list_t *lp;

	lp = malloc(sizeof(list_t) + sizeof(data_t) * size);
	lp->last = -1;
	lp->size = size;

	return lp;
}


/*
 * 功能：  向顺序表插入一个元素
 * 返回值：插入成功返回 0；失败返回 -1；
 * 参数： 1.容器的地址
 * 		  2.插入的下标位置
 * 		  3.插入的数据
 * */

int ins_list(list_t *lp,int index,const data_t *data)
{
	int i;

	//什么时候可以插入
	if(lp->last >= lp->size - 1)
		return -1;
	
	//什么位置可以插入
	if(index < 0 || index > lp->last + 1)
		return -1;

	//如何插入
	//1.将插入位置下面的元素依次向下移动
	//2.将要插入的数据写入到指定位置
	//3.last ++；
	for(i = lp->last;i >= index;i --){
		lp->data[i + 1] = lp->data[i];
	}

	lp->data[index] = *data;
	lp->last ++;


	return 0;
}

int del_list(list_t *lp,int index,data_t *databuf)
{
	//什么时候可以删除
	//哪些位置可以删除
	//如何删除

	int i;

	if(-1 == lp->last)
		return -1;

	if(index < 0 || index > lp->last)
		return -1;

	if(NULL != databuf)
		*databuf = lp->data[index];

	for(i = index;i < lp->last; i ++){
		lp->data[i] = lp->data[i + 1];
	}
	lp->last --;

	return 0;
}


int clean_list(list_t *lp)
{
	lp->last = -1;
	return 0;
}

int length_list(list_t *lp)
{
	return lp->last + 1;
}

int empty_list(list_t *lp)
{
	return -1 == lp->last;
}

int full_list(list_t *lp)
{
	return lp->size - 1 == lp->last;
}

int change_list(list_t *lp,int index,const data_t *data)
{

	if(index < 0 || index > lp->last)
		return -1;

	lp->data[index] = *data;
	return 0;
}

int get_list(list_t *lp,int index,data_t *databuf)
{
	if(index < 0 || index > lp->last)
		return -1;

	*databuf = lp->data[index];

	return 0;
}

int locate_list(list_t *lp,const data_t *data)
{
	int i;

	for(i = 0;i <= lp->last; i ++){
		if(*data == lp->data[i])
			return i;
	}

	return -1;
}

int locate_del_list(list_t *lp,const data_t *data)
{

	del_list(lp,locate_list(lp,data),NULL);
	return 0;
}

int dis_list(list_t **lpp)
{
	free(*lpp);
	*lpp = NULL;

	return 0;
}

void print_list(list_t *lp)
{
	int i;
	
	for(i = 0;i <= lp->last;i ++){
		printf(" %d ",lp->data[i]);
	}
	printf("\n");
	printf("[print_list] last = %d\n",lp->last);

	return ;
}

int main(int argc, const char *argv[])
{

	list_t *lp;
	int i;
	data_t buf;

	lp = creat_list(4);

	for(i = 7; i >= 0;i --){
		ins_list(lp,0,&i);
	}
	print_list(lp);

	del_list(lp,5,&buf);
	print_list(lp);

	del_list(lp,7,&buf);
	print_list(lp);

	dis_list(&lp);

	return 0;
}
