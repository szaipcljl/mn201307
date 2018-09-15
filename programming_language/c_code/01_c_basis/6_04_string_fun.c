#include <stdio.h>

#pragma pack(4)

//extern void fun(void);

int fun(char *a,char b[])
{
	int i = 0;

	printf("abcd sizeof b = %ld \n",sizeof(b));
	while('\0' != b[i]){
		a[i] = b[i];
		i ++;
	}
	a[i] = '\0';

	return 0;
}


int strcat_my(char *a,char *b )
{
	int i = 0,j = 0;

	while('\0' != a[i])
		i ++;

	while('\0' != b[j]){
		puts("strcat_my:11");
		a[i ++] = b[j ++];
	}
	a[i] = b[j];

	return 0;
}

int strcmp(char *a,char *b)
{
	int i = 0;

	while(0 != a[i] && 0 != b[i]){
		if(a[i] - b[i])
			break;
		i ++;
	}

	return  a[i] - b[i];
}

int strlen(char *a)
{
	return ;//返回不算\0的字母个数(未完善)
}

int main(int argc, const char *argv[])
{

	int define;

	char ss1[64] = "123\0";
	char ss2[64] = "456";

	//ss1  >> ss2;

	strcat_my(ss1,ss2);

	//fun(ss2,ss1);

	//printf("abcd %d \n",ss);
	printf("ss1:\n");
	puts(ss1);

	printf("strcmp return: %d\n",strcmp(ss1,ss2));

	return 0;
}
