#include <stdio.h>
#include <sys/stat.h>

int main(int argc, const char *argv[])
{
	//	chmod("./1.txt",0777);
	chmod("./1.txt",S_IRUSR | S_IWGRP); //Man 2 chmod

	return 0;
}
