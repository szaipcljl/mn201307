#include <stdio.h>
#include <stdlib.h>

typedef int data_t;
typedef struct {
	int front;
	int rear;
	int size;
	data_t data[0];
}queue_t;

queue_t *creat_queue(int size)
{
	queue_t *sqp;

	sqp = malloc(sizeof(queue_t) + sizeof(data_t) * size);

	sqp->size = size;
	sqp->front = 0;
	sqp->rear = 0;

	return sqp;
}

int in_queue(queue_t *sqp, const data_t *data)
{
	if((sqp->rear + 1 ) % sqp->size == sqp->front)
		return -1;

	sqp->data[sqp->rear] = *data;

	sqp->rear = (sqp->rear + 1) % sqp->size;

	return 0;
}

int out_queue(queue_t *sqp,data_t *databuf)
{
	if(sqp->front == sqp->rear)
		return -1;

	*databuf = sqp->data[sqp->front];
	sqp->front = (sqp->front + 1) % sqp->size;

	return 0;
}

int get_front_queue(queue_t *sqp,data_t *databuf)
{
	if(sqp->front == sqp->rear)
		return -1;

	*databuf = sqp->data[sqp->front];

	return 0;
}

int empty_queue(queue_t *sqp)
{
	return (sqp->rear == sqp->front);
}

int full_queue(queue_t *sqp)
{
	return ((sqp->rear + 1) % sqp->size == sqp->front);
}

int length_queue(queue_t *sqp)
{
	return (sqp->rear + sqp->size - sqp->front ) % sqp->size;
}

int clean_queue(queue_t *sqp)
{
	sqp->front  = sqp->rear = 0;
	return 0;
}

int dis_queue(queue_t **sqpp)
{
	free(*sqpp);
	*sqpp = NULL;

	return 0;
}

void print_queue(queue_t *sqp)
{
	int front = sqp->front;

	while(front != sqp->rear){
		printf(" %d ",sqp->data[front]);
		front = (front + 1) % sqp->size;
	}

	printf("\n");
}

int main(int argc, const char *argv[])
{
	queue_t *sqp;
	int i;
	data_t buf;

	sqp = creat_queue(8);

	for(i = 0;i < 10;i ++){
		in_queue(sqp,&i);
	}
	print_queue(sqp);

	for(i = 0 ;i < 4;i ++){
		buf = -1;
		out_queue(sqp,&buf);
		printf("out : %d\n",buf);
	}
	print_queue(sqp);

	for(i = 11;i < 20;i ++){
		in_queue(sqp,&i);
	}
	print_queue(sqp);

	for(i = 0 ;i < 8;i ++){
		buf = -1;
		out_queue(sqp,&buf);
		printf("out : %d\n",buf);
	}
	print_queue(sqp);

	return 0;
}
