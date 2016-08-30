#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fsrc;
	FILE *fdest;

	char buf[64];
	int ch;
	if(argc < 3)
	{
		fprintf(stderr,"usage:%s <file1> <file2>\n",argv[0]);
		return -1;
	}

	if((fsrc = fopen(argv[1],"r")) == NULL)
	{
		perror("fail to fopen");
		return -1;
	}

	if((fdest = fopen(argv[2],"w")) == NULL)
	{
		perror("fail to fopen");
		return -1;
	}
#if 1
a1:	ch = fgetc(fsrc);

	while(feof(fsrc) == 0)  //判断上一步fgetc取得的值，取到值，就
	{ 						//不是文件结尾，feof返回值就是0；
		//		ch = fgetc(fsrc); //获得最后一个字符后，feof返回值是0；
		fputc(ch,fdest);
a2:		ch = fgetc(fsrc);
	}
#endif
#if 0
	while(fgets(buf,sizeof(buf),fsrc) != NULL)
	{
		fputs(buf,fdest);
	}
#endif

#if 0
	size_t n;

	while((n = fread(buf,1,sizeof(buf),fsrc)) > 0)
	{
		fwrite(buf,1,n,fdest);
	}

#endif
	fclose(fsrc);
	fclose(fdest);
	return 0;
}
