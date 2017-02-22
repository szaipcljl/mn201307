/*
 * C标准中一些预定义的宏:
 *
 *    宏                    意义
 * __DATE__    进行预处理的日期（“Mmm dd yyyy”形式的字符串）
 * __TIME__    源文件编译时间，格式“hh：mm：ss”
 *
 * __FILE__    当前源代码文件名
 * __LINE__    当前源代码行号
 * __func__    当前所在函数名
 *
 * */

#include <stdio.h>
#include <libgen.h> //basename

#define DEBUG

#ifdef DEBUG
#define debug(M, ...) printf("[DEBUG][%s/%d/%s]: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

void where_me();

int main(int argc, const char *argv[])
{
	printf("The date and time is %s %s.\n", __DATE__, __TIME__);
	printf("filename:%s\n", basename(__FILE__));

	debug("hello linux\n");

	where_me();
	return 0;
}

void where_me()
{
	debug();
}

