#include<stdio.h>

int main(int argc, const char *argv[])
{
	fprintf(stdout,"hello\n");

	fclose(stdout);

	fprintf(stdout,"world\n");
	return 0;
}
