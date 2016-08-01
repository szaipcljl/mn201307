#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "mytype.h"
#define queue_t struct queue


extern queue_t *creat_queue(int size);
extern int in_queue(queue_t *sqp,const data_t *data);
extern int out_queue(queue_t *sqp,data_t *databuf);

extern int get_front_queue(queue_t *sqp,data_t *databuf);
extern int empty_queue(queue_t *sqp);
extern int full_queue(queue_t *sqp);
extern int length_queue(queue_t *sqp);
extern int clean_queue(queue_t *sqp);
extern int dis_queue(queue_t **sqpp);
extern void print_queue(queue_t *sqp);

#endif

