#ifndef __PSH_CHT_SPI_H__
#define __PSH_CHT_SPI_H__

#include <linux/device.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/pci.h>
#include <linux/circ_buf.h>
#include <linux/completion.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include "psh_cht_spi_poller.h"
#include "psh_cht_spi_util.h"

#define DRV_POLLING_MODE

#define FRAME_HEAD_SIGN           ((u16)0x4853)
#define FRAME_HEAD_SIGN_PREFIX    ((u8)0x53)

#define IS_VALID_FRAME_HEAD(x)  (FRAME_HEAD_SIGN == (x)->sign)
#define SIZE_OF_FRAME_HEAD      (sizeof(struct frame_head))
#define INIT_FRAME_HEAD(_x, _length) {\
                                    (_x)->sign = FRAME_HEAD_SIGN;\
                                    (_x)->length = _length;\
                                   }

struct buffer
{
    u8* lp_ptr;
    int length;
    int index;
};
#define DECLAR_INIT_BUFFER(x, ptr, _length) struct buffer x = {.lp_ptr = ptr,\
                                                               .length = _length,\
                                                               .index = 0};

#define INIT_BUFFER(x, ptr, _length) (x)->lp_ptr = ptr;(x)->length = _length;(x)->index = 0;

#define DECLAR_BUFFER(x, _length) u8 x##_buffer[_length] = {0};          \
                                  DECLAR_INIT_BUFFER(x, x##_buffer, _length)


#define RESET_BUFFER(x) (x)->index = 0

#define BUFFER_REMAIN_LENGTH(x) ((x).length - (x).index)
#define BUFFER_USED_LENGTH(x)  ((x).length - BUFFER_REMAIN_LENGTH(x))
#define BUFFER_PTR(x) ((x).lp_ptr + (x).index)

#define BUFFER_PTR_FROM_TAIL(x, _tail_index)      ((x).lp_ptr + ((x).length - _tail_index))
#define BUFFER_PTR_FROM_USED_TAIL(x, _tail_index) ((x).lp_ptr + MAX((int)((x).index - _tail_index), 0))

struct spi_io_context
{
#define SPI_IO_DUMMY_BUFFER_SIZE   128
    u8 send_dummy_buffer[SPI_IO_DUMMY_BUFFER_SIZE];
    u8 recv_dummy_buffer[SPI_IO_DUMMY_BUFFER_SIZE];

    u8 send_dummy_buffer_size;
    u8 recv_dummy_buffer_size;
};
#define INIT_SPI_IO_CONTEXT(x)  {(x)->send_dummy_buffer_size = sizeof((x)->send_dummy_buffer); \
                                 (x)->recv_dummy_buffer_size = sizeof((x)->recv_dummy_buffer);}


/*for quick-dirty, must refine after deadline*/

struct send_list_entry
{
    struct list_head list_entry;

#define MAX_SEND_DATA_SIZE 256
    u8 data[MAX_SEND_DATA_SIZE];
    int used_size;
    int debug_index;
};

struct send_list
{
    struct mutex lock;

    /*each one is a send_list_entry*/
    struct list_head head;

    struct spi_io_context low_spi_io_context;   
};


/* need a global lock to check the psh driver access */
#define TASK_FLAG_CLEAR        0x0
#define TASK_FLAG_REQUEST_LOOP 0x1

struct psh_ext_if
{
	struct device *hwmon_dev;
	struct spi_device *pshc;

	u8  psh_frame[LBUF_MAX_CELL_SIZE];

#define SEND_ZERO_SIZE  16
    u8 zero_send_buffer[SEND_ZERO_SIZE];

	int gpio_psh_ctl, gpio_psh_rst;
	int gpio_psh_int;

	int irq_disabled;
	
	struct workqueue_struct *wq;
	
	struct work_struct work;

#ifdef DRV_POLLING_MODE	
	struct delayed_work dwork;
    struct mutex workitem_mutex;
    int task_flag;
#endif 

    struct psh_ia_priv* ia_data;	

    struct send_list send_data_list;

    struct poller poller_worker;

    struct timestamp_item io_profiler;
};


void insert_send_data_entry_to_list(struct psh_ext_if* lp_psh_if_info,
                                    struct send_list_entry* lp_new_entry);

struct send_list_entry* remove_send_data_entry_from_list(struct psh_ext_if* lp_psh_if_info);


int do_io_transaction(struct spi_device* lp_dev, 
                      _IN_ struct spi_io_context* lp_io_context,
                      _IN_ u8* const lp_send_buffer, int send_buffer_size,
                      _OUT_ u8* lp_recv_buffer, int recv_buffer_size, 
                      _OUT_ int* lp_recved_size );

int process_send_cmd_sync(struct psh_ia_priv *ia_data,
                          int ch,
                          struct ia_cmd *cmd, int len);


int process_send_buffer_sync(struct psh_ia_priv *ia_data,
                             int ch, void* lp_buffer, int len);

#endif
