#include <stdio.h>
#include <unistd.h>

/*
 * 文件的访问模式，io.h文件中宏定义：
 * #define  F_OK      0     //Check for file existence
 * #define  X_OK      1     // Check for execute permission.
 * #define  W_OK     2     // Check for write permission
 * #define  R_OK      4     // Check for read permission
 *
 */

int main(void)
{
	if (access("./access_demo.c", F_OK) == 0) {
		printf("fileexists!\n");

	}


	if (access("./access_demo.c", X_OK) == 0) {
		printf("file can beexecuted!\n");

	}


	if(access("./access_demo.c", W_OK) == 0) {
		printf("file can bewritten!\n");
	}


	if(access("./access_demo.c", R_OK) == 0) {
		printf("file can beread!\n");
	}

	return 0;
}

