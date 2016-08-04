#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#define DATA_BASE  "./my.db"
#define WORD_TABLE  "word_table"
#define DICT  "./dict.txt"

#define BUFF_SIZE 1024

int do_loadword(sqlite3 *db)
{
	FILE *fp = NULL ;
	char buf[BUFF_SIZE];
	int i ;
	char word[512];
	char disp[512];
	char sql[BUFF_SIZE];
	char *errmsg,*p;
	int count = 0,sum = 0;

	if(NULL == (fp = fopen(DICT, "r")))
	{
		perror("fopen");
		return -1;
	}

	while(fgets(buf,BUFF_SIZE,fp) != NULL)
	{
		i = 0;
		buf[strlen(buf) - 1] = '\0';
		if(strlen(buf) == 0)
			continue;

		p = buf;
		while(*p != '\0')
		{
			if(*p == '\'' || *p == '\"')
				*p = ' ';
			p ++;
		}

		while(buf[i] != ' ' && buf[i] != '\0') i ++;
		if(i == strlen(buf))
			continue;
		buf[i] = '\0';
		strcpy(word,buf);

		i ++;
		while(buf[i] == ' ' && buf[i] != '\0') i ++;
		if(buf[i] == '\0')
            continue;

		strcpy(disp,&buf[i]);

		sprintf(sql,"insert into %s values('%s','%s')",WORD_TABLE,
				word,disp);

		if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
		{
			fprintf(stderr,"sqlite3_exec : %s\n",errmsg);
			return -1;
		}

		//printf("word :%s\n",word);
		count ++;
		if(count >= 500)
		{
			sum ++;
			count = 0;
			for(i = 0; i < 50 ; i ++)
				putchar('\b');
			printf("%%%2.1f", sum*2.55);
			fflush(stdout);

			for(i = 0; i < sum; i ++)
			printf(".");
			fflush(stdout);
		}
	}

	for(i = 0; i < 50 ; i ++)
		putchar('\b');
	printf("%%%2.1f",100*1.0);
	fflush(stdout);
	for(i = 0; i < sum; i ++)
		printf(".");
	fflush(stdout);

	putchar('\n');
	return 0;
}


int main(int argc, const char *argv[])
{
	sqlite3 *db ;
	char *errmsg;
	char sql[128],buf[128] ;

	if(SQLITE_OK != sqlite3_open(DATA_BASE,&db))
	{
		fprintf(stderr,"sqlite3_open : %s\n",sqlite3_errmsg(db));
		return -1;
	}
	sprintf(sql,"create table %s (word text,disp text)",WORD_TABLE);

	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
	{
		sprintf(buf,"table %s already exists",WORD_TABLE);
		if(strcmp(errmsg,buf) )
		{
			fprintf(stderr,"sqlite3_exec : %s\n",errmsg);
			return -1;
		}
	}

	do_loadword(db);

	return 0;
}



