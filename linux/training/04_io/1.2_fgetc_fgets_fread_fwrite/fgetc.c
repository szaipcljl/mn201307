#include <stdio.h>

int main(int argc, const char *argv[])
{
	int ch;

	//	char ch;
	ch = fgetc(stdin); //从终端读取一个字符，

	printf("%x\n",ch);
	return 0;
}
