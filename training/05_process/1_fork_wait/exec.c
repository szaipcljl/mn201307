#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
#if 0
	execl("/bin/ls","ls","-l",NULL);
#elif 0
	char *const str[] = {"ls","-l",NULL};
	execv("/bin/ls",str);
#elif 1
	execlp("ls","ls","-l",NULL);
#endif

	return 0;
}
