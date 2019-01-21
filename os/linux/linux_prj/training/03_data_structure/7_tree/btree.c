#include <stdio.h>
#include <stdlib.h>

typedef int data_t;

typedef struct btree{ 	//定义二叉数节点的内部结构
	data_t data; 		//数据域
	data_t num;
	struct btree *lchild;
	struct btree *rchild; //指向左孩子和右孩子到指针
}btree_t; //二叉数节点类型


/*创建一个二叉数（Binary Tree）
*/
btree_t *creat_btree(int num,int max)   //num:节点编号；max:节点最大编号；
{
	btree_t *root; 						//定义指向二叉数的指针（根节点指针）

	if(num > max)
		return NULL;

	root = malloc(sizeof(btree_t));

	root->data = num;
	root->num = num;
	root->lchild = creat_btree(2 * num, max);
	root->rchild = creat_btree(2 * num + 1, max);

	return root;
}

/*创建一个普通树
*/
btree_t *creat_tree(int num,int max,int *flag)
{
	btree_t *root;

	if(num > max || 0 == flag[num])
		return NULL;

	root = malloc(sizeof(btree_t));

	root->data = num;
	root->num = num;
	root->lchild = creat_tree(2 *num,max,flag);
	root->rchild = creat_tree(2 * num + 1,max,flag);

	return root;
}


void pre_visit(btree_t *root)  //前序(preorder)遍历
{
	if(NULL == root) 	  		//空树返回
		return ;

	printf(" %d ",root->data); 	//先访问当前节点
	pre_visit(root->lchild); 	//再访问该节点的左子树
	pre_visit(root->rchild);  	//最后访问该节点到右子树

	return ;
}

void post_visit(btree_t *root) //后根次序(postorder)遍历
{
	if(NULL == root)
		return ;

	post_visit(root->lchild);
	post_visit(root->rchild);
	printf(" %d ",root->data);

a4:	return ;
}

void mid_visit(btree_t *root) 	//中序（inorder）遍历
{
	if(NULL == root)
		return ;

	mid_visit(root->lchild);
	printf(" %d ",root->data);
	mid_visit(root->rchild);

	return ;
}

/*按编号顺序遍历：创建数组形式顺序队列，下标变量标记队前队尾位置
*/
void leve_visit(btree_t *root)
{
	btree_t * *queue; //创建顺序队列，数组 队前下标 队尾下标
	int rear = 0;     //入队（队尾插入）赋值后队尾下标加一指下一向空位；
	//出队（队头删除）后队头下标加一指向下一数据
	int front = 0; 	  //队头与队尾相遇，数据出队完，下一待删数据为空，空队列

	queue = malloc(sizeof(data_t) * 100);

	//1.入队根节点
	queue[rear ++] = root;
	while(rear != front){
		if(/*有左孩子*/queue[front]->lchild != NULL)
			queue[rear ++] = queue[front]->lchild;
		if(/*有右孩子*/NULL != queue[front]->rchild)
			queue[rear ++] = queue[front]->rchild;
		//出队打印自己
		printf(" %d ",queue[front++]->data);
	}
	printf("\n");

	free(queue);

	return ;
}

/*以非递归的形式实现二叉树访问（以栈的形式模拟递归调用过程）：
 * 前序遍历
 */
void pre_stack_visit(btree_t *root)
{
	btree_t *stack[20]; //创建顺序栈，由数组定义，配合栈顶下标变量top
	int top = -1; 		//指示栈顶位置（数组下标）的变量

	printf("pre_stack_visit");
	while(NULL != root || -1 != top){ //空树或空栈结束循环
		while(NULL != root){ //空树结束循环
			//打印自己
			printf(" %d ",root->data);
			//入栈自己
			stack[++top] = root; //加一后空位置入栈，
			//获取左孩子
			root = root->lchild;
		}

		//出栈栈顶元素
		root = stack[top --];  //出栈后减一指向有效元素
		//获取右孩子
		root = root->rchild;
	}
	printf("\n");

	return ;
}

/*非递归形式：中序遍历
*/
void mid_stack_visit(btree_t *root)
{
	btree_t *stack[20];
	int top = -1;

	printf("mid_stack_visit");
	while(NULL != root || -1 != top){
		while(NULL != root){
			//入栈自己
			stack[++top] = root;
			//获取左孩子
			root = root->lchild;
		}

		//出栈栈顶元素
		root = stack[top --];
		//打印自己
		printf(" %d ",root->data);
		//获取右孩子
		root = root->rchild;
	}
	printf("\n");

	return ;
}

#if 0
void post_visit(btree_t *root)
{
	if(NULL == root)
		return ;
a1: post_visit(root->lchild);
	// 数据出栈，获取右孩子
	// 数据入栈
a2:	post_visit(root->rchild);
	//数据出栈，打印自己
a3:	printf(" %d ",root->data);

a4:	return ;
}

#endif

/*以栈的形式实现：后根次序遍历
*/
void post_stack_visit(btree_t *root)
{
	struct st{
		btree_t *root; //定义指向二叉数的指针
		int flag; 	//标记位
	}*stack; 		//定义一个指向用来保存地址和标记的栈结构体的指针

	int top = -1; 	//指示栈顶位置的下标变量

	stack = malloc(sizeof(struct st) * 20); //顺序栈指针指向顺序栈

	printf("post_stack_visit:");
	while(NULL != root || -1 != top){
		while(NULL != root){
			stack[++top].root = root;
			stack[top].flag = 1;  //压栈后获取左孩子
			root = root->lchild;
		}

		if(1 == stack[top].flag){
			root = stack[top].root->rchild; //获取右孩子后标记为2
			stack[top].flag = 2;
		}
		else if(2 == stack[top].flag){
			printf(" %d ",stack[top --].root->data);
		}
	}

	printf("\n");
	free(stack);
	return ;
}

int main(int argc, const char *argv[])
{
	int flag[13] = {0,1,1,1,0,1,1,0,0,0,1,1,0};

	btree_t *root = creat_tree(1,11,flag);


	pre_visit(root);
	printf("\n");

	mid_visit(root);
	printf("\n");

	post_visit(root);
	printf("\n");

	leve_visit(root);

	pre_stack_visit(root);
	mid_stack_visit(root);
	post_stack_visit(root);

	return 0;
}
