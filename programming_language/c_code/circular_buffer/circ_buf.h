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
	unsigned int in;/* data is added at offset (in % size)*/
	unsigned int out;/* data is extracted from off. (out % size)*/
	unsigned int mask;/* the size of the allocated buffer - 1*/
	unsigned int esize; //element size
	void *data;/* the buffer holding the data*/
};
//size, in, out are all unsigned，because they can't be negative。

int myfifo_init(struct my_fifo *fifo, void *buffer,
		unsigned int size, size_t esize);
int myfifo_alloc(struct my_fifo *fifo, unsigned int size,
		size_t esize);
void myfifo_free(struct my_fifo *fifo);
unsigned int myfifo_in(struct my_fifo *fifo,
		const void *buf, unsigned int len);
unsigned int myfifo_out(struct my_fifo *fifo,
		void *buf, unsigned int len);
void myfifo_reset(struct my_fifo *fifo);
unsigned int myfifo_used(struct my_fifo *fifo);
#endif
