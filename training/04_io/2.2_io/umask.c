#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, const char *argv[])
{
	int fd;

	umask(0222);
	fd = open("./3.txt",O_RDONLY | O_CREAT | O_TRUNC,0664);
	return 0;
}

//文件实际权限=~umask & open第三个参数
//
//010  010   010
//
//101  101   101
//110  110   100
//100  100   100
