#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>
#include<unistd.h>
#include<dirent.h>

void display_file(const char *filename)
{
	struct stat info;


	stat(filename,&info);

	switch(info.st_mode & S_IFMT)
	{
	case S_IFSOCK:
		putchar('s');
		break;
	case S_IFDIR:
		putchar('d');
		break;
	case S_IFREG:
		putchar('-');
		break;
	case S_IFBLK:
		putchar('b');
		break;
	case S_IFCHR:
		putchar('c');
		break;
	case S_IFLNK:
		putchar('l');
		break;
	case S_IFIFO:
		putchar('p');
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

	struct group *q;
	q = getgrgid(info.st_gid);
	printf("%s",q->gr_name);
	putchar(' ');

	printf("%ld",info.st_size);
	putchar(' ');

	struct tm *ptm;
	ptm = localtime(&info.st_mtime);
	printf("%d:%d:%d",ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	putchar(' ');

	printf("%s\n",filename);
}

void display_dir(const char *filename)
{
	DIR *pdir;
	struct dirent *pdirent;
	pdir = opendir(filename);

	chdir(filename);

	while((pdirent = readdir(pdir)) != NULL)
	{
		if(pdirent->d_name[0] == '.')
			continue;
		display_file(pdirent->d_name);
	}
}

int main(int argc, const char *argv[])
{
	struct stat info;
	if(argc < 2)
	{

	}

	stat(argv[1],&info);

	if(S_ISDIR(info.st_mode))
		display_dir(argv[1]);
	else
		display_file(argv[1]);
	return 0;
}

