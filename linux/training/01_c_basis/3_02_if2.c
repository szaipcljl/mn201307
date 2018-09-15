#include <stdio.h>

/*输入一组字符串，将其中字母大小写转换后输出;数字输出number;其他输出other*/

int main(int argc, const char *argv[])
{
	char ch;

	printf("Please input a string:\n");
	while((ch = getchar()) != '\n')
	{
		//	ch = getchar();

		if(ch >= 'a' && ch <= 'z'){
			putchar(ch - 32);
			putchar(10);
		}else if(ch >= 'A' && ch <= 'Z'){
			putchar(ch + 32);
			putchar(10);
		}else if(ch >= '0' && ch <= '9'){
			printf("number\n");
		}else{
			printf("other\n");
		}
	}

	return 0;
}
