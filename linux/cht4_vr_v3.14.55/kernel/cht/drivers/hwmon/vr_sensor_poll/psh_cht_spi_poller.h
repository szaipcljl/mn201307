#ifndef __PSH_CHT_SPI_POLLER_H__
#define __PSH_CHT_SPI_POLLER_H__

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/ktime.h>

typedef void (*poller_fn)(void* lp_parameter);


enum poller_state
{
    POLLER_INIT = 0,
    POLLER_RUN,
    POLLER_STOP
};


enum poller_sub_state
{
    POLLER_SUB_STATE_S0,
    POLLER_SUB_STATE_S3,
};

struct poller
{
    struct task_struct* lp_thread;
    poller_fn worker_callback;
    void* lp_parameter;

    struct mutex queue_lock;
    struct list_head cmd_queue;
    atomic_t state;
    atomic_t sub_state;

    struct timespec begin_ts;
    struct timespec end_ts;

    atomic64_t period_time;
};

void poller_init(struct poller* lp_poller,
                 poller_fn callback, void* lp_parameter);

int poller_start(struct device* lp_dev, struct poller* lp_poller);

int poller_pause(struct device* lp_dev, struct poller* lp_poller);

int poller_resume(struct device* lp_dev, struct poller* lp_poller);

int poller_stop(struct device* lp_dev, struct poller* lp_poller);

int poller_set_frequency(struct poller* lp_poller, long new_frequency);


#endif
