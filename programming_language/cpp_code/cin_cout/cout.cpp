#include <iostream>
/*
 * 输入流与输出流的基本操作:
 * cout语句的一般格式为：
 * 	cout<<表达式1<<表达式2<<……<<表达式n;
 * 
 * cin语句的一般格式为：
 * 	cin>>变量1>>变量2>>……>>变量n;
 **/

using namespace std;

int main(int argc, const char *argv[])
{
	cout << "This is a simple c++ program." << endl;

	//一个cout语句可以分写成若干行
	cout << "This is "//注意行末尾无分号
		<< "a simple c++ program."
		<< endl;//语句最后有分号

	//也可写成多个cout语句
	cout << "This is ";//语句末尾有分号
	cout << "a simple c++ program.";
	cout << endl;

	//一个插入运算符“<<”, 插入1个输出项到输出流对象
	int a = 1, b = 2, c = 3;
	cout << "a+b+c = " << a + b + c << endl;


	//1.自动判别输出数据的类型
	float d = 3.14;
	char e = 'e';
	cout << a << ' ' << d << ' ' << e << endl;

	return 0;
}
