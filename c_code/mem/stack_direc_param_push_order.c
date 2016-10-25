//#include <iostream>
#include <stdio.h>

/*
 * 1. 栈的增长方向均为 High -> low
 * 2. 不同的系统参数入栈顺序可能不同
 * 满递减：堆栈通过减小存储器的地址向下增长，堆栈指针指向内含有效数据项的最低地址。
 * */

void findStackDirection()
{
	static int* addr = NULL;
	int dummy;
	if(addr == NULL) {
		addr = &dummy;
		findStackDirection();
	} else {
		if(&dummy > addr) {
			printf("STACK direction: Low -> High\n");
			//std::cout << "STACK direction: Low -> High" << std::endl;
		} else {
			printf("STACK direction: High -> low\n");
			//std::cout << "STACK direction: High -> low" << std::endl;
		}
	}
}

void func(int a, int b)
{
	if (&b < &a)	{
		printf("ARGS: left -> right\n");
		//std::cout << "ARGS: left -> right" << std::endl;
	} else {
		printf("ARGS: right -> left\n");
		//std::cout << "ARGS: right -> left" << std::endl;
	}
}

int main()
{
	findStackDirection();
	func(1,2);
}
