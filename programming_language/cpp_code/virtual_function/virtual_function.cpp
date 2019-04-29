#include <iostream>
#include "virtual_function.h"


int main(int argc, const char *argv[])
{
	typedef void(*Fun)(void);

	Base b;

	Fun pFun = NULL;

	cout << "虚函数表地址：" << (int*)(&b) << endl;

	cout << "虚函数表 — 第一个函数地址：" << (int*)*(int*)(&b) << endl;

	// Invoke the first virtual function

	pFun = (Fun)*((int*)*(int*)(&b));
	pFun();

	return 0;
}
