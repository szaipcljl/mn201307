#include <stdio.h>
#include <sys/stat.h>

int main(int argc, const char *argv[])
{
	struct stat info;

	stat("/1.c",&info);

	chown("./x.c",info.st_uid,info.st_gid);

	struct stat info1;
	stat("./x.c",&info1);
	printf("%d,%d\n",info1.st_uid,info1.st_gid);

	return 0;
}
