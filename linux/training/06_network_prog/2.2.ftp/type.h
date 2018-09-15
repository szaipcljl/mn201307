#ifndef __TYPE_H__
#define __TYPE_H__

typedef struct {
	char cmd;
	char len;
	char data[64];
}send_data_t;

#define SA struct sockaddr
#define DATA 10
#define DATA_END 12
#define DATA_ERROR -11

#define SEND_START 50

#define DOWNLOAD 1
#define GET_UP 2
#define LIST_FILE 3
#define QUIT 4


#endif

