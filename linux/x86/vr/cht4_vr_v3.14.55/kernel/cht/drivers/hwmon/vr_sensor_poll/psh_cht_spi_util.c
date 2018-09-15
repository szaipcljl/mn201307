#include <linux/ktime.h>
#include <linux/string.h>
#include "psh_cht_spi_util.h"
#include "psh_cht_spi_extern.h"


/*

  delcaration here

*/

static ssize_t ts_sysfs_control_store(struct device *dev,
                                      struct device_attribute *attr,
                                      const char *str, size_t count);

static ssize_t ts_sysfs_control_show(struct device *dev,
                                     struct device_attribute *attr, char *buf);


static ssize_t ts_sysfs_data_read(struct file *file, struct kobject *kobj,
                                  struct bin_attribute *attr, char *buf,
                                  loff_t off, size_t count);



/*
  the atomic64_set may not work as my expected on x86-64, only know that it wokred on IA64.
  but for this util which work with very limit concurrency, it should still work well
*/

static void save_to_dist_array(atomic64_t* lp_array, long result)
{
    int array_index;

    array_index = (int)
        (1000000000.0 / result);        
    
    if (array_index > MAX_DIST_INDEX)
    {
        array_index = MAX_DIST_INDEX;
    }

    atomic64_inc(lp_array+array_index);
}


static void timestamp_init(struct timestamp_item* lp_ts_item)
{
    int loop_i;

    if (!lp_ts_item)
    {return;}
    
    lp_ts_item->data.version = TS_DATA_VERSION_1;
    lp_ts_item->data.reserved = 0;

    atomic64_set(&lp_ts_item->data.count, 0);
    atomic64_set(&lp_ts_item->data.total_delta, 0);
    atomic64_set(&lp_ts_item->data.delta_max, 0);
    atomic64_set(&lp_ts_item->data.delta_min, LONG_MAX);

    atomic64_set(&lp_ts_item->flag, TS_FLAG_INIT_VALUE);

    memset(&lp_ts_item->begin, 0, sizeof(lp_ts_item->begin));
    memset(&lp_ts_item->end, 0, sizeof(lp_ts_item->end));

    for (loop_i = 0; loop_i < MAX_DIST; ++loop_i)
    {
        atomic64_set(lp_ts_item->data.dist_array + loop_i, 0);
    }
}


static void build_sysfs_name(char* lp_buff, int count,
                     char* lp_name, char* lp_suffix)
{
    if (lp_buff && lp_name && lp_suffix)
    {
        memset(lp_buff, count, 0);
        strcpy(lp_buff, lp_name);
        strcat(lp_buff, lp_suffix);

        pr_err("%s: build path name: %s\n",
               __func__, lp_buff);
        return;
    }

    pr_err("%s: build sysfs interface file name failed! \n",
           __func__);

    return;
}


static void build_sysfs_control_attribute(struct timestamp_item* lp_ts_item,
                                      char* lp_base_name)
{
    build_sysfs_name(lp_ts_item->sysfs_control_path, 
                     sizeof(lp_ts_item->sysfs_control_path),
                     lp_base_name,
                     TS_SYSFS_CONTROL_SUFFIX);

    lp_ts_item->sysfs_control.attr.name = lp_ts_item->sysfs_control_path;
    lp_ts_item->sysfs_control.attr.mode = S_IRUSR|S_IWUSR;

    lp_ts_item->sysfs_control.store = ts_sysfs_control_store;
    lp_ts_item->sysfs_control.show = ts_sysfs_control_show;
}


static void build_sysfs_data_attribute(struct timestamp_item* lp_ts_item,
                                          char* lp_base_name)
{
    build_sysfs_name(lp_ts_item->sysfs_data_path,
                     sizeof(lp_ts_item->sysfs_data_path),
                     lp_base_name,
                     TS_SYSFS_DATA_SUFFIX);

    lp_ts_item->sysfs_data.attr.name = lp_ts_item->sysfs_data_path;
    lp_ts_item->sysfs_data.attr.mode = S_IRUSR;

    lp_ts_item->sysfs_data.read = ts_sysfs_data_read;
    lp_ts_item->sysfs_data.write = NULL;
    lp_ts_item->sysfs_data.mmap = NULL;
    lp_ts_item->sysfs_data.private = NULL;

    lp_ts_item->sysfs_data.size = sizeof(struct timestamp_item_data);
}


static int create_sysfs_interface(struct device* lp_dev, char* lp_base_name,
                                  struct timestamp_item* lp_ts_item)
{
    int ret_value;
    
    build_sysfs_control_attribute(lp_ts_item,
                                  lp_base_name);
    ret_value = sysfs_create_file(&lp_dev->kobj,
                                  &lp_ts_item->sysfs_control.attr);
    if (ret_value >= 0)
    {
        build_sysfs_data_attribute(lp_ts_item,
                                   lp_base_name);
        ret_value = sysfs_create_bin_file(&lp_dev->kobj,
                                          &lp_ts_item->sysfs_data);
    }

    return ret_value;
}


static void remove_sysfs_interface(struct device* lp_dev,
                                   struct timestamp_item* lp_ts_item)
{
    sysfs_remove_file(&lp_dev->kobj,
                      &lp_ts_item->sysfs_control.attr);

    sysfs_remove_bin_file(&lp_dev->kobj,
                          &lp_ts_item->sysfs_data);
}

int timestamp_init_with_name(struct device* lp_dev,
                             struct timestamp_item* lp_ts_item,
                             char* lp_name)
{
    int ret_value;

    if (!lp_ts_item || !lp_name || !lp_dev)
    {return -1;}

    timestamp_init(lp_ts_item);
    
    ret_value = create_sysfs_interface(lp_dev, lp_name, lp_ts_item);

    if (ret_value < 0)
    {
        remove_sysfs_interface(lp_dev, lp_ts_item);
    }

    return ret_value;
}


void timestamp_destory(struct device* lp_dev,
                       struct timestamp_item* lp_ts_item)
{
    if (!lp_ts_item)
    {return;}

    remove_sysfs_interface(lp_dev, lp_ts_item);
}

void timestamp_record_begin(struct timestamp_item* lp_ts_item)
{
    if (!lp_ts_item || timestamp_is_disabled(lp_ts_item)) 
    {return;}

    ktime_get_ts(&lp_ts_item->begin);    
}


void timestamp_record_end(struct timestamp_item* lp_ts_item)
{
    long result;
    long begin;

    if (!lp_ts_item || timestamp_is_disabled(lp_ts_item))
    {return;}

    begin = timespec_to_ns(&lp_ts_item->begin);
    ktime_get_ts(&lp_ts_item->end);

    result = timespec_to_ns(&lp_ts_item->end) - begin;

    if (result < 0 || 0 == begin)
    {
        return;
    }

    if (result > atomic64_read(&lp_ts_item->data.delta_max))
    {
        atomic64_set(&lp_ts_item->data.delta_max, result);
    }

    if (result < atomic64_read(&lp_ts_item->data.delta_min))
    {
        atomic64_set(&lp_ts_item->data.delta_min, result);
    }

    if (result)
    {
        save_to_dist_array(lp_ts_item->data.dist_array, result);
    }

    atomic64_set(&lp_ts_item->data.delta_now, result);
    atomic64_add(result, &lp_ts_item->data.total_delta);
    atomic64_inc(&lp_ts_item->data.count);
}

void timestamp_record_loop(struct timestamp_item* lp_ts_item)
{
    if (0 == lp_ts_item->begin.tv_nsec
        && 0 == lp_ts_item->begin.tv_sec)
    {
        timestamp_record_begin(lp_ts_item);
        return; 
    }

    timestamp_record_end(lp_ts_item);
    timestamp_record_begin(lp_ts_item);

    return;
}

void timestamp_get_result(struct timestamp_item* lp_ts_item,
                          struct timestamp_result* lp_ts_result)
{
    if (!lp_ts_item || !lp_ts_result)
    {return;}

    lp_ts_result->count = atomic64_read(&lp_ts_item->data.count);
    lp_ts_result->total_delta = atomic64_read(&lp_ts_item->data.total_delta);
    lp_ts_result->delta_max = atomic64_read(&lp_ts_item->data.delta_max);
    lp_ts_result->delta_min = atomic64_read(&lp_ts_item->data.delta_min);
}


int timestamp_get_binary_result(struct timestamp_item* lp_ts_item,
                                char *buf,size_t count,
                                loff_t off)
{
    unsigned char* lp_copy_ptr;
    int copy_size = 0;

    if (!lp_ts_item || !buf)
    {return -1;}

    lp_copy_ptr = ((unsigned char*)&lp_ts_item->data) + off;
    copy_size = MIN(sizeof(lp_ts_item->data) - off, count);

    if (copy_size > count)
    {
        pr_err("%s count = %zu, require %d. copy binary profile data failed\n",
                __func__, count, copy_size);

        return 0;
    }
        
    if (copy_size)
    {
        memcpy(buf, lp_copy_ptr, copy_size);
    }

    pr_err("%s count = %zu, require= %d. off = %lld\n",
            __func__, count, copy_size, off);

    return copy_size;
}


void timestamp_enable(struct timestamp_item* lp_ts_item)
{
    if (!lp_ts_item)
    {return;}

    atomic64_set(&lp_ts_item->flag, TS_FLAG_ENABLE);
}

void timestamp_disable(struct timestamp_item* lp_ts_item)
{
    if (!lp_ts_item)
    {return;}

    atomic64_set(&lp_ts_item->flag, TS_FLAG_DISABLE);
}


int timestamp_is_disabled(struct timestamp_item* lp_ts_item)
{
    long flag = atomic64_read(&lp_ts_item->flag);

    if (flag & TS_FLAG_ENABLE)
    {
        return FALSE;
    }

    return TRUE;
}


static int parse_new_state_enable_disable(const char* str, size_t count,
                                          int* lp_new_state)
{
    int ret_value;
    int loop_i, loop_end;
    int str_len;

    const static struct 
    {
        char* lp_state_str;
        int state_value;
    }str2value[] = 
         {
             {"enable",  TS_FLAG_ENABLE},
             {"disable", TS_FLAG_DISABLE},
             {"1",       TS_FLAG_ENABLE},
             {"0",       TS_FLAG_DISABLE},
             {"true",    TS_FLAG_ENABLE},
             {"false",   TS_FLAG_DISABLE},
         };

    loop_end = sizeof(str2value)/sizeof(str2value[0]);

    for (loop_i = 0; loop_i < loop_end; ++loop_i)
    {
        str_len =  strlen(str2value[loop_i].lp_state_str);

        ret_value = strncasecmp(str, str2value[loop_i].lp_state_str,
                                MIN(count, str_len)
                                );

        if (!ret_value)
        {
            *lp_new_state = str2value[loop_i].state_value;
            return 0;
        }
    }

    return -1;
}


static ssize_t ts_sysfs_control_store(struct device *dev,
                                      struct device_attribute *attr,
                                      const char *str, size_t count)
{
    struct timestamp_item* lp_ts_item;
    int new_state;
    int ret_value;

    lp_ts_item = container_of(attr, struct timestamp_item, sysfs_control);

    ret_value = parse_new_state_enable_disable(str, strlen(str), &new_state);

    if (ret_value >= 0)
    {
        switch(new_state)
        {
        case TS_FLAG_DISABLE:
            timestamp_disable(lp_ts_item);
            break;
        case TS_FLAG_ENABLE:
            timestamp_enable(lp_ts_item);
            break;
        default:
            break;
        }
    }

	return count;
}


static ssize_t ts_sysfs_control_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
    ssize_t ret_value;

    struct timestamp_item* lp_ts_item;

    lp_ts_item = container_of(attr, struct timestamp_item, sysfs_control);

    ret_value = snprintf(buf, PAGE_SIZE,
                         "enable state: %s\n",
                         timestamp_is_disabled(lp_ts_item) ? "disable" : "enable");

    return ret_value;
}


static ssize_t ts_sysfs_data_read(struct file *file, struct kobject *kobj,
                                  struct bin_attribute *attr, char *buf,
                                  loff_t off, size_t count)
{
    struct timestamp_item* lp_ts_item;
    ssize_t ret_value;

    lp_ts_item = container_of(attr, struct timestamp_item, sysfs_data);

    ret_value = timestamp_get_binary_result(lp_ts_item,
                                            buf, count,
                                            off);
    return ret_value;
}
