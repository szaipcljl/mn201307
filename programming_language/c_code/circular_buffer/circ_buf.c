#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circ_buf.h"
#include <errno.h>
//#include <fifo.h>

#define min(a,b) ((a) < (b) ? (a):(b))

/*
 * 循环缓冲区在一些竞争问题上提供了一种免锁的机制，免锁的前提是，生产者和消费
 * 只有一个的情况下，否则也要加锁。下面就内核中提取出来，而经过修改后的fifo进
 * 简要的分析。
 */



/*
 * form kernel/kfifo.c
 */

/*
 * internal helper to calculate the unused elements in a fifo
 */
static inline unsigned int myfifo_unused(struct my_fifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}

/**
 * myfifo_init - initialize a fifo using a preallocated buffer
 * @fifo: the fifo to assign the buffer
 * @buffer: the preallocated buffer to be used
 * @size: the size of the internal buffer, this have to be a power of 2
 *
 * This function initialize a fifo using a preallocated buffer.
 *
 * The numer of elements will be rounded-up to a power of 2.
 * Return 0 if no error, otherwise an error code.
 */
int myfifo_init(struct my_fifo *fifo, void *buffer,
		unsigned int size, size_t esize)
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

/**
 * myfifo_alloc - dynamically allocates a new fifo buffer
 * @fifo: pointer to the fifo
 * @size: the number of elements in the fifo, this must be a power of 2
 * @esize:
 *
 * This function dynamically allocates a new fifo buffer.
 *
 * The numer of elements will be rounded-up to a power of 2.
 * The fifo will be release with myfifo_free().
 * Return 0 if no error, otherwise an error code.
 */
int myfifo_alloc(struct my_fifo *fifo, unsigned int size,
		size_t esize)
{
	/*
	 * round up to the next power of 2, since our 'let the indices
	 * wrap' tachnique works only in this case.
	 */
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

/*
 * myfifo_free
 */
void myfifo_free(struct my_fifo *fifo)
{
	free(fifo->data);
	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = 0;
	fifo->data = NULL;
	fifo->mask = 0;
}

/*
 *  myfifo_copy_in()
 */
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
	/*可能是缓冲区的空闲长度或者要写长度*/
	l = min(len, size - off);

	/* first put the data starting from fifo->in to buffer end*/
	memcpy(fifo->data + off, src, l);
	/* then put the rest (if any) at the beginning of the buffer*/
	memcpy(fifo->data, src + l, len - l);
	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
	//smp_wmb();
}

/**
 * myfifo_in - put data into the fifo
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @len: number of elements to be added
 *
 * This function copies the given buffer into the fifo and returns the
 * number of copied elements.
 */
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

/*
 * myfifo_copy_out
 */
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

	/* first get the data from fifo->out until the end of the buffer*/
	memcpy(dst, fifo->data + off, l);
	/* then get the rest (if any) from the beginning of the buffer*/
	memcpy(dst + l, fifo->data, len - l);
	/*
	 * make sure that the data is copied before
	 * incrementing the fifo->out index counter
	 */
	//smp_wmb();
}
/*
 * myfifo_out_peek - gets some data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @len: max. number of elements to get
 *
 * This function get the data from the fifo and return the numbers of elements
 * copied. The data is not removed from the fifo.
 */
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

/**
 * myfifo_out - get data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @len: max. number of elements to get
 *
 * This function get some data from the fifo and return the numbers of elements
 * copied.
 */
unsigned int myfifo_out(struct my_fifo *fifo,
		void *buf, unsigned int len)
{
	len = myfifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}
/*
   这两个读写结构才是循环缓冲区的重点。在fifo结构中，size是缓冲区的大小，是由用
   户自己定义的，但是在这个设计当中要求它的大小必须是2的幂次。
   当in==out时，表明缓冲区为空的，当(in-out)==size 时，说明缓冲区已满。

   我们看下具体实现，在96行处如果size-in+out ==0,也即获得的len值会0，而没有数
   据写入到缓冲区中。所以在设计缓冲区的大小的时候要恰当，读出的速度要比写入的速
   度要快，否则缓冲区满了会使数据丢失，可以通过成功写入的反回值来做判断尝试再次
   写入.
   另一种情况则是缓冲区有足够的空间给要写入的数据，但是试想一下，如果空闲的空间
   在缓冲的首尾两次，这又是如何实现呢？这部分代码实现得非常巧妙。
   我们看fifo->in &(fifo->size-1) 这个表达式是什么意思呢？我们知道size是2的幂次
   项，那么它减1即表示其值的二进制所有位都为1，与in相与的最终结果是in%size,比
   size要小，所以看in及out的值都是不断地增加，但再相与操作后，它们即是以size为
   周期的一个循环。98行就是比较要写入的数据应该是多少，如果缓冲区后面的还有足够
   的空间可写，那么把全部的值写到后面，否则写满后面，再写到前面去102行。

   读数据也可以作类似的分析，118行表示请求的数据要比缓冲区的数据要大时，只
   读取缓冲区中可用的数据。
   */

/* static inline */ void myfifo_reset(struct my_fifo *fifo)
{
	fifo->in = fifo->out = 0;
}

/* static inline */ unsigned int myfifo_used(struct my_fifo *fifo)
{
	return fifo->in - fifo->out;
}

//在头文件里还有缓冲区置位及返回缓冲区中数据大小两个函数。
