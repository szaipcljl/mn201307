/*Joseph问题：编号分别为1到10的10个人围成一圈，约定序号为1的人从1开始计数
 * 数到3的人枪毙，他的下一位又从1开始计数，直到人数不足3人为止
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

typedef int data_t;
typedef struct linknode{
	data_t data;
	struct linknode *next;
}link_t;


link_t *creat_linknode(const data_t *data)
{
	link_t *node;

	node = malloc(sizeof(link_t));
	node->data = *data;
	node->next = node;

	return node;
}

int ins_head_link(link_t *node,const data_t *data)
{
	link_t *newnode;

	newnode = malloc(sizeof(link_t));
	newnode->data = *data;
	newnode->next = node->next;
	node->next = newnode;

	return 0;
}

link_t *init_jos(int number)
{
	link_t *node;
	int i;

	node = creat_linknode(&number);

	for(i = number - 1;i > 0;i --){
		ins_head_link(node,&i);
	}

	return node->next;
}

link_t *shushu_jos(link_t *node,int num)
{
	num --;
	while(num --){
		node = node->next;
	}

	return node;
}

int del_head_link(link_t *node)
{
	link_t *temp;
	printf("%d\n",node->next->data);

	temp = node->next;
	node->next = temp->next;

	free(temp);

	return 0;
}

/*
 * 如果满足终止条件返回 0
 * */
int stop_jos(link_t *node,int num)
{
	link_t *temp = node;

	while(num --){
		node = node->next;
	}

	return !(node == temp);
}

link_t *kill_jos(link_t *node,int num)
{
	while(stop_jos(node,num)){
		//1.偏移删除
		node = shushu_jos(node,num);
		del_head_link(node);
		//2.指向下一个=查数节点
		node = node->next;
	}

	return node;
}


void print_jos(link_t *node)
{
	link_t *temp = node;

	do{
		printf(" %d ",node->data);
		node = node->next;

	}while(temp != node);
	printf("\n");
	return ;
}

int main(int argc, const char *argv[])
{
	link_t *jos;

	jos = init_jos(10);

	jos = kill_jos(jos,2);

	print_jos(jos);

	return 0;
}
