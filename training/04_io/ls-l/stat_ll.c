#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/stat.h>

int main(int argc, const char *argv[])
{
	if(argc < 2)
	{

	}
	struct stat info;

	stat(argv[1],&info);

	switch(info.st_mode & S_IFMT)
	{
	case S_IFBLK:
		putchar('b');
		break;
	case S_IFCHR:
		putchar('c');
		break;
	case S_IFDIR:
		putchar('d');
		break;
	case S_IFREG:
		putchar('-');
		break;

	case S_IFLNK:
		putchar('l');
		break;
	case S_IFIFO:
		putchar('p');
		break;
	case S_IFSOCK:
		putchar('s');
		break;
	}

	if(info.st_mode & S_IRUSR)
		putchar('r');
	else
		putchar('-');


	if(info.st_mode & S_IWUSR)
		putchar('w');
	else
		putchar('-');

	if(info.st_mode & S_IXUSR)
		putchar('x');
	else
		putchar('-');

	putchar(' ');

	printf("%d",info.st_nlink);
	putchar(' ');

	struct passwd *p;

	p = getpwuid(info.st_uid);
	printf("%s",p->pw_name);
	putchar(' ');

	struct group *g;

	g = getgrgid(info.st_gid);
	printf("%s",g->gr_name);
	putchar(' ');

	printf("%ld",info.st_size);
	putchar(' ');
	struct tm *ptm;
	ptm = localtime(&info.st_mtime);
	printf("%d:%d:%d",ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	putchar(' ');


	printf("%s\n",argv[1]);

	return 0;
}
