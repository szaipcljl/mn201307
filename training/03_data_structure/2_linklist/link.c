#include <stdio.h>
#include <stdlib.h>

typedef int data_t;
typedef struct linknode {
	data_t data; 			//结点的数据域
	struct linknode *next;  //结点的后继指针域
}link_t; 		//定义一个单链表结点类型的结构体，并起一个新名

/*创建链表头
*/
link_t *creat_link()
{
	link_t *head; 		//定义一个指向头结点的指针

	head = malloc(sizeof(link_t));
	if(NULL == head) 	//堆栈满时不分配内存空间
		return NULL;

	head->next = NULL; 	//头结点的后继指针置空

	return head; 		//返回头指针
}

/*链表的头部插入
*/
int ins_head_link(link_t *head,const data_t *data)
{
	//1.开辟保存新数据的空间
	// 	1.2 初始化数据
	//2.将这块空间插入到链表中
	//  2.1 改变指针指向
	//  	1）新创建的空间的next 指向 插入位置的后一个元素
	//  	2）插入位置的前一个元素的 next 指向新节点

	link_t *newnode;

	newnode = malloc(sizeof(link_t));
	if(NULL == newnode)
		return -1;

	newnode->data = *data;

	newnode->next = head->next;	//指向头结点的后继
	head->next = newnode; 		//头结点的后继为新结点（指向新结点）

	return 0;
}

/*链表的任意位置插入：将数据插入到表中index位置之前
*/
int ins_index_link(link_t *head,int index,const data_t *data)
{
	//1.查找定位，通过头指针依次向后移动;移动时错误检查
	//2.假设指向的节点为头，执行头部插入

	if(index < 0)  // 参数index出错
		return -1;

	while(index --){
		head = head->next; 	//头指针指向后继指针，头结点移动到0位置，
		//直到index-1，位置，共index次
		if(NULL == head) 	//前驱或未查找到前驱前即指向空，即index大于表长
			return -1;
	}

	ins_head_link(head,data); //找到 ，头结点指向插入位置前驱，执行头部插入

	return 0;
}

/*链表的头部删除
*/
int del_head_link(link_t *head,data_t *databuf)
{
	link_t *temp;

	if(NULL == head->next)
		return -1;

	if(NULL != databuf)
		*databuf = head->next->data;

	temp = head->next; //中间变量保存要删除结点地址，用于删除后空间释放

	head->next = temp->next; //head->next = head->next->next;
	//删除结点前驱的后继指针指向删除结点的后继

	free(temp);

	printf("[del_dead_link]\n");
	return 0;
}

/*链表的任意位置删除
*/
int del_index_link(link_t *head,int index,data_t *databuf)
{
	//1.定位
	//2.拷贝
	//3.头部删除

	while(index --){ //头指针向后移动，直到移动到要删除结点的前驱结点
		head = head->next;
		if(NULL == head->next) //头指针移动到前驱或之前即为空，index大于表长
			return -1;
	}

	del_head_link(head,databuf); //以前驱为头指针，执行头部删除

	return 0;
}

/*删除所有结点
*/
int clean_link(link_t *head)
{
#if 0
	while(NULL != head->next){
		del_head_link(head,NULL);
	}
#endif
	while( ! del_head_link(head,NULL)) //循环执行头指针删除，成功返回0，！0
		; 			//头指针的后继为空时，返回值为-1，取反结束循环

	return 0;
}


/*
 * 返回找到的元素的前一个节点的地址
 */
link_t *locate_link(link_t *head,const data_t *data)
{

	while(NULL != head->next){ //头结点后继不为空
		if(*data == head->next->data) //判断与头结点后继中数据是否相等
			return head; 		//相等，返回查找到元素前驱地址
		head = head->next;  	//不相等，头指针后移一个结点
	}

	return NULL; //查找失败返回空
}

/*获取头结点后继中数据，通过地址指针传出
*/
int get_head_link(link_t *head,data_t *databuf)
{
	if(NULL == head->next) //错误返回情况：头结点后继为空
		return -1;

	if(NULL != databuf)
		*databuf = head->next->data;

	return 0;
}


/*获取第index个位置上的数据
*/
int get_index_link(link_t *head,int index,data_t *databuf)
{
	while(index --){ //“头指针”从位置0，移动到获取位置的前驱index-1
		head = head->next; 		//移动index-1次，执行一次，index减一
		if(NULL == head->next)  //获取位置前驱或前面为空，大于表长
			return -1; 			//错误返回
	}
	get_head_link(head,databuf);

	return 0;
}

/*计算表长
*/
int length_link(const link_t *head)
{
	int sum = 0;

	while(NULL != head->next){ //每次头结点后继不为空，
		sum ++;            //“头指针”向后移动一次，计数器加一，
		head = head->next; //“头指针”后继为空，即已移动到表尾
	}

	return sum;
}

/*判断是否为空：头指针后继为空
*/
int empty_link(link_t *head)
{
	return NULL == head->next; // head->next  == (*head).next;
}

/*销毁单链表
*/
int des_link(link_t **headp)
{
	clean_link(*headp); //先清空结点，每一个节点空间都会被释放
	free(*headp); 		//释放头结点，通过地址操控头结点指针（全局变量）
	*headp = NULL;  	//头指针赋值为空，防止成为野指针

	return 0;
}

/*打印单链表中数据
*/
void print_link(const link_t *head)
{

	if(NULL == head){
		printf("[print_link] no link: head = NULL\n"); //传入NULL: Segmentation fault
		return ;
	}

	//函数内head为局部变量，可不断被赋值
	while(NULL != head->next){ //“头指针”后继不为空，有后继，可打印，可移动
		printf(" %d ",head->next->data); //打印后继中数据
		head = head->next;  	//头指针向后移一个位置，直到移到表尾退出
	}

	printf("\n");
}

int main(int argc, const char *argv[])
{

	link_t *head;
	int i;

	head = creat_link();

	for(i = 9;i >= 0;i --){
		ins_head_link(head,&i);
	}
	print_link(head);

	i = 90;
	ins_index_link(head,11,&i);
	ins_index_link(head,15,&i);
	print_link(head);



	del_index_link(head,10,&i);
	printf("del:%d\n",i);
	print_link(head);

	del_head_link(head,&i);
	print_link(head);

	des_link(&head);
	print_link(head);

	return 0;
}
