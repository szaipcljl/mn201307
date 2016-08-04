#include "head.h"

int send_message(int sockfd,msg_t *msg)
{
	if(-1 == send(sockfd,msg,sizeof(msg_t),0))
		err_sys("send_message");

	return 0 ;
}

int recv_message(int sockfd,msg_t *msg)
{
	if(recv(sockfd,msg,sizeof(msg_t),0)  <= 0 )
		err_sys("recv_message");
	return 0;
}
int do_register(int connectfd,msg_t *msg,sqlite3 *db)
{

	char sql[BUFF_SIZE];
	char *errmsg ;
	char **result ;
	int n_row,n_column;


	sprintf(sql,"select * from  %s where name='%s'",USER_TABLE,msg->name);

	if(SQLITE_OK != sqlite3_get_table(db,sql,&result,&n_row,&n_column,&errmsg))
	{
		printf("sqlite3_get_table: %s\n",errmsg);

		msg->flag = -2;
		send_message(connectfd,msg);
		return -1;
	}

	if(n_row != 0)
	{
		msg->flag = -1;
		send_message(connectfd,msg);
		return -1;
	}

	sprintf(sql,"insert into %s values('%s','%s')",USER_TABLE,msg->name,msg->passwd);

	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
	{
		printf("sqlite3_exec");
		return -1;
	}

	msg->flag = 1 ;
	send_message(connectfd,msg);

	return 0;
}

int do_login(int connectfd,msg_t *msg,sqlite3 *db)
{
	char sql[BUFF_SIZE] ;
	char *errmsg ;
	char **result ;
	int n_row,n_column;

	sprintf(sql,"select * from %s where name='%s'",USER_TABLE,msg->name);

	if(SQLITE_OK != sqlite3_get_table(db,sql,&result,&n_row,&n_column,&errmsg))
	{
		printf("sqlite3_get_table: %s\n",errmsg);
		msg->flag = -2 ;
		send_message(connectfd,msg);
		return -1;
	}

	if(n_row == 0)
	{
		msg->flag = -3;
		send_message(connectfd,msg);
		return -1;
	}

	sprintf(sql,"select * from %s where name='%s' and passwd='%s'",USER_TABLE,msg->name,msg->passwd);

	if(SQLITE_OK != sqlite3_get_table(db,sql,&result,&n_row,&n_column,&errmsg))
	{
		printf("sqlite3_get_table: %s\n",errmsg);
		msg->flag = -2 ;
		send_message(connectfd,msg);
		return -1;
	}

	if(n_row == 0)
	{
		msg->flag = -1;
		send_message(connectfd,msg);
		return -1;
	}

	msg->flag = 1;
	send_message(connectfd,msg);

	return 0;
}

int do_querry(int connectfd,msg_t *msg,sqlite3 *db)
{
	char sql[BUFF_SIZE] ;
	char *errmsg ;
	char **result ;
	int n_row,n_column;
	time_t t ;

	sprintf(sql,"select * from %s where word='%s'",WORD_TABLE,msg->data);

	if(SQLITE_OK != sqlite3_get_table(db,sql,&result,&n_row,&n_column,&errmsg))
	{
		printf("sqlite3_get_table: %s\n",errmsg);
		msg->flag = -2 ;
		send_message(connectfd,msg);
		return -1;
	}

	printf("word = %s\n",msg->data);

	if(n_row == 0)
	{
		msg->flag = -1;
		send_message(connectfd,msg);
		return -1;
	}

	msg->flag = 1;

	sprintf(msg->data,"%s",result[3]);
	send_message(connectfd,msg);

	time(&t);
	sprintf(sql,"insert into %s values('%s','%s','%s') ",HIS_TABLE,msg->name,result[2],ctime(&t));

	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
	{
		printf("sqlite3_exec : %s\n",errmsg);
		return -1;
	}

	return 0;
}

int do_history(int connectfd,msg_t *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[BUFF_SIZE] ;
	char **result ;
	int n_row,n_column;
	int i ;

	sprintf(sql,"select * from %s where name='%s'",HIS_TABLE,msg->name);
	if(SQLITE_OK != sqlite3_get_table(db,sql,&result,&n_row,&n_column,&errmsg))
	{
		printf("sqlite3_get_table: %s\n",errmsg);
		msg->flag = -2 ; //执行失败
		send_message(connectfd,msg);
		return -1;
	}

	if(n_row == 0)
	{
		msg->flag = -1;//没有查询记录
		send_message(connectfd,msg);
		return -1;
	}

	sprintf(msg->data,"%s %s %s",result[0],result[1],result[2]);
	msg->flag = 1 ;
	send_message(connectfd,msg);

	for(i = 1 ; i <= n_row; i ++)
	{
		//发送历史数据
		sprintf(msg->data,"%s %s %s",result[i*n_column],result[n_column*i+1] ,result[n_column + 2]);
		msg->flag = 1 ;
		send_message(connectfd,msg);
	}

	msg->flag = -2 ; //发送完毕
	send_message(connectfd,msg);

	return 0;
}

int do_delhistory(int connectfd,msg_t *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[BUFF_SIZE] ;

	sprintf(sql,"delete from %s where name='%s' ",HIS_TABLE,msg->name);
	if(SQLITE_OK != sqlite3_exec(db,sql,NULL,NULL,&errmsg))
	{
		msg->flag = -1;
		send_message(connectfd,msg);
		return -1;
	}

	msg->flag = 1;
	send_message(connectfd,msg);

	return 0;
}


int do_quit(int connectfd,msg_t *msg)
{
	close(connectfd);
	free(msg);
	exit(EXIT_SUCCESS);

	return 0;
}
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
int ser_init(sqlite3 **db)
{
	int sockfd ;
	char sql[BUFF_SIZE] ;
	char *errmsg ,buf[BUFF_SIZE];
	struct  sockaddr_in  ser_addr;


	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	ser_addr.sin_addr.s_addr  = inet_addr(SER_IP);


	if(-1 == (sockfd = socket(AF_INET,SOCK_STREAM,0)))
		err_sys("socket");

	if(-1 ==  bind(sockfd,(SA *)&ser_addr,sizeof(ser_addr) ))
		err_sys("bind");

	if(-1 == listen(sockfd,5))   // 5 * 1.5
		err_sys("listen");


	if(SQLITE_OK != sqlite3_open(DATA_BASE,db))
	{
		fprintf(stdout,"sqlite3_open : %s\n",sqlite3_errmsg(*db));
		return -1 ;
	}

	sprintf(sql,"create table %s(word text,disp text);",WORD_TABLE);
	if(SQLITE_OK != sqlite3_exec(*db,sql,NULL,NULL,&errmsg))
	{
		sprintf(buf,"table %s already exists",WORD_TABLE);
		if(strcmp(errmsg,buf) )
		{
			fprintf(stderr,"sqlite3_exec : %s\n",errmsg);
			return -1;
		}
	}
	sprintf(sql,"create table %s(name text ,word text,time text);",HIS_TABLE);
	if(SQLITE_OK != sqlite3_exec(*db,sql,NULL,NULL,&errmsg))
	{
		sprintf(buf,"table %s already exists",HIS_TABLE);
		if(strcmp(errmsg,buf) )
		{
			fprintf(stderr,"sqlite3_exec : %s\n",errmsg);
			return -1;
		}
	}

	sprintf(sql,"create table %s(name text ,passwd text);",USER_TABLE);
	if(SQLITE_OK != sqlite3_exec(*db,sql,NULL,NULL,&errmsg))
	{
		sprintf(buf,"table %s already exists",USER_TABLE);
		if(strcmp(errmsg,buf) )
		{
			fprintf(stderr,"sqlite3_exec : %s\n",errmsg);
			return -1;
		}
	}

#if 0
	do_loadword(*db);
#endif

	return sockfd ;
}

int do_client(int connectfd,sqlite3 *db)
{
	int n ;
	msg_t *msg = malloc(sizeof(msg_t));

	while(1)
	{
		if( -1 == ( n = recv(connectfd,msg,sizeof(msg_t),0)	))
			err_sys("recv");
		if(n == 0)
			break ;

		switch(msg->type)
		{
		case REGISTER:
			do_register(connectfd,msg,db);
			break;

		case LOGIN:
			do_login(connectfd,msg,db);
			break;

		case QUERRY:
			do_querry(connectfd,msg,db);
			break;

		case QUIT:
		case EXIT:
			do_quit(connectfd,msg);
			break;

		case HISTORY:
			do_history(connectfd,msg,db);
			break;

		case DEL_HISTORY:
			do_delhistory(connectfd,msg,db);
			break ;

		default:
			printf("invalid cmd\n");
			break;

		}
	}
	return 0;
}

int main(int argc, const char *argv[])
{

	int sockfd ,n,connectfd;
	struct sockaddr_in peer_addr ;
	pid_t pid ;
	socklen_t len = sizeof(peer_addr);

	sqlite3 *db ;

	sockfd = ser_init(&db);


	signal(SIGCHLD,SIG_IGN);

	printf("sockfd = %d\n",sockfd);

	while(1)
	{
		if(-1 == (connectfd  = accept(sockfd,(SA *)&peer_addr,&len)))
			err_sys("accept");

		printf("connectfd = %d\n",connectfd);

		printf("peer ip : %s peer port: %d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));

		if(-1 == (pid = fork()))
			err_sys("fork");

		else if(pid == 0)
		{
			do_client(connectfd,db);
			exit(EXIT_SUCCESS); //结束子进程
		}
		close(connectfd);
	}

	close(sockfd);


	return 0;
}

