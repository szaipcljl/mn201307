#include <stdio.h>
#include <stdlib.h>


typedef int data_t;
typedef struct node{
	data_t data;
	struct node *next;
}linkstack_t;

linkstack_t *creat_linkstack()
{
	linkstack_t *head;

	head = malloc(sizeof(linkstack_t));
	head->next = NULL;

	return head;
}

int push_linkstack(linkstack_t *lsp,const data_t *data)
{
	linkstack_t *newnode;

	newnode = malloc(sizeof(linkstack_t));
	newnode->data = *data; //初始化新节点数据域
	newnode->next = lsp->next; //初始化新节点指针域
	lsp->next = newnode;

	return 0;
}

int pop_linkstack(linkstack_t *lsp,data_t *databuf)
{
	linkstack_t *temp;

	if(NULL == lsp || NULL == lsp->next)
		return -1;

	*databuf = lsp->next->data;
	temp = lsp->next;
	lsp->next = temp->next;

	free(temp);

	return 0;
}


void print_linkstack(const linkstack_t *head)
{
	while(NULL != head->next){
		head = head->next;
		printf(" %d ",head->data);
	}
	printf("\n");
}


int clean_linkstack(linkstack_t *lsp)
{

}

int empty_linkstack(linkstack_t *lsp)
{

}

int get_top_linkstack(linkstack_t *lsp,data_t *databuf)
{

}

int length_linkstack(linkstack_t *lsp)
{

}

int dis_linkstack(linkstack_t **lspp)
{

}


int main(int argc, const char *argv[])
{
	linkstack_t *lsp;
	int i;
	data_t buf;

	lsp = creat_linkstack();

	for(i = 0;i < 10;i ++){
		push_linkstack(lsp,&i);
	}
	print_linkstack(lsp);

	pop_linkstack(lsp,&buf);
	printf("pop:%d\n",buf);

	pop_linkstack(lsp,&buf);
	printf("pop:%d\n",buf);

	pop_linkstack(lsp,&buf);
	printf("pop:%d\n",buf);

	pop_linkstack(lsp,&buf);
	printf("pop:%d\n",buf);

	print_linkstack(lsp);

	return 0;
}

