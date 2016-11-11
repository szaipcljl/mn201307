#include "psh_cht_spi_poller.h" 
#include "psh_cht_spi_extern.h"
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sched/rt.h>
#include <linux/compiler.h>
#include <linux/hrtimer.h>

#define  _1s_in_ns  1000000000

enum poller_cmd_id
{
    POLLER_CMD_REQUEST_PAUSE,
    POLLER_CMD_REQUEST_RESUME,
    POLLER_CMD_REQUEST_EXIT,
};

struct poller_cmd
{
    uint cmd;
    void* lp_parameter;

    struct list_head link;
};



static struct poller_cmd* 
alloc_new_cmd_node(uint cmd_id, void* lp_parameter)
{
    struct poller_cmd* lp_new_cmd;

    lp_new_cmd = kzalloc(sizeof(*lp_new_cmd), GFP_KERNEL);
    if (lp_new_cmd)
    {
        lp_new_cmd->cmd = cmd_id;
        lp_new_cmd->lp_parameter = lp_parameter;
    }

    return lp_new_cmd;
}

void free_cmd_node(struct poller_cmd* lp_cmd)
{
    if (lp_cmd)
    {
        kfree(lp_cmd);
    }
}

static struct poller_cmd* get_cmd_node_from_queue(struct list_head* lp_cmd_queue)
{
    struct poller_cmd* lp_cmd_node;

    lp_cmd_node = NULL;
    if (! list_empty(lp_cmd_queue) )
    {
        lp_cmd_node = list_entry(lp_cmd_queue->next,
                                 struct poller_cmd,
                                 link);

        list_del(lp_cmd_queue->next);
    }

    return lp_cmd_node;
}

static void put_cmd_node_into_queue(struct list_head* lp_cmd_queue, 
                                    struct poller_cmd* lp_new_cmd)
{
    list_add(&lp_new_cmd->link, lp_cmd_queue->prev);   
}


static void pm_disable_suspend(struct device* lp_dev)
{
    /*
#ifdef ENABLE_RPM
    pm_runtime_get_sync(lp_dev);
#endif
    */ 
}


static void pm_enable_suspend(struct device* lp_dev)
{
    /*
#ifdef ENABLE_RPM
	pm_runtime_mark_last_busy(lp_dev);
	pm_runtime_put_autosuspend(lp_dev);
#endif
    */
}


static void frequency_controller(struct poller* lp_poller, long request_period_time)
{
    long each_period_time = timespec_to_ns(&lp_poller->end_ts) 
                            - timespec_to_ns(&lp_poller->begin_ts);
                            

    long delta = request_period_time - each_period_time;

    if (delta > 0)
    {
        struct timespec hold_time;
        
        hold_time.tv_sec = 0;
        hold_time.tv_nsec = delta;

        hrtimer_nanosleep(&hold_time, NULL, HRTIMER_MODE_REL, CLOCK_MONOTONIC);
    }
     
    return;
}

#if 1
static int  poller_thread(void* lp_param)
{
    struct poller* lp_poller = lp_param;
    struct poller_cmd* lp_new_cmd;

    poller_fn callback = lp_poller->worker_callback;
    void* lp_parameter = lp_poller->lp_parameter;

    int sub_state;
    int state;

    long request_period_time;

    atomic_set(&lp_poller->state, POLLER_RUN);
    atomic_set(&lp_poller->sub_state, POLLER_SUB_STATE_S0);

    sub_state = atomic_read(&lp_poller->sub_state);
    state = atomic_read(&lp_poller->state);

    while(POLLER_RUN == state)
    {
        request_period_time = atomic64_read(&lp_poller->period_time);
        
        if (0 != request_period_time)
        {
            ktime_get_ts(&lp_poller->begin_ts);
        }
        
        if (likely(POLLER_SUB_STATE_S0 == sub_state))
        {
            callback(lp_parameter);
        }
        else
        {
            msleep(10);
        }
        /* check if has queue cmd:  */

        mutex_lock(&lp_poller->queue_lock);

        lp_new_cmd = get_cmd_node_from_queue(&lp_poller->cmd_queue);

        mutex_unlock(&lp_poller->queue_lock);

        if (lp_new_cmd)
        {
            switch(lp_new_cmd->cmd)
            {
            case POLLER_CMD_REQUEST_PAUSE:
                atomic_set(&lp_poller->sub_state, POLLER_SUB_STATE_S3);
                sub_state = POLLER_SUB_STATE_S3;
                break;
            case POLLER_CMD_REQUEST_RESUME:
                atomic_set(&lp_poller->sub_state, POLLER_SUB_STATE_S0);
                sub_state = POLLER_SUB_STATE_S0;
                break;
            case POLLER_CMD_REQUEST_EXIT:
                atomic_set(&lp_poller->state, POLLER_STOP);
                state = POLLER_STOP;
                break;
            default:
                break;
            }

            free_cmd_node(lp_new_cmd);
        }
        
        if (0 != request_period_time)
        {
            ktime_get_ts(&lp_poller->end_ts);            
            frequency_controller(lp_poller, request_period_time);
        }
    }

    atomic_set(&lp_poller->state, POLLER_STOP);

    return 0;
}
#else
static int  poller_thread(void* lp_param)
{
    while(1)
    {
        printk(KERN_ERR "poller_thread run\n");
    }
}
#endif
void poller_init(struct poller* lp_poller,
                 poller_fn callback, void* lp_parameter)
{
    if (lp_poller)
    {
        mutex_init(&lp_poller->queue_lock);

        lp_poller->worker_callback = callback;
        lp_poller->lp_parameter = lp_parameter;    
        lp_poller->lp_thread = NULL;

        atomic_set(&lp_poller->state, POLLER_STOP);
        atomic_set(&lp_poller->sub_state, POLLER_SUB_STATE_S3);

        atomic64_set(&lp_poller->period_time, 0);

        memset(&lp_poller->begin_ts, 0, sizeof(lp_poller->begin_ts));
        memset(&lp_poller->end_ts, 0, sizeof(lp_poller->end_ts));

        INIT_LIST_HEAD(&lp_poller->cmd_queue);
    }
}

int poller_start(struct device* lp_dev,
                 struct poller* lp_poller)
{
    int ret_value;
    struct sched_param param;

    if (!lp_poller || !lp_poller->worker_callback
        || !lp_dev)
    {
        return ER_FAILED;
    }

    if (POLLER_RUN == atomic_read(&lp_poller->state))
    {
        return ER_SUCCESS;
    }

    lp_poller->lp_thread = kthread_run(poller_thread,
                                       lp_poller, "%s", "poller_thread");
    if (lp_poller->lp_thread)
    {     
        pm_disable_suspend(lp_dev);

        //want more...
        param.sched_priority = MAX_RT_PRIO / 4;
        ret_value = sched_setscheduler(lp_poller->lp_thread,
                                       SCHED_FIFO,
                                       &param);

        printk(KERN_ERR "poller priority ret: %d\n", ret_value);
    }

    return lp_poller->lp_thread ? ER_SUCCESS:ER_FAILED;
}

int poller_pause(struct device* lp_dev,
                 struct poller* lp_poller)
{
    struct poller_cmd* lp_new_cmd;

    if (!lp_dev || !lp_poller)
    {
        return ER_FAILED;
    }

    if (!lp_poller->lp_thread
        || POLLER_SUB_STATE_S3 == atomic_read(&lp_poller->sub_state))
    {
        return ER_SUCCESS;
    }

    lp_new_cmd = alloc_new_cmd_node(POLLER_CMD_REQUEST_PAUSE, NULL);
    if (lp_new_cmd)
    {
        mutex_lock(&lp_poller->queue_lock);

        put_cmd_node_into_queue(&lp_poller->cmd_queue,
                                lp_new_cmd);

        mutex_unlock(&lp_poller->queue_lock);

        /*keep wait until the work thread paused*/
        while(POLLER_SUB_STATE_S3 != atomic_read(&lp_poller->sub_state))
        {
            msleep(5);
        }

        pm_enable_suspend(lp_dev);

        return ER_SUCCESS;
    }

    return ER_FAILED;
}

int poller_resume(struct device* lp_dev,
                  struct poller* lp_poller)
{
    struct poller_cmd* lp_new_cmd;

    if (!lp_dev || !lp_poller)
    {
        return ER_FAILED;
    }

    if (!lp_poller->lp_thread
        || POLLER_SUB_STATE_S0 == atomic_read(&lp_poller->sub_state))
    {
        return ER_SUCCESS;
    }

    lp_new_cmd = alloc_new_cmd_node(POLLER_CMD_REQUEST_RESUME, NULL);
    if (lp_new_cmd)
    {
        mutex_lock(&lp_poller->queue_lock);

        put_cmd_node_into_queue(&lp_poller->cmd_queue,
                                lp_new_cmd);

        mutex_unlock(&lp_poller->queue_lock);

        while(POLLER_SUB_STATE_S0 != atomic_read(&lp_poller->sub_state))
        {
            msleep(5);
        }

        pm_disable_suspend(lp_dev);

        return ER_SUCCESS;
    }

    return ER_FAILED;
}


int poller_stop(struct device* lp_dev,
                struct poller* lp_poller)
{
    struct poller_cmd* lp_new_cmd;

    if (!lp_dev || !lp_poller)
    {
        return ER_FAILED;
    }

    if (!lp_poller->lp_thread 
        || POLLER_STOP == atomic_read(&lp_poller->state))
    {
        return ER_SUCCESS;
    }

    lp_new_cmd = alloc_new_cmd_node(POLLER_CMD_REQUEST_EXIT, NULL);
    if (lp_new_cmd)
    {
        mutex_lock(&lp_poller->queue_lock);

        put_cmd_node_into_queue(&lp_poller->cmd_queue,
                                lp_new_cmd);

        mutex_unlock(&lp_poller->queue_lock);

        /*keep wait until the work thread exit*/
        while(POLLER_STOP != atomic_read(&lp_poller->state))
        {
            msleep(5);
        }

        pm_enable_suspend(lp_dev);

        return ER_SUCCESS;
    }

    return ER_FAILED;
}


int poller_set_frequency(struct poller* lp_poller, long new_frequency)
{
    long new_period;

    if (!lp_poller)
    {
        return ER_FAILED;
    }

    if (0 == new_frequency)
    {
        new_period = 0;
    }
    else
    {
        new_period = _1s_in_ns / new_frequency;
    }

    atomic64_set(&lp_poller->period_time, new_period);

    return ER_SUCCESS;
}
