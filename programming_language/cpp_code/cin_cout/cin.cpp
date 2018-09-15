#include <iostream> 

#define METHOD_ONE
#ifdef METHOD_ONE
using namespace std;
#endif

int main(int argc, const char *argv[])
{
	int a, b;
	//根据变量的类型从输入流中提取相应长度的字节
	//空格字符和回车换行符将被跳过;按照相应的格式输入，否则容易出错
#if defined(METHOD_ONE)
	cin >> a >> b;

	cout << "METHOD_ONE:" << endl;
	cout << a << ' ' << b << endl;
#else
	std::cin >> a >> b;

	std::cout << "METHOD_TWO:" << std::endl;
	std::cout << a << ' ' << b << std::endl;
#endif

	return 0;
}
