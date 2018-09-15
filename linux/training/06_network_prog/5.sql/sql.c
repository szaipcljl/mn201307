#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sqlite3.h>


int callback(void *p,int num,char *val[],char *name[])
{
	int i;

	if(0 == *(int *)p){
		for(i = 0;i < num;i ++){
			printf("%s\t",name[i]);
		}
		printf("\n");
		*(int *)p = 1;
	}
	for(i = 0;i < num;i ++){
		printf("%s\t",val[i]);
	}
	printf("\n");

	return 0;
}

void insert_sql(sqlite3 *dbp,int id,char *name,float english)
{
	char *buf;
	char *errmsg;
	buf = malloc(128);
	bzero(buf,128);
	sprintf(buf,"insert into stu (id,name,english) values(%d,'%s',%.2f);",id,name,english);
	if(SQLITE_OK != sqlite3_exec(dbp,
				buf,NULL,NULL,&errmsg) ){
		puts(errmsg);
	}

	free(buf);

	return ;
}


int main(int argc, const char *argv[])
{
	sqlite3 *dbp;
	char *errmsg;
	int flag = 0;

	char **pazResult;
	int row;
	int column;

	int i,j;
	char buf[128];
	int id = 10005;
	char name[64] = "xiaoming";
	float english = 99;

	if(SQLITE_OK != sqlite3_open("my.db",&dbp) ){
		puts(sqlite3_errmsg(dbp));
		return -1;
	}

	if(SQLITE_OK != sqlite3_exec(dbp,
				"create table stu(id integer primary key,name text,english real NULL);",NULL,NULL,&errmsg) ){
		puts(errmsg);
	}
	if(SQLITE_OK != sqlite3_exec(dbp,
				"insert into stu (id,name,english) values(10000,'li',59);",NULL,NULL,&errmsg) ){
		puts(errmsg);
	}
	if(SQLITE_OK != sqlite3_exec(dbp,
				"insert into stu (id,name,english) values(10001,'xiaoming',100);",NULL,NULL,&errmsg) ){
		puts(errmsg);
	}

	if(SQLITE_OK != sqlite3_exec(dbp,
				"insert into stu (id,name,english) values(10002,'xiaoli',70);",NULL,NULL,&errmsg) ){
		puts(errmsg);
	}

	bzero(buf,sizeof(buf));
	sprintf(buf,"insert into stu (id,name,english) values(%d,'%s',%.2f);",id,name,english);
	if(SQLITE_OK != sqlite3_exec(dbp,
				buf,NULL,NULL,&errmsg) ){
		puts(errmsg);
	}

	insert_sql(dbp,10007,"zhangsan",57);

	flag = 0;
	if(SQLITE_OK != sqlite3_exec(dbp,
				"select * from stu;",callback,(void *)&flag,&errmsg) ){
		puts(errmsg);
	}
	flag = 0;
	if(SQLITE_OK != sqlite3_exec(dbp,
				"select * from stu;",callback,(void *)&flag,&errmsg) ){
		puts(errmsg);
	}

	puts("/**************************/");


	if(SQLITE_OK != sqlite3_get_table(dbp,
				"select * from stu;",&pazResult,&row,&column,&errmsg)){
		puts(errmsg);
	}



	for(j = 0;j <= row;j ++){
		for(i = column * j;i < column *(j + 1);i ++ ){
			printf("%s\t",pazResult[i]);
		}
		printf("\n");
	}

	sqlite3_free_table(pazResult);



	if(SQLITE_OK != sqlite3_close(dbp) ){
		puts(sqlite3_errmsg(dbp));
		return -1;
	}

	return 0;
}
