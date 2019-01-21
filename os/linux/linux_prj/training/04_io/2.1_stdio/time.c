#include<stdio.h>
#include<time.h>

void do_log(FILE *fp)
{
	time_t t;
	struct tm *ptm;

	while(1)
	{
		t = time(NULL);
		ptm = localtime(&t);


		printf("%d-%d-%d,%d:%d:%d\n",ptm->tm_year + 1900,ptm->tm_mon + 1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
		fprintf(fp,"%d-%d-%d,%d:%d:%d\n",ptm->tm_year + 1900,ptm->tm_mon + 1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

		fflush(fp);
		sleep(1);
	}
}


int main(int argc, const char *argv[])
{
	FILE *fp;

	if((fp = fopen("./log","w")) == NULL)
	{

	}

	do_log(fp);

	fclose(fp);
	return 0;
}
