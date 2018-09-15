#include <stdio.h>
#include <string.h>
#include <errno.h>  /* for strerror */

/*
 * 获取错误代码描述
 **/
int main(int argc, char ** argv)
{
	int i = 0;

	for(i = 0; i < 256; i++)
		printf("errno.%02d is: %s\n", i, strerror(i));

	return 0;
}
