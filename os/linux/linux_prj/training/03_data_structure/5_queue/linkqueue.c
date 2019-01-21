#include <stdio.h>
#include <stdlib.h>


typedef int data_t;
typedef struct linknode{
	data_t data;
	struct linknode *next;
}link_t;

typedef struct {
	link_t *head;
	link_t *tail;
}linkqueue_t;

linkqueue_t *creat_linkqueue()
{
	linkqueue_t *lqp;

	lqp = malloc(sizeof(linkqueue_t));
	lqp->head = malloc(sizeof(link_t));

	lqp->tail = lqp->head;
	lqp->head->next = NULL;

	return lqp;
}


int in_linkqueue(linkqueue_t * lqp,const data_t *data)
{
	link_t *newnode;

	newnode = malloc(sizeof(link_t));

	newnode->data = *data;
	newnode->next = NULL;//lqp->tail->next;

	lqp->tail->next = newnode;
	lqp->tail = newnode;

	return 0;
}

int out_linkqueue2(linkqueue_t *lqp,data_t *databuf)
{
	link_t *temp;

	if(NULL == lqp->head->next)
		return -1;

	*databuf = lqp->head->next->data;

	temp = lqp->head->next;
	lqp->head->next = temp->next;

	free(temp);

	if(temp == lqp->tail)
		lqp->tail = lqp->head;

	return 0;
}

int out_linkqueue(linkqueue_t *lqp,data_t *databuf)
{
	link_t *temp;
	if(NULL == lqp->head->next)
		return -1;

	if(NULL != databuf)
		*databuf = lqp->head->next->data;

	temp = lqp->head;
	lqp->head = lqp->head->next;

	free(temp);

	return 0;
}

int clean_linkqueue(linkqueue_t *lqp)
{

	while( !out_linkqueue(lqp,NULL))
		;

	return 0;
}


int get_head_linkqueue(linkqueue_t *lqp,data_t *databuf)
{
	if(NULL == lqp->head->next)
		return -1;
	*databuf = lqp->head->next->data;

	return 0;
}

int length_linkqueue(const linkqueue_t *lqp)
{
	link_t *head = lqp->head;

	int sum = 0;
	while(NULL != head->next){
		sum ++;
		head = head->next;
	}

	return sum;
}

int empty_linkqueue(linkqueue_t *lqp)
{
	return NULL == lqp->head->next;
}


int dis_linkqueue(linkqueue_t **lqpp)
{
	clean_linkqueue(*lqpp);
	free((*lqpp)->head);
	free(*lqpp);
	*lqpp = NULL;

	return 0;
}


void print_linkqueue(linkqueue_t *lqp)
{
	link_t *head;
	head = lqp->head;

	while(head = head->next){
		printf(" %d ",head->data);
	}
	printf("\n");

	return ;
}


int main(int argc, const char *argv[])
{
	linkqueue_t *lqp;
	int i;
	data_t buf;

	lqp = creat_linkqueue();

	for(i = 0;i < 10;i ++){
		in_linkqueue(lqp,&i);
	}
	print_linkqueue(lqp);

	for(i = 0; i < 12; i ++){
		buf = -1;
		out_linkqueue(lqp,&buf);
		printf("out: %d\n",buf);
	}
	print_linkqueue(lqp);

	for(i = 0; i < 10; i ++){
		in_linkqueue(lqp,&i);
	}
	print_linkqueue(lqp);

	return 0;
}
