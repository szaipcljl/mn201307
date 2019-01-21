#include <stdio.h>
#include <stdlib.h>

typedef int data_t;

typedef struct dblinknode{
	data_t data;
	struct dblinknode *front;   //单链表增加指针域prior,指向本结点直接前驱
	struct dblinknode *next;
}dblink_t;

/*创建双向链表
*/
dblink_t *creat_dblink(void)
{
#if 0
	dblink_t *head,*tail;

	head = malloc(sizeof(dblink_t));
	tail = malloc(sizeof(dblink_t));

	head->next = tail;
	head->front = NULL;
	tail->next = NULL;
	tail->front = head;
#else
	dblink_t *head;

	head = malloc(sizeof(dblink_t));
	head->next = malloc(sizeof(dblink_t)); //头结点的后继指针指向尾结点

	head->front = NULL;  //头结点的直接前驱为空

	head->next->front = head;  //尾结点的直接前驱为头结点
	head->next->next = NULL; 	//尾结点的直接后继为空

#endif

	return head; //返回头结点指针

}

/*头部插入
*/
int ins_head_dblink(dblink_t *head,const data_t *data)
{
	dblink_t * newnode;
	newnode = malloc(sizeof(dblink_t));

	newnode->data = *data;
	newnode->next = head->next; //新结点的后继为头结点原来的后继
	newnode->front = head; 		//新结点的前驱为头结点
	//由新结点发出的个箭头

#if 0
	head->next->front = newnode;
	head->next = newnode;
#else
	newnode->next->front = newnode; //新结点后继的前驱为新结点
	newnode->front->next = newnode; //新结点前驱的后继为新结点
	//新结点收到到两个箭头
#endif

	return 0;
}

/*双向链表头部删除
*/
int del_head_dblink(dblink_t *head,data_t *databuf)
{
	//1.拷贝数据
	//2.保存即将删除的节点地址
	//3.更改表结构
	//4.free
	//
	dblink_t *temp;

	if(NULL == head->next->next) //错位返回情况
		return -1;

	if(NULL != databuf)
		*databuf = head->next->data;

	temp = head->next;

	head->next = head->next->next; //头结点后继的后继作为头结点新到后继
	head->next->front = head;   //头结点作为头结点新后继的前驱

	free(temp);

	return 0;
}

/*打印链表中数据
*/
void print_dblink(const dblink_t *head)
{
	dblink_t *tail;   //函数内head和tail均为局部变量，赋值不改变全局变量值
	while(NULL != head->next->next){
		head = head->next;  //head->next = ...,会改变全局变量值
		printf(" %d ",head->data);
	}
	printf("\n");

	tail = head->next;  //反向打印
	while(NULL != tail->front->front){
		tail = tail->front;
		printf(" %d ",tail->data);
	}
	printf("\n");

	return ;
}

int main(int argc, const char *argv[])
{

	int i;
	dblink_t *head;
	head = creat_dblink();

	for(i = 9 ; i >= 0;i --){
		ins_head_dblink(head,&i);
	}

	print_dblink(head);

	del_head_dblink(head,NULL);
	del_head_dblink(head,NULL);
	del_head_dblink(head,NULL);
	del_head_dblink(head,NULL);

	print_dblink(head);

	return 0;
}
