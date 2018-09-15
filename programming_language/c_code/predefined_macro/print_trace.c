#include <stdio.h>

#if 1
void _print_log(const char *func, const char *file, int line, const char *msg)
{
	printf("###-mn [%s:%s:%d] %s\n",func, file, line, msg);
}

#define print_log(msg) do {					\
	_print_log(__func__, __FILE__, __LINE__, msg);		\
} while (0)
#else
#define print_log(msg) do {					\
	printf("###-mn [%s:%s:%d] %s\n",__func__, __FILE__, __LINE__, msg);	\
} while (0)
#endif


int main(int argc, const char *argv[])
{
	print_log("error_msg");
	return 0;
}
