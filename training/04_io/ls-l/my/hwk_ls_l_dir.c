/*ls -l .c
 * -rwxrwxr-x 1 linux linux 497 Mar 30 18:35 stack.h
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include<dirent.h>

int filetype(mode_t st_mode)
{
	switch(st_mode & S_IFMT)
	{
	case S_IFSOCK:
		printf("sock");
		break;
	case S_IFLNK:
		printf("link");
		break;
	case S_IFREG: //普通文件
		printf("-");
		return 0;
	case S_IFBLK:
		printf("blk");
		break;
	case S_IFDIR:
		printf("dir");//目录文件
		break;
	case S_IFCHR:
		printf("chr");
		break;
	case S_IFIFO:
		printf("ifo");
		break;
	}
	return -1;
}

int read_perm(mode_t st_mode,int S_IR)
{
	if(st_mode & S_IR)
		printf("r");
	else
		printf("-");
}

int write_perm(mode_t st_mode,int S_IW)
{
	if(st_mode & S_IW)
		printf("w");
	else
		printf("-");
}

int ext_perm(mode_t st_mode,int S_IX)
{
	if(st_mode & S_IX)
		printf("x");
	else
		printf("-");
}

int perm(mode_t st_mode)
{
	read_perm(st_mode,S_IRUSR);
	write_perm(st_mode,S_IWUSR);
	ext_perm(st_mode,S_IXUSR);


	read_perm(st_mode,S_IRGRP);
	write_perm(st_mode,S_IWGRP);
	ext_perm(st_mode,S_IXGRP);


	read_perm(st_mode,S_IROTH);
	write_perm(st_mode,S_IWOTH);
	ext_perm(st_mode,S_IXOTH);

	return 0;
}

int info_ls(struct stat info,const char *name)
{

	struct passwd *pd;
	struct group *pg;
	struct tm *tmp;
	filetype(info.st_mode);
	perm(info.st_mode);

	printf(" %d",info.st_nlink);

	pd = getpwuid(info.st_uid);
	printf(" %s",pd->pw_name);

	pg = getgrgid(info.st_gid);
	printf(" %s",pg->gr_name);

	printf(" %ld",info.st_size);

	tmp = localtime(&info.st_mtime);
	printf(" %d %d %d:%d",tmp->tm_mon + 1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min);
	printf(" %s\n",name);

	return 0;
}


int main(int argc, const char *argv[])
{
	struct stat info;
	int flag = 0;
	DIR *pdir;
	struct dirent *p;
	if(argc < 2)
	{
		printf("Usage:%s filename\n",argv[0]);
		return -1;
	}

	if((info.st_mode & S_IFMT) != S_IFDIR)
	{
		stat(argv[1],&info);
		info_ls(info,argv[1]);
	}

	else
	{
		if((pdir = opendir(argv[1])) == NULL)
		{
			perror("opendir");
			return -1;
		}
		while((p = readdir(pdir)) != NULL)
		{
			if(p->d_name[0] == '.')
				continue;
			stat(argv[1],&info);
			info_ls(info,p->d_name);

			//		printf("%s\n",p->d_name);
		}


	}






	return 0;
}
