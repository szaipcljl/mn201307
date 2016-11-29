#include <stdlib.h>
#include <stdio.h>
/*
 * atof: ascii to floating point numbers
 * atof - convert a string to a double
 * Man atof
 * atof()会扫描参数nptr字符串，跳过前面的空格字符，
 * 直到遇上数字或正负符号才开始做转换，而再遇到非数
 * 字或字符串结束时('\0')才结束转换，并将结果返回。
 * 参数nptr字符串可包含正负号、小数点或E(e)来表示指数部分
 */

int main()
{
	double d;
	char str[] = "123.456";

	d = atof(str);
	printf("string=%s\ndouble=%lf\n",str,d);

	return 0;
}
