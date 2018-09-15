#ifndef __PSH_CHT_SPI_UTIL__
#define __PSH_CHT_SPI_UTIL__

#ifdef __KERNEL__

#include <linux/time.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#else

#include <linux/time.h>
/*
  some defination used for user space parser only
 */
typedef struct {
	long counter;
} atomic64_t;

#define atomic64_read(x) (x).counter

/*#ifdef __KERNEL__*/
#endif

#define MAX_DIST 3002
#define MIN_DIST_INDEX (1)
#define MAX_DIST_INDEX (MAX_DIST - 1)

#define TS_DATA_VERSION_1  0x1

#define TS_SYSFS_DATA_SUFFIX    "_data"
#define TS_SYSFS_CONTROL_SUFFIX "_control"

struct timestamp_item_data
{
    int version;
    int reserved;

    atomic64_t count;
    atomic64_t total_delta;

    atomic64_t delta_max;
    atomic64_t delta_min;
    atomic64_t delta_now;

    atomic64_t dist_array[MAX_DIST];
};


#ifdef __KERNEL__

enum
{
    TS_FLAG_ENABLE = 0x1,
    TS_FLAG_DISABLE = 0x0,
};

#define TS_FLAG_INIT_VALUE  TS_FLAG_DISABLE
#define TS_NAME_MAX         100
#define TS_NAME_PREFIX_MAX  32

struct timestamp_item
{
    struct timespec begin;
    struct timespec end;

    atomic64_t flag;

    char sysfs_control_path[TS_NAME_MAX];
    char sysfs_data_path[TS_NAME_MAX];

    struct bin_attribute sysfs_data;
    struct device_attribute sysfs_control;

    struct timestamp_item_data data;
};

struct timestamp_result
{
    long count;
    long total_delta;
    long delta_max;
    long delta_min;
};

int timestamp_init_with_name(struct device* lp_dev,
                             struct timestamp_item* lp_ts_item,
                             char* lp_name);


void timestamp_destory(struct device* lp_dev,
                       struct timestamp_item* lp_ts_item);


void timestamp_record_begin(struct timestamp_item* lp_ts_item);



void timestamp_record_end(struct timestamp_item* lp_ts_item);



void timestamp_record_loop(struct timestamp_item* lp_ts_item);



void timestamp_get_result(struct timestamp_item* lp_ts_item,
                          struct timestamp_result* lp_ts_result);



int timestamp_get_binary_result(struct timestamp_item* lp_ts_item,                                
                                char *buf,size_t count,
                                loff_t off);

void timestamp_enable(struct timestamp_item* lp_ts_item);



void timestamp_disable(struct timestamp_item* lp_ts_item);



int timestamp_is_disabled(struct timestamp_item* lp_ts_item);



#define GET_TIMESTAMP_NANO(_var_64)              \
    {                                            \
        struct timespec __current_ts;            \
        ktime_get_ts(&__current_ts);             \
        _var_64 = timespec_to_ns(&__current_ts); \
    }

#endif


#endif
