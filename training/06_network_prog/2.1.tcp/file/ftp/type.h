#ifndef __TYPE_H__
#define __TYPE_H__

typedef struct {
	char cmd;
	char len;
	char data[64];
}send_data_t;

#define DATA 1
#define DATA_END 2
#define DATA_ERROR -1


#endif

