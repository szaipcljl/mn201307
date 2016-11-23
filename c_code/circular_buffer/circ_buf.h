#ifndef _CIRC_BUF_H
#define _CIRC_BUF_H

typedef struct _circ_buf_t {
	char a;
	int b;
	struct vector {
		int x;
		int y;
		int z;
	} my_vector;
} circ_buf_t;

struct my_fifo {
	circ_buf_t *buffer;/* the buffer holding the data*/
	unsigned int size;/* the size of the allocated buffer*/
	unsigned int in;/* data is added at offset (in % size)*/
	unsigned int out;/* data is extracted from off. (out % size)*/
};
//size, in, out are all unsigned，because they can't be negative。

struct my_fifo *my_fifo_init(circ_buf_t *buffer, unsigned int size);
struct my_fifo *my_fifo_alloc(unsigned int size);
void my_fifo_free(struct my_fifo *fifo);
unsigned int my_fifo_put(struct my_fifo *fifo,
		circ_buf_t *buffer, unsigned int len);
unsigned int my_fifo_get(struct my_fifo *fifo,
		circ_buf_t *buffer, unsigned int len);
void my_fifo_reset(struct my_fifo *fifo);
unsigned int my_fifo_len(struct my_fifo *fifo);
#endif
