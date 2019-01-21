#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

int main(int argc, const char *argv[])
{
	DIR *pdir;
	struct dirent *p;
	if((pdir = opendir("/usr/include/")) == NULL)
	{
		perror("opendir");
		return -1;
	}

	//	p = readdir(pdir);

	//	printf("%s\n",p->d_name);


	chdir("/usr/include/");
	while((p = readdir(pdir)) != NULL)
	{
		if(p->d_name[0] == '.')
			continue;
		printf("%s\n",p->d_name);
	}

	return 0;
}
