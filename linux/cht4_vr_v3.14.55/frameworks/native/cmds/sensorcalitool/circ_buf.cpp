#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "circ_buf.h"
#define min(a,b) ((a) < (b) ? (a):(b))

static inline unsigned int myfifo_unused(struct my_fifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}

int myfifo_init(struct my_fifo *fifo, void *buffer,
		unsigned int size, unsigned int esize)
{
	size /= esize;

	//size = roundup_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;
	fifo->data = buffer;

	if (size < 2) {
		fifo->mask = 0;
		return -EINVAL;
	}
	fifo->mask = size - 1;

	return 0;
}

int myfifo_alloc(struct my_fifo *fifo, unsigned int size,
		unsigned int esize)
{
	//size = roundup_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;

	if (size < 2) {
		fifo->data = NULL;
		fifo->mask = 0;
		return -EINVAL;
	}

	fifo->data = malloc(size * esize);

	if (!fifo->data) {
		fifo->mask = 0;
		return -ENOMEM;
	}
	fifo->mask = size - 1;

	return 0;
}

void myfifo_free(struct my_fifo *fifo)
{
	free(fifo->data);
	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = 0;
	fifo->data = NULL;
	fifo->mask = 0;
}

void myfifo_copy_in(struct my_fifo *fifo, const void *src,
		unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);
}

unsigned int myfifo_in(struct my_fifo *fifo,
		const void *buf, unsigned int len)
{
	unsigned int l;

	l = myfifo_unused(fifo);
	if (len > l)
		len = l;

	myfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

static void myfifo_copy_out(struct my_fifo *fifo, void *dst,
		unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(dst, fifo->data + off, l);
	memcpy(dst + l, fifo->data, len - l);
}
unsigned int myfifo_out_peek(struct my_fifo *fifo,
		void *buf, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	myfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

unsigned int myfifo_out(struct my_fifo *fifo,
		void *buf, unsigned int len)
{
	len = myfifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}

void myfifo_reset(struct my_fifo *fifo)
{
	fifo->in = fifo->out = 0;
}

unsigned int myfifo_used(struct my_fifo *fifo)
{
	return fifo->in - fifo->out;
}

