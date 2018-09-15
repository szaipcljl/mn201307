#include <stdio.h>
#include <stdlib.h>

#include "mytype.h"

typedef struct mstack{
	data_t *data;
	int top;
	int size;
}mstack_t;

mstack_t *creat_stack(int size)
{
	mstack_t *sp;

	sp = malloc(sizeof(mstack_t));
	sp->data = malloc(sizeof(data_t) * size);
	sp->top = -1;
	sp->size = size;

	return sp;
}


int push_stack(mstack_t *sp,const data_t *data)
{
	if(sp->size - 1 == sp->top)
		return -1;

	sp->data[++sp->top] = *data;

	return 0;
}

int pop_stack(mstack_t *sp,data_t *databuf)
{
	if(-1 == sp->top)
		return -1;

	*databuf = sp->data[sp->top --];

	return 0;
}

int empty_stack(mstack_t *sp)
{
	return -1 == sp->top;
}

int full_stack(mstack_t *sp)
{
	return sp->size - 1 == sp->top;
}

int clean_stack(mstack_t *sp)
{
	sp->top = -1;
	return 0;
}

int get_stack(mstack_t *sp,data_t *databuf)
{
	if(-1 == sp->top)
		return -1;
	*databuf = sp->data[sp->top];
	return 0;
}

int dis_stack(mstack_t **spp)
{
	free((*spp)->data);
	free(*spp);
	*spp = NULL;

	return 0;
}


void print_stack(mstack_t *sp)
{
	int i;

	for(i = 0;i <= sp->top;i ++){
		printf(" %d ",sp->data[i]);
	}
	printf("\n");

	return ;
}

#if 0

int main(int argc, const char *argv[])
{

	mstack_t *sp;
	int i;
	data_t buf;
	sp = creat_stack(6);

	for(i = 0 ; i < 10;i ++){
		push_stack(sp,&i);
	}

	print_stack(sp);

	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);
	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);
	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);

	print_stack(sp);
	return 0;
}

#endif
