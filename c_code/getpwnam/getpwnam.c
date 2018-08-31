#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	struct passwd * pwd;
	char *username = "chris";

#if 0
	/*
	 * 可以使用getuid()获取用户的ID号,然后通过getpwuid函数
	 * 通过用户的uid查找用户的passwd数据来获取系统登录的用户名。
	 */
	pwd = getpwuid(getuid());
	printf("The currently logged-in user:%s\n", pwd->pw_name);
#else

	pwd = getpwnam(username);
	if(!pwd) {
		printf("%s is not exist\n", username);
		return -1;
	}
#endif

	printf("pwd->pw_name = %s\n", pwd->pw_name);
	printf("pwd->pw_passwd = %s\n", pwd->pw_passwd);
	printf("pwd->pw_uid = %d\n", pwd->pw_uid);
	printf("pwd->pw_gid = %d\n", pwd->pw_gid);
	printf("pwd->pw_gecos = %s\n", pwd->pw_gecos);
	printf("pwd->pw_dir = %s\n", pwd->pw_dir);
	printf("pwd->pw_shell = %s\n", pwd->pw_shell);

	char *groupname = strdup(username);
	struct group* grp = getgrnam(groupname);
	if (!grp) {
		printf("invalid gid\n");
		return -1;
	}

	printf("\ngrp->gr_name = %s\n", grp->gr_name);
	printf("grp->gr_passwd = %s\n", grp->gr_passwd);
	printf("grp->gr_gid = 0x%x\n", grp->gr_gid);

	return 0;
}


