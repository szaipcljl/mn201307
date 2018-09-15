#include <stdio.h>

/*输入字符串,字母向后移动三位后输出，其余原样输出*/

int main(int argc, const char *argv[])
{
	char ch;

	printf("input a srting:\n");
	while((ch = getchar()) != '\n'){
		if(ch >= 'a' && ch <= 'w' || ch >= 'A' && ch <= 'W')
			ch = ch + 3;
		else if(ch >= 'x' && ch <= 'z' || ch >= 'X' && ch <= 'Z')
			ch = ch - 26 + 3;
		else 
			;

		putchar(ch);
	}

	putchar(10);

	return 0;
}
