#ifndef __PSH_CHT_SPI_EXTERN__
#define __PSH_CHT_SPI_EXTERN__

/*
  some function allow call outside of psh_cht_spi.

*/
#ifndef _IN_
#define _IN_
#endif

#ifndef _OUT_
#define _OUT_
#endif

#ifndef MIN
#define MIN(a,b)  ((a) <= (b) ? (a): (b))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) >= (b) ? (a): (b))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define ER_SUCCESS                     0
#define ER_FAILED                     -1          
#define ER_NO_ENOUGH_BUFFER_SIZE      -2
#define ER_IO_ERROR                   -3
#define ER_NO_ENOUGH_RECV_BUFFER      -4


typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;

#define IS_SUCCESS(x)  ((x) >= 0)
#define IS_FAILED(x)   ((x) < 0)


struct psh_ext_if;
int is_polling_worker_requested(struct psh_ext_if* lp_psh_if_info);

void request_start_polling_worker(struct psh_ext_if* lp_psh_if_info);

void request_stop_polling_worker(struct psh_ext_if* lp_psh_if_info);

void dump_buffer(void* lp_dumpbuffer, int size);


#define DBG_CMD_XFER          "xfer"
#define DBG_CMD_STRESS        "stress"
#define DBG_CMD_STREAM_START  "stream_start"
#define DBG_CMD_STREAM_STOP   "stream_stop"
#define DBG_CMD_STREAM_FREQ   "freq"
#define DBG_CMD_TS_SOURCE     "ts_source"
#define DBG_CMD_PWR           "pwr"

#endif
