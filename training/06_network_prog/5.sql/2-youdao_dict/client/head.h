#ifndef _HEAD_H__
#define _HEAD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "sqlite3.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <termios.h>

#include <ncurses.h>
#include <unistd.h>

#define CLI_PORT 50003
#define CLI_IP "127.0.0.1"

#define ECHOFLAGS (ECHO | ECHOK | ECHONL)

#define DATA_BASE "./my.db"
#define DICT_TABLE "word"
#define USER_TABLE "user"
#define HIS_TABLE  "history"

#define DICT_FILE  "./dict.txt"

#define err_quit(_errmsg) do{perror(_errmsg);exit(EXIT_FAILURE);}while(0)
#define err_sys(_errmsg) do{printf("error:%s\n",_errmsg);exit(EXIT_FAILURE);}while(0)

#define SER_IP "127.0.0.1"
#define SER_PORT 50000

typedef struct sockaddr SA ;

#define NAME_LEN 32
#define DATA_LEN 512
#define PASSWD_LEN 128
#define BUFF_SIZE 1024

typedef struct _MSG_
{
	union{
	int type;
	int flag;
	}flg;

#define flag flg.flag
#define type flg.type

	char name[NAME_LEN];
	union{
	char data[DATA_LEN];
	char passwd[PASSWD_LEN];
	}dat;
#define passwd dat.passwd
#define data   dat.data
}msg_t ;


typedef struct _PARAMA_
{
	int connectfd;
	msg_t *msg ;
}PARAME;

#define REGISTER     1
#define LOGIN        2
#define QUIT  3


#define QUERRY       4
#define HISTORY      5
#define DEL_HISTORY  6
#define EXIT  7

#define DATA_BASE  "./my.db"
#define WORD_TABLE  "word_table"
#define DICT  "./dict.txt"

#define BUFF_SIZE 1024



#endif

