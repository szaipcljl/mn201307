#ifndef __STACK_H__
#define __STACK_H__

typedef int data_t;
#define  mstack_t struct mstack

extern mstack_t *creat_stack(int size);

extern int push_stack(mstack_t *sp,const data_t *data);
extern int pop_stack(mstack_t *sp,data_t *databuf);

extern int empty_stack(mstack_t *sp);
extern int full_stack(mstack_t *sp);

extern int clean_stack(mstack_t *sp);
extern int get_stack(mstack_t *sp,data_t *databuf);
extern int dis_stack(mstack_t **spp);

extern void print_stack(mstack_t *sp);

#endif
