
/*
 *  psh.c - Cheerytrail PSH IA side driver
 *
 *  (C) Copyright 2016 Intel Corporation
 *  Author: Alek Du <alek.du@intel.com>
 *          Shuang Liu <shuang.liu@intel.com> 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301, USA
 */

/*
 * PSH IA side driver for Cherrytrail Platform
 */

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
#include <linux/sched.h>

//#include <linux/acpi_gpio.h>
#include <linux/pm_runtime.h>
//#include <asm/intel_vlv2.h>
#include <linux/workqueue.h>

#include "psh_ia_common.h"
#include "psh_cht_spi_extern.h"
#include "psh_cht_spi.h"
#include "psh_cht_spi_test.h"

#define GPIO_PSH_INT ((int)275)
#define GPIO_PSH_MCU_RESET (341)

#define POLL_HZ_PER_SECOND   1000
#define POLLING_HZ (HZ/POLL_HZ_PER_SECOND)


static int no_err(const struct device *dev, const char *fmt, ...)
{ return 0; }
 
#ifdef dev_err
#undef dev_err
#define dev_err no_err
#endif

#ifdef dev_info
#undef dev_info
#define dev_info no_err
#endif



int is_polling_worker_requested(struct psh_ext_if* lp_psh_if_info)
{
    return FALSE;
    //    return (lp_psh_if_info->task_flag & TASK_FLAG_REQUEST_LOOP);
}


void request_start_polling_worker(struct psh_ext_if* lp_psh_if_info)
{
    lp_psh_if_info->task_flag |= TASK_FLAG_REQUEST_LOOP;

    //    queue_delayed_work(lp_psh_if_info->wq, &lp_psh_if_info->dwork, POLLING_HZ);
    //    atomic_inc(&lp_psh_if_info->pending_polling_request);
}


void request_stop_polling_worker(struct psh_ext_if* lp_psh_if_info)
{
    //lp_psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);

    //cancel_delayed_work_sync(&lp_psh_if_info->dwork);
    //atomic_set(&lp_psh_if_info->pending_polling_request, 0);
}


int read_psh_data(struct psh_ia_priv *ia_data)
{
#if 0
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;
	int cur_read = 0, ret = 0;
	struct frame_head fh;
	struct timespec t1,t2,t3,t4;	

	struct spi_message msg;
	
	struct spi_transfer xfer_fh = {
	
		.rx_buf		= (void *)&fh,
		.len		= sizeof(fh)
	
	};

	struct spi_transfer xfer_payload = {
		.rx_buf = (void *)&psh_if_info->psh_frame
	};
	
    int gpio_val = -1;
	int sequent_dummy = 0;
	static int loop = 0;

#ifdef ENABLE_RPM
	/* We may need to zero all the buffer */
	pm_runtime_get_sync(&psh_if_info->pshc->dev);
#endif 
	psh_if_info->gpio_psh_int = GPIO_PSH_INT;

	gpio_val = gpio_get_value(psh_if_info->gpio_psh_int);

	dev_dbg(&psh_if_info->pshc->dev, "%s, gpio_val=%d\n", __func__, gpio_val);
	
	/* Loop read till error or no more data */
	while (!gpio_get_value(psh_if_info->gpio_psh_int)) {
		
		char *ptr;
		int len;

		if (ia_data->cmd_in_progress == CMD_RESET)
			break;
		else if (ia_data->cmd_in_progress != CMD_INVALID)
			schedule();

		if (sequent_dummy >= 2) {
			/* something wrong, check FW */
			dev_dbg(&psh_if_info->pshc->dev,
				"2 sequent dummy frame header read!");
			break;
		}

		spi_message_init(&msg);
		spi_message_add_tail(&xfer_fh, &msg);

		ktime_get_ts(&t1);
		ret = spi_sync(psh_if_info->pshc, &msg);
		ktime_get_ts(&t3);
		
		if (ret) {
			dev_err(&psh_if_info->pshc->dev, "Read frame header error!"
					" ret=%d\n", ret);
			loop++;
			break;
		}

		dev_dbg(&psh_if_info->pshc->dev, "sign=0x%x(0x4853), len=%d\n", fh.sign, fh.length);

		if (fh.sign == LBUF_CELL_SIGN) {
			if (fh.length > LBUF_MAX_CELL_SIZE) {
				dev_err(&psh_if_info->pshc->dev, "frame size is too big!\n");
				ret = -EPERM;
				break;
			}
			sequent_dummy = 0;
		} else {
			if (fh.sign || fh.length) {
				dev_err(&psh_if_info->pshc->dev, "wrong fh (0x%x, 0x%x)\n",
						fh.sign, fh.length);
				ret = -EPERM;
				break;
			}
			sequent_dummy++;
			continue;
		}

		//len = frame_size(fh.length) - sizeof(fh);
		len = fh.length;
		xfer_payload.len = len;

		dev_dbg(&psh_if_info->pshc->dev, "%s xfer_payload.len=%d\n", __func__, len);
	
		
		spi_message_init(&msg);
		spi_message_add_tail(&xfer_payload, &msg);
		ret = spi_sync(psh_if_info->pshc, &msg);
		
		if (ret) {
			dev_err(&psh_if_info->pshc->dev, "Read main frame error!"
				   " ret=%d\n", ret);
			break;
		}

		ptr = psh_if_info->psh_frame;
		
		ktime_get_ts(&t4);
		//dump_cmd_resp(ptr, len);
		
		while (len > 0) {
			struct cmd_resp *resp = (struct cmd_resp *)ptr;
			u32 size = sizeof(*resp) + resp->data_len;

			ret = ia_handle_frame(ia_data, ptr, size);
			if (ret > 0) {
				cur_read += ret;

				if (cur_read > 250) {
					cur_read = 0;
					sysfs_notify(&psh_if_info->pshc->dev.kobj,
						NULL, "data_size");
				}
			}
			//ptr += frame_size(size);
			//len -= frame_size(size);
			ptr += size;
			len -= size;
		}
	}

#ifdef ENABLE_RPM
	pm_runtime_mark_last_busy(&psh_if_info->pshc->dev);
	pm_runtime_put_autosuspend(&psh_if_info->pshc->dev);
#endif 

	if (cur_read){
		sysfs_notify(&psh_if_info->pshc->dev.kobj, NULL, "data_size");
	    
		ktime_get_ts(&t2);
	
		long elapsedTime_t12 = timespec_to_ns(&t2) - timespec_to_ns(&t1);
		long elapsedTime_t13 = timespec_to_ns(&t3) - timespec_to_ns(&t1);
		long elapsedTime_t34 = timespec_to_ns(&t4) - timespec_to_ns(&t3);
		long elapsedTime_t42 = timespec_to_ns(&t2) - timespec_to_ns(&t4);

		dev_dbg(&psh_if_info->pshc->dev, "elapsedTime_t12 = %lld ns, t13 = %lld ns, t34 = %lld ns, t42 = %lld ns\n", 
												elapsedTime_t12,
												elapsedTime_t13,
												elapsedTime_t34,
												elapsedTime_t42);

	}
	if (loop > 8) {
		queue_work(psh_if_info->wq, &psh_if_info->work);
		loop = 0;
    }

	return ret;
#else
    return 0;
#endif
}
 
#define CMD_START_STREAMING (3)
#define CMD_STOP_STREAMING  (4)


void dump_tx_buf(struct ia_cmd *cmd, int len)
{
	struct sensor_cfg_param *stream_cfg;

	printk(KERN_DEBUG "%s, tran_id=%d, cmd_id=%d, sensor_id=%d\n", __func__, cmd->tran_id, cmd->cmd_id, cmd->sensor_id);

	if(cmd->cmd_id == CMD_START_STREAMING){
		stream_cfg = (struct sensor_cfg_param *)cmd->param;
		printk(KERN_DEBUG"sample_freq=%d, buffer_delay=%d\n", stream_cfg->sample_freq, stream_cfg->buff_delay);		
	}
		
}

#define HOST2PSH_PACKET_LEN (16)
#if 0
int process_send_cmd(struct psh_ia_priv *ia_data,
			int ch, struct ia_cmd *cmd, int len)
{
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;
	int ret = 0;
	int i = 0;
	char cmd_buf[HOST2PSH_PACKET_LEN];	

	// fix host2psh package len to 16 
	len = HOST2PSH_PACKET_LEN;

	memset(cmd_buf, '\0', HOST2PSH_PACKET_LEN);
	memcpy(cmd_buf, (char *)cmd, len);
	
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = len, 
		.tx_buf = (void *)cmd_buf
	};

	for(i=0; i<len; i++)
		dev_dbg(&psh_if_info->pshc->dev," %d ", cmd_buf[i]);
	
	dev_dbg(&psh_if_info->pshc->dev,"\n");

	pm_runtime_get_sync(&psh_if_info->pshc->dev);

/*
		Host needs to reset FW for each boot up by sending CMD_RESET
		Once FW reboot, host will enable interrupt and wait data from sensorhub
*/
/*
 process_send_cmd+0x11f/0x1d0
 ia_send_cmd+0x7f/0x140
 ia_start_control+0xe5/0x1a0
 dev_attr_store+0x18/0x30
 sysfs_write_file+0xe7/0x160
 vfs_write+0xbe/0x1e0
 SyS_write+0x4d/0xa0
 ia32_do_call+0x13/0x13
*/

	if (ch == 0 && cmd->cmd_id == CMD_RESET) {
		if (psh_if_info->irq_disabled == 0) {
			disable_irq(psh_if_info->pshc->irq);
			psh_if_info->irq_disabled = 1;
			dev_info(&psh_if_info->pshc->dev, "%s disable irq %d\n", psh_if_info->pshc->irq);
		}

		/* first send soft reset to disable sensors running,
			or sensor I2C bus may hang */
		spi_message_init(&msg);
		spi_message_add_tail(&xfer, &msg);

		ret = spi_sync(psh_if_info->pshc, &msg);
		
		msleep(200);

		gpio_set_value(psh_if_info->gpio_psh_rst, 0);
		usleep_range(10000, 10000);
		gpio_set_value(psh_if_info->gpio_psh_rst, 1);

		/* wait for pshfw to run */
		msleep(1000);

		if (psh_if_info->irq_disabled == 1) {
			dev_info(&psh_if_info->pshc->dev, "%s enable irq %d\n", psh_if_info->pshc->irq);
			enable_irq(psh_if_info->pshc->irq);
			psh_if_info->irq_disabled = 0;
		}
	} else if (ch == 0 && cmd->cmd_id == CMD_FW_UPDATE) {
		if (psh_if_info->irq_disabled == 0) {
			dev_info(&psh_if_info->pshc->dev, "%s disable irq %d\n", psh_if_info->pshc->irq);
			disable_irq(psh_if_info->pshc->irq);
			psh_if_info->irq_disabled = 1;
		}

		msleep(1000);

		ret = 0;

		goto exit;
	} else if (ch == 0 && psh_if_info->irq_disabled == 1) {
		/* prevent sending command during firmware updating,
		 * or update will fail.
		 */
		ret = -EPERM;

		goto exit;
	}

		while(!gpio_get_value(psh_if_info->gpio_psh_int)){

                       msleep(1);
        }

		spi_message_init(&msg);
		spi_message_add_tail(&xfer, &msg);
		ret = spi_sync(psh_if_info->pshc, &msg);

		dump_tx_buf(cmd, len);
	
		if (ret) {
			dev_err(&psh_if_info->pshc->dev, "sendcmd through spi fail!\n");
			ret = -EIO;
		} else {
			ret = 0;
		}

#ifdef DRV_POLLING_MODE	
	if(cmd->cmd_id == CMD_START_STREAMING)
	{
		dev_err(&psh_if_info->pshc->dev, "%s start_stream\n", __func__);
		queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);
	}
	else if (cmd->cmd_id == CMD_STOP_STREAMING)
	{
		dev_err(&psh_if_info->pshc->dev, "%s stop_stream\n", __func__);
		cancel_delayed_work(&psh_if_info->dwork);
	}
#endif 	

	pm_runtime_mark_last_busy(&psh_if_info->pshc->dev);

exit:
	pm_runtime_put_autosuspend(&psh_if_info->pshc->dev);

	return ret;
}
#else

/*buffer size is just fh's palyload total size, not include fh head size*/
struct send_list_entry*  build_send_list_entry(void* lp_buffer, int buffer_size)
{
    struct send_list_entry* lp_new_entry = NULL;
    int total_size = buffer_size + SIZE_OF_FRAME_HEAD;

    if (!lp_buffer || total_size  > MAX_SEND_DATA_SIZE)
    {
        return NULL;
    }

    lp_new_entry = kzalloc(sizeof(struct send_list_entry), GFP_KERNEL);
    if (lp_new_entry)
    {
        struct frame_head* lp_fh = 
            (struct frame_head*)lp_new_entry->data;

        INIT_FRAME_HEAD(lp_fh, buffer_size);        

        memcpy(lp_fh + 1, lp_buffer, buffer_size);

        lp_new_entry->used_size = total_size;
        lp_new_entry->debug_index = 0;
    }

    return lp_new_entry;
}


void insert_send_data_entry_to_list(struct psh_ext_if* lp_psh_if_info,
                                    struct send_list_entry* lp_new_entry)
{
    mutex_lock(&lp_psh_if_info->send_data_list.lock);

    //add new to tail of the list
    list_add(&lp_new_entry->list_entry,  lp_psh_if_info->send_data_list.head.prev);

    mutex_unlock(&lp_psh_if_info->send_data_list.lock);
}

struct send_list_entry* remove_send_data_entry_from_list(struct psh_ext_if* lp_psh_if_info)
{
    struct send_list_entry* lp_removed_entry;

    mutex_lock(&lp_psh_if_info->send_data_list.lock);

    lp_removed_entry = NULL;
    if (!list_empty(&lp_psh_if_info->send_data_list.head))
    {
        lp_removed_entry = list_entry(lp_psh_if_info->send_data_list.head.next,
                                      struct send_list_entry,
                                      list_entry);
        
        list_del(lp_psh_if_info->send_data_list.head.next);
    }
    
    mutex_unlock(&lp_psh_if_info->send_data_list.lock);

    return lp_removed_entry;
}

/*The len is just parameter part of struct ia_cmd*/
int process_send_cmd(struct psh_ia_priv *ia_data,
			int ch, struct ia_cmd *cmd, int len)
{
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;
	int ret = 0;
	int i = 0;
	char cmd_buf[HOST2PSH_PACKET_LEN];	

    struct send_list_entry* lp_new_entry = NULL;

    /*yy: remove this limitation*/
	// fix host2psh package len to 16 
	//len = HOST2PSH_PACKET_LEN;
    len += (sizeof(struct ia_cmd) - CMD_PARAM_MAX_SIZE);
    //	memset(cmd_buf, '\0', HOST2PSH_PACKET_LEN);
	// memcpy(cmd_buf, (char *)cmd, len);    
    lp_new_entry = build_send_list_entry(cmd, len);
    if (!lp_new_entry)
    {
        dev_err(&psh_if_info->pshc->dev," drop send data becuause no enough memory.\n");
        return -1;
    }

	for(i=0; i<len; i++)
		dev_dbg(&psh_if_info->pshc->dev," %d ", cmd_buf[i]);
	
	dev_dbg(&psh_if_info->pshc->dev,"\n");

	pm_runtime_get_sync(&psh_if_info->pshc->dev);

/*
		Host needs to reset FW for each boot up by sending CMD_RESET
		Once FW reboot, host will enable interrupt and wait data from sensorhub
*/
/*
 process_send_cmd+0x11f/0x1d0
 ia_send_cmd+0x7f/0x140
 ia_start_control+0xe5/0x1a0
 dev_attr_store+0x18/0x30
 sysfs_write_file+0xe7/0x160
 vfs_write+0xbe/0x1e0
 SyS_write+0x4d/0xa0
 ia32_do_call+0x13/0x13
*/

    /*
      put the send data entry to send list 
      and request delay worker
     */
    insert_send_data_entry_to_list(psh_if_info, lp_new_entry);    
 
    if (!is_polling_worker_requested(psh_if_info))
    {
        request_start_polling_worker(psh_if_info);
    }

    /*         
#ifdef DRV_POLLING_MODE	
	if(cmd->cmd_id == CMD_START_STREAMING)
	{
		dev_err(&psh_if_info->pshc->dev, "%s start_stream\n", __func__);
        psh_if_info->task_flag = TASK_FLAG_REQUEST_LOOP;
		queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);
	}
	else if (cmd->cmd_id == CMD_STOP_STREAMING)
	{
		dev_err(&psh_if_info->pshc->dev, "%s stop_stream\n", __func__);
		cancel_delayed_work(&psh_if_info->dwork);
	}
#endif 	
    */

	pm_runtime_mark_last_busy(&psh_if_info->pshc->dev);
	pm_runtime_put_autosuspend(&psh_if_info->pshc->dev);

	return ret;
}


#endif
int do_setup_ddr(struct device *dev)
{
	return 0;
}

static irqreturn_t psh_byt_irq_thread(int irq, void *dev)
{
	struct spi_device *client = (struct spi_device *)dev;
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(&client->dev);

	read_psh_data(ia_data);
	return IRQ_HANDLED;
}

#ifdef ENABLE_POWER_CTRL_PIN
static void psh_byt_toggle_ctl_pin(struct device *dev,
		int value)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;
	if (psh_if_info->gpio_psh_ctl > 0) {
		gpio_set_value(psh_if_info->gpio_psh_ctl, value);
		if (value)
			usleep_range(2000, 2000);
	}
}
#endif


static int psh_byt_suspend(struct device *dev)
{
	int ret;

	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);

    struct psh_ext_if* lp_psh_if_info = 
        (struct psh_ext_if*)ia_data->platform_priv;
    
    //	struct spi_device *client =
	//	container_of(dev, struct spi_device, dev);

	ret = psh_ia_comm_suspend(dev);
	if (ret)
		return ret;

    //	disable_irq(client->irq);

#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 0);
#endif 

	//enable_irq_wake(client->irq);

    poller_pause(dev, &lp_psh_if_info->poller_worker);
    request_stop_polling_worker(lp_psh_if_info);
 
	return 0;
}

static int psh_byt_resume(struct device *dev)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);

    struct psh_ext_if* lp_psh_if_info = 
        (struct psh_ext_if*)ia_data->platform_priv;
    
    //	struct spi_device *client =
	//	container_of(dev, struct spi_device, dev);

#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 1);

    //	read_psh_data(ia_data);


#endif 
    //	enable_irq(client->irq);
	//disable_irq_wake(client->irq);

    poller_resume(dev, &lp_psh_if_info->poller_worker);
    request_start_polling_worker(lp_psh_if_info);

	return psh_ia_comm_resume(dev);
}
 
static int psh_byt_runtime_suspend(struct device *dev)
{
#ifdef ENABLE_POWER_CTRL_PIN

	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);

    struct psh_ext_if* lp_psh_if_info = 
            (struct psh_ext_if*)ia_data->platform_priv;

    psh_byt_toggle_ctl_pin(dev, 0);
    poller_pause(&lp_psh_if_info->poller_worker);

#endif 

	dev_dbg(dev, "PSH_BYT: %s\n", __func__);

	return 0;
}

static int psh_byt_runtime_resume(struct device *dev)
{
#ifdef ENABLE_POWER_CTRL_PIN

	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);

    struct psh_ext_if* lp_psh_if_info = 
        (struct psh_ext_if*)ia_data->platform_priv;
    
    psh_byt_toggle_ctl_pin(dev, 1);
    poller_resume(dev, &lp_psh_if_info->poller_worker);

#endif 

	dev_dbg(dev, "PSH_BYT: %s\n", __func__);

	return 0;
}

static const struct dev_pm_ops psh_byt_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(psh_byt_suspend,
			psh_byt_resume)
	SET_RUNTIME_PM_OPS(psh_byt_runtime_suspend,
			psh_byt_runtime_resume, NULL)
};

static void psh_work_func(struct work_struct *work)
{
	struct psh_ext_if *psh_if_info =
		container_of(work, struct psh_ext_if, work);
	
	if (psh_if_info->irq_disabled == 0) {
	    disable_irq(psh_if_info->pshc->irq);
		psh_if_info->irq_disabled = 1;
		dev_err(&psh_if_info->pshc->dev, "disable psh irq\n");
	}
	
}

#ifdef DRV_POLLING_MODE 	
#if 0
static void poll_sensor_data(struct work_struct *work)
{
	struct psh_ext_if *psh_if_info = 
		container_of(work, struct psh_ext_if, dwork.work);
	
	struct spi_device *client = (struct spi_device *)psh_if_info->pshc;

	struct psh_ia_priv *ia_data =
		(struct psh_ia_priv *)dev_get_drvdata(&client->dev);

    dev_info(&psh_if_info->pshc->dev,"I'm sensor hub D\n", __func__);

	if(ia_data){	
	    read_psh_data(ia_data);
        queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);  // polling@100Hz
	}
	else
		dev_err(&psh_if_info->pshc->dev,"%s ia_data is NULL\n", __func__);

}
#else

static void process_received_data(struct psh_ext_if *lp_psh_if_info,
                                  u8* lp_buffer, int buffer_size)
{
    int ret_value;
    int cur_read = 0;
    struct psh_ia_priv* lp_ia_data = lp_psh_if_info->ia_data;

    //skip the frame head
    lp_buffer += SIZE_OF_FRAME_HEAD;
    buffer_size = MAX(buffer_size - SIZE_OF_FRAME_HEAD, 0);

    while (buffer_size > 0) 
    {
        struct cmd_resp *resp = (struct cmd_resp *)lp_buffer;
        u32 size = sizeof(*resp) + resp->data_len;
        
        //printk("yy 7\n");
        ret_value = ia_handle_frame(lp_ia_data, lp_buffer, size);
        // printk("yy 8\n");
        if (ret_value > 0) 
        {
            cur_read += ret_value;
            
            if (cur_read > 250) 
            {
                cur_read = 0;
                //printk("yy 9\n");
                sysfs_notify(&lp_psh_if_info->pshc->dev.kobj,
                             NULL, "data_size");
                //printk("yy 10\n");
                dev_err(&lp_psh_if_info->pshc->dev, "request daemon to fetch data\n");
            }
        }
        //ptr += frame_size(size);
        //len -= frame_size(size);
        lp_buffer += size;
        buffer_size -= size;
    }

    if (cur_read)
    {
        //printk("yy 11\n");
		sysfs_notify(&lp_psh_if_info->pshc->dev.kobj, NULL, "data_size");
        //printk("yy 12\n");
    }    
}


static void poll_sensor_data_normal(struct psh_ext_if *psh_if_info)
{
    u8* lp_send;
    int send_size;
    int receved_size;
    int ret_value;

	struct psh_ia_priv *ia_data = psh_if_info->ia_data;
    struct send_list_entry* lp_send_entry = NULL;

	if(!ia_data)
    {
        dev_err(&psh_if_info->pshc->dev,"%s ia_data is NULL, ignore this time\n", __func__);
        return;
    }

    lp_send_entry = remove_send_data_entry_from_list(psh_if_info);    
    if (lp_send_entry)
    {
        /*send data logic*/
        dev_info(&psh_if_info->pshc->dev,"will send real data, index = %d, size = %d\n",
                lp_send_entry->debug_index,
                lp_send_entry->used_size);
        
        lp_send = lp_send_entry->data;
        send_size = lp_send_entry->used_size;
    }
    else
    {
        lp_send = psh_if_info->zero_send_buffer;
        send_size = sizeof(psh_if_info->zero_send_buffer);
        dev_info(&psh_if_info->pshc->dev,"Will end dummy data\n");
    }
 
    dev_info(&psh_if_info->pshc->dev,"will send data size = %d\n",
            send_size);
        
    /*let's run SPI!! */
    INIT_SPI_IO_CONTEXT(&psh_if_info->send_data_list.low_spi_io_context);

    dev_info(&psh_if_info->pshc->dev,"+++++++++++do_io_transaction++++++++++\n");

    ret_value = do_io_transaction(psh_if_info->pshc, 
                                  &psh_if_info->send_data_list.low_spi_io_context,
                                  lp_send, send_size,
                                  psh_if_info->psh_frame, sizeof(psh_if_info->psh_frame),
                                  &receved_size);
    dev_info(&psh_if_info->pshc->dev,"-----------do_io_transaction----------\n");

    /*
      by yy:
      below part should move to high level component...just workaround here
     */
#if 0
	if (ch == 0 && cmd->cmd_id == CMD_RESET) 
    {
        /*wait to let psh reset*/
        dev_err(&psh_if_info->pshc->dev,"Send CMD_RESET\n");
		msleep(1000);
	} 
    else if (ch == 0 && cmd->cmd_id == CMD_FW_UPDATE) 
    {
        /*wait to let psh reset*/
        dev_err(&psh_if_info->pshc->dev,"Send CMD_FW_UPDATE\n");
		msleep(1000);
	} 
#endif

    if (IS_SUCCESS(ret_value))
    {
        /*received something, let's check it*/
        if (0 != receved_size)
        {
             process_received_data(psh_if_info,
                                  psh_if_info->psh_frame, receved_size);
        }
    }

    /*finished, so clear resource*/
    if (lp_send_entry)
    {
        kfree(lp_send_entry);
    }
}



static void poll_sensor_data(struct work_struct *work)
{
    struct psh_ext_if *psh_if_info = 
		container_of(work, struct psh_ext_if, dwork.work);
	
	struct psh_ia_priv *ia_data = psh_if_info->ia_data;

#ifdef ENABLE_RPM
	/* We may need to zero all the buffer */
	pm_runtime_get_sync(&psh_if_info->pshc->dev);
#endif 

    mutex_lock(&psh_if_info->workitem_mutex);
        
    if (ia_data->is_in_debug)
    {
        poll_sensor_data_debug(psh_if_info);
    }
    else
    {
        poll_sensor_data_normal(psh_if_info);
    }

    mutex_unlock(&psh_if_info->workitem_mutex);

    /*continue polling if necessary*/
    if (TASK_FLAG_REQUEST_LOOP & psh_if_info->task_flag)
    {
        queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);
    }
    else
    {
        dev_err(&psh_if_info->pshc->dev, "note: IO delay workered exited.\n");
    }

#ifdef ENABLE_RPM
	pm_runtime_mark_last_busy(&psh_if_info->pshc->dev);
	pm_runtime_put_autosuspend(&psh_if_info->pshc->dev);
#endif 

}
#endif

#endif 	


static void poll_sensor_data_by_thread(void* lp_param)
{
    struct psh_ext_if *psh_if_info = (struct psh_ext_if *)lp_param;
	struct psh_ia_priv *ia_data = psh_if_info->ia_data;

    timestamp_record_begin(&psh_if_info->io_profiler);

    if (ia_data->is_in_debug)
    {
        poll_sensor_data_debug(psh_if_info);
    }
    else
    {
        poll_sensor_data_normal(psh_if_info);
    }

    timestamp_record_end(&psh_if_info->io_profiler);
}


/* FIXME: it will be a platform device */
static int psh_probe(struct spi_device *client)
{
	int ret = -EPERM;
	struct psh_ia_priv *ia_data;
	struct psh_ext_if *psh_if_info;
	int rc;
	
	dev_err(&client->dev, "%s\n", __func__);

	psh_if_info = kzalloc(sizeof(*psh_if_info), GFP_KERNEL);
	if (!psh_if_info) {
		dev_err(&client->dev, "can not allocate psh_if_info\n");
		goto psh_if_err;
	}

	ret = psh_ia_common_init(&client->dev, &psh_if_info->ia_data);
	if (ret) {
		dev_err(&client->dev, "fail to init psh_ia_common\n");
		goto psh_ia_err;
	}

    ia_data = psh_if_info->ia_data;

    /*
      initialize send list
     */
    mutex_init(&psh_if_info->send_data_list.lock);
    mutex_init(&psh_if_info->workitem_mutex);
    INIT_LIST_HEAD(&psh_if_info->send_data_list.head);
    
    psh_if_info->task_flag = TASK_FLAG_CLEAR;
    dev_err(&client->dev, "send list inited\n");

	psh_if_info->hwmon_dev = hwmon_device_register(&client->dev);
	if (!psh_if_info->hwmon_dev) {
		dev_err(&client->dev, "fail to register hwmon device\n");
		goto hwmon_err;
	}

	psh_if_info->pshc = client;

	ia_data->platform_priv = psh_if_info;

#if 0
	psh_if_info->gpio_psh_ctl =
				acpi_get_gpio_by_index(&client->dev, 1, NULL);
#endif 

	psh_if_info->gpio_psh_ctl = -1;

	if (psh_if_info->gpio_psh_ctl < 0) {
		dev_warn(&client->dev, "fail to get psh_ctl pin by ACPI\n");
	} else {
		rc = gpio_request(psh_if_info->gpio_psh_ctl, "psh_ctl");
		if (rc) {
			dev_warn(&client->dev, "fail to request psh_ctl pin\n");
			psh_if_info->gpio_psh_ctl = -1;
		} else {
			gpio_export(psh_if_info->gpio_psh_ctl, 1);
			gpio_direction_output(psh_if_info->gpio_psh_ctl, 1);
			gpio_set_value(psh_if_info->gpio_psh_ctl, 1);
		}
	}
#if 0
	psh_if_info->gpio_psh_rst =
				acpi_get_gpio_by_index(&client->dev, 0, NULL);
#endif 

	psh_if_info->gpio_psh_rst = GPIO_PSH_MCU_RESET;

	if (psh_if_info->gpio_psh_rst < 0) {
		dev_warn(&client->dev, "failed to get psh_rst pin by ACPI\n");
	} else {
		rc = gpio_request(psh_if_info->gpio_psh_rst, "psh_rst");
		if (rc) {
			dev_warn(&client->dev, "fail to request psh_rst pin\n");
			psh_if_info->gpio_psh_rst = -1;
		} else {
			gpio_export(psh_if_info->gpio_psh_rst, 1);
			gpio_direction_output(psh_if_info->gpio_psh_rst, 1);
			gpio_set_value(psh_if_info->gpio_psh_rst, 1);
		}
	}
	// TODO update gpio_psh_int from platform_data 
	//psh_if_info->gpio_psh_int = (int)id->driver_data;
	
	client->irq = gpio_to_irq(GPIO_PSH_INT);  // force polling mode
#ifdef DRV_POLLING_MODE
	client->irq = -1;
#endif
 
	if(client->irq > 0){

	    psh_if_info->gpio_psh_int = GPIO_PSH_INT; 
	
	    rc = gpio_request(psh_if_info->gpio_psh_int, "psh_int");
	    if (rc) {
		    dev_warn(&client->dev, "fail to request psh_int pin\n");
		    psh_if_info->gpio_psh_int = -1;
	    } else {
		    gpio_export(psh_if_info->gpio_psh_int, 1);
	    }

	    /* set the flag to to enable irq when need */
	    irq_set_status_flags(client->irq, IRQ_NOAUTOEN);

	    ret = request_threaded_irq(client->irq, NULL, psh_byt_irq_thread,
			IRQF_TRIGGER_FALLING | IRQF_ONESHOT, "psh_byt", client);
	    if (ret) {
		    dev_err(&client->dev, "fail to request irq\n");
		    goto irq_err;
	    }

	    psh_if_info->irq_disabled = 1;
    }

#ifdef DRV_POLLING_MODE 
	psh_if_info->gpio_psh_int = GPIO_PSH_INT;
	rc = gpio_request(psh_if_info->gpio_psh_int, "psh_int");
	if (rc) {
		    dev_warn(&client->dev, "fail to request psh_int pin\n");
		    psh_if_info->gpio_psh_int = -1;
	}
	else {
		    gpio_export(psh_if_info->gpio_psh_int, 1);
			gpio_direction_input(psh_if_info->gpio_psh_int);
	}
#endif 	

    //    psh_if_info->wq = create_singlethread_workqueue("psh_work");
    /*    psh_if_info->wq = alloc_ordered_workqueue("%S", WQ_MEM_RECLAIM|WQ_HIGHPRI, "psh_work");*/


    psh_if_info->wq = alloc_workqueue("%s", WQ_HIGHPRI, 0, "psh_work");

#if 1

    //just a profiler here
    timestamp_init_with_name(&client->dev, 
                             &psh_if_info->io_profiler, "profile_pull");

    poller_init(&psh_if_info->poller_worker,
                poll_sensor_data_by_thread, psh_if_info);

    if (!IS_SUCCESS(poller_start(&client->dev,
                                 &psh_if_info->poller_worker)))
    {
        dev_err(&client->dev, "fail to create poller\n");
        goto wq_err;
    }

    /*
      try sync timestamp with sensorhub fw once 
     */
    ia_sync_timestamp_with_sensorhub_fw(psh_if_info->ia_data);

#endif

	if (!psh_if_info->wq) {
	    dev_err(&client->dev, "fail to create workqueue\n");
		goto wq_err;
	}
    
	INIT_WORK(&psh_if_info->work, psh_work_func);
    
    
    //dev_err(&client->dev, "sensor polling speed: %dHZ\n", POLL_HZ_PER_SECOND);
    
#ifdef DRV_POLLING_MODE 	
	INIT_DELAYED_WORK(&psh_if_info->dwork, poll_sensor_data);
#endif 

#ifdef ENABLE_RPM	
	pm_runtime_set_active(&client->dev);
	pm_runtime_use_autosuspend(&client->dev);
	pm_runtime_set_autosuspend_delay(&client->dev, 0);
	pm_runtime_enable(&client->dev);
#endif 

#ifndef DRV_POLLING_MODE 
	psh_if_info->irq_disabled = 0;
    enable_irq(psh_if_info->pshc->irq);
#endif 

	return 0;

wq_err:
	free_irq(client->irq, psh_if_info->pshc);
irq_err:
	hwmon_device_unregister(psh_if_info->hwmon_dev);
hwmon_err:
	psh_ia_common_deinit(&client->dev);
psh_ia_err:
	kfree(psh_if_info);
psh_if_err:
	return ret;
}

static int psh_remove(struct spi_device *client)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(&client->dev);
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;

	pm_runtime_get_sync(&client->dev);
	pm_runtime_disable(&client->dev);

	if (psh_if_info->wq)
		destroy_workqueue(psh_if_info->wq);

    poller_stop(&client->dev, &psh_if_info->poller_worker);

	free_irq(client->irq, psh_if_info->pshc);
	gpio_unexport(psh_if_info->gpio_psh_rst);
	gpio_unexport(psh_if_info->gpio_psh_ctl);
	gpio_free(psh_if_info->gpio_psh_rst);
	gpio_free(psh_if_info->gpio_psh_ctl);
	hwmon_device_unregister(psh_if_info->hwmon_dev);
	psh_ia_common_deinit(&client->dev);
    
    timestamp_destory(&client->dev, &psh_if_info->io_profiler);

	return 0;
}


static int verify_frame_head_and_get_payload_size(u8* lp_buffer, int buffer_size, 
                                                  _OUT_ int* lp_total_payload_size,
                                                  _OUT_ int* lp_contained_payload_size,
                                                  _OUT_ int* lp_frame_head_start_index)
{
    union {
        struct frame_head* lp_fh;
        u8* lp_u8;
    }lp_frame_head;    

    int loop_i;
    int loop_end = buffer_size - SIZE_OF_FRAME_HEAD;

    int payload_size = 0;
    int contained_payload_size = 0;

    if (buffer_size < SIZE_OF_FRAME_HEAD)
    {
        return ER_FAILED;
    }

    lp_frame_head.lp_u8 = lp_buffer;

    for (loop_i = 0; loop_i <= loop_end; ++loop_i)
    {
        if (IS_VALID_FRAME_HEAD(lp_frame_head.lp_fh))
        {
            payload_size = lp_frame_head.lp_fh->length;
            contained_payload_size = MIN(buffer_size - loop_i - SIZE_OF_FRAME_HEAD, payload_size);

            lp_total_payload_size ? *lp_total_payload_size = payload_size : 0;
            lp_contained_payload_size ? *lp_contained_payload_size = contained_payload_size : 0;
            lp_frame_head_start_index ? *lp_frame_head_start_index = loop_i : 0;

            return ER_SUCCESS;
        }
        else
        {
            ++lp_frame_head.lp_u8;
        }
    }

    return ER_FAILED;
}


static int verify_frame_head_prefix(u8* const lp_buffer, int buffer_size)
{
    int loop_i;

    for (loop_i = 0; loop_i < buffer_size; ++loop_i)
    {
        if (FRAME_HEAD_SIGN_PREFIX == lp_buffer[loop_i])
        {
            return ER_SUCCESS;
        }
    }

    return ER_FAILED;
}


static int do_spi_io(struct spi_device* lp_dev, u8* lp_send_buffer, u8* lp_recv_buffer, 
                     int buffer_size)
{
    int ret_value;
	struct spi_message msg;
    struct spi_transfer xfer = 
        {
            .len = buffer_size, 
            .tx_buf = (void*)lp_send_buffer,
            .rx_buf = (void*)lp_recv_buffer,
            .speed_hz = 1000000,
        };

    spi_message_init(&msg);
    spi_message_add_tail(&xfer, &msg);

    dev_info(&lp_dev->dev, "spi io: transfer size = %d\n", buffer_size);
    
    ret_value = spi_sync(lp_dev, &msg);

    if (IS_SUCCESS(ret_value))
    {
        dev_info(&lp_dev->dev, "spi io done.\n");
    }
    
    dev_info(&lp_dev->dev, "do_spi_io ret_value = %d\n", ret_value);

    return ret_value;
}


int do_io_transaction(struct spi_device* lp_dev, 
                      _IN_ struct spi_io_context* lp_io_context,
                      _IN_ u8* const lp_send_buffer, int send_buffer_size,
                      _OUT_ u8* lp_recv_buffer, int recv_buffer_size, 
                      _OUT_ int* lp_recved_size )
{
    #define DUMMY_BUFFER_SIZE  16

    int ret_value = ER_FAILED;

    int total_trafster = 0;
    int one_time_transfer = 0;

    int remain_send_count = send_buffer_size;
    int remain_recv_count = 0;

    int is_recved_vaild_fh = 0;

    struct buffer* lp_send_operator = NULL;
    struct buffer* lp_recv_operator = NULL;

    struct buffer dummy_send_buffer;
    struct buffer dummy_recv_buffer;

    struct buffer send_buffer;
    struct buffer recv_buffer;

    INIT_BUFFER(&dummy_send_buffer,
                       lp_io_context->send_dummy_buffer,
                       lp_io_context->send_dummy_buffer_size);
    
    INIT_BUFFER(&dummy_recv_buffer,
                       lp_io_context->recv_dummy_buffer,
                       lp_io_context->recv_dummy_buffer_size);

    INIT_BUFFER(&send_buffer, lp_send_buffer, send_buffer_size);

    INIT_BUFFER(&recv_buffer, lp_recv_buffer, recv_buffer_size);

    /*need some check here, but still in think.*/

    total_trafster = send_buffer_size;
    while(total_trafster > 0)
    {
        int send_buffer_is_dummy;
        int recv_buffer_is_dummy;

        /*
          Step1. try calc out transfer bye count
        */
        if (0 != BUFFER_REMAIN_LENGTH(send_buffer))
        {
            lp_send_operator = &send_buffer;
            send_buffer_is_dummy = FALSE;
        }
        else
        {
            lp_send_operator = &dummy_send_buffer;
            send_buffer_is_dummy = TRUE;
        }

        if (0 != remain_recv_count 
            && is_recved_vaild_fh)
        {
            lp_recv_operator = &recv_buffer;
            recv_buffer_is_dummy = FALSE;
        }
        else
        {
            lp_recv_operator = &dummy_recv_buffer;
            recv_buffer_is_dummy = TRUE;
        }

        if (is_recved_vaild_fh)
        {
            RESET_BUFFER(&dummy_send_buffer);
            RESET_BUFFER(&dummy_recv_buffer);

            if (send_buffer_is_dummy && recv_buffer_is_dummy)
            {
                one_time_transfer = 0;
            }
            else
            {
                one_time_transfer = MIN(BUFFER_REMAIN_LENGTH(*lp_send_operator),
                                        BUFFER_REMAIN_LENGTH(*lp_recv_operator));
            }
        }
        else
        {
            /*
              can't reset dummy recv buffer because it contain last time received
              splited data
             */
            one_time_transfer = MIN(BUFFER_REMAIN_LENGTH(*lp_send_operator), 
                                    BUFFER_REMAIN_LENGTH(dummy_recv_buffer));                       
        }

        if (0 == one_time_transfer)
        {
            /*caller's receive buffer is not enough case.*/
            if ( 0 != remain_recv_count)
            {
                ret_value = ER_NO_ENOUGH_RECV_BUFFER;
            }

            break;
        }

        /*
          Step 2. Prepare and do transfer
         */
        dev_info(&lp_dev->dev, "before do_spi_io\n");
        ret_value = do_spi_io(lp_dev,
                              BUFFER_PTR(*lp_send_operator), 
                              BUFFER_PTR(*lp_recv_operator),
                              one_time_transfer);
        if (IS_FAILED(ret_value))
        {
            dev_err(&lp_dev->dev, "do_spi_io() failed! \n");
            break;
        }
        dev_info(&lp_dev->dev, "after do_spi_io\n");

        lp_send_operator->index += one_time_transfer;
        lp_recv_operator->index += one_time_transfer;

        remain_send_count = MAX(0, remain_send_count - one_time_transfer);
        remain_recv_count = MAX(0, remain_recv_count - one_time_transfer);
        total_trafster -= one_time_transfer;

        /*
          Step 3. check if we received valid frame header
         */
        if (!is_recved_vaild_fh)
        {
            int total_payload_size;
            int contained_payload_size;
            int fh_start_index;
            int is_valid_fh;
            
            is_valid_fh = verify_frame_head_and_get_payload_size(lp_recv_operator->lp_ptr, 
                                                                 BUFFER_USED_LENGTH(*lp_recv_operator),
                                                                 &total_payload_size,
                                                                 &contained_payload_size,
                                                                 &fh_start_index);
            if (IS_SUCCESS(is_valid_fh))
            {
                int copy_size = contained_payload_size + SIZE_OF_FRAME_HEAD;
                int need_recv_buffer_size = total_payload_size + SIZE_OF_FRAME_HEAD;

                /*received new frame head!*/
                remain_recv_count = total_payload_size - contained_payload_size;
                
                /*received frame head, so we update total transfer count here*/
                total_trafster = MAX(remain_recv_count, remain_send_count);

                /*
                printf("[packege check]: total payload = %d, contained = %d, fh_start = %d\n",
                       total_payload_size,
                       contained_payload_size,
                       fh_start_index);
                */

                /*copy all valid data to actual receive buffer head*/

                if (need_recv_buffer_size > recv_buffer_size)
                {
                    ret_value = ER_NO_ENOUGH_RECV_BUFFER;
                    break;
                }
                
                RESET_BUFFER(&recv_buffer);
                
                memcpy(BUFFER_PTR(recv_buffer), 
                       lp_recv_operator->lp_ptr + fh_start_index,
                       copy_size);
                
                recv_buffer.index += copy_size;
                recv_buffer.length = need_recv_buffer_size;
            
                is_recved_vaild_fh = TRUE;
            }
            else
            {
                int is_recved_hf_prefix = ER_FAILED;

                remain_recv_count = 0;

                //copy SIZEOF_FRAME_HEAD bytes from tail to head
                memcpy(dummy_recv_buffer.lp_ptr,
                       BUFFER_PTR_FROM_USED_TAIL(*lp_recv_operator, SIZE_OF_FRAME_HEAD),
                       SIZE_OF_FRAME_HEAD);

                dummy_recv_buffer.index = SIZE_OF_FRAME_HEAD;

                /*check if the last SIZE_OF_FRAME_HEAD bytes contained frame head prefix,
                  we will read more data if it contained, to resovle slice case
                 */
                is_recved_hf_prefix = verify_frame_head_prefix(BUFFER_PTR_FROM_USED_TAIL(*lp_recv_operator, SIZE_OF_FRAME_HEAD),
                                                               SIZE_OF_FRAME_HEAD);
                /*
                  check if the received data included frame head prefix 0x53
                 */
                if (IS_SUCCESS(is_recved_hf_prefix))
                {
                    total_trafster += BUFFER_REMAIN_LENGTH(dummy_recv_buffer);
                    /*
                    printf("set total_transfer = %d\n", total_trafster);
                    */
                }
                
                is_recved_vaild_fh = FALSE;
            }
        }
    }

#if 1   
    if (IS_FAILED(ret_value))
    {
        /*
          dump recvied buffer
         */
        
        //        dump_buffer(lp_recv_operator->lp_ptr, BUFFER_USED_LENGTH(*lp_recv_operator));
    }
    else
    {
        dump_buffer(recv_buffer.lp_ptr, BUFFER_USED_LENGTH(recv_buffer));
    }
#endif


    lp_recved_size ? *lp_recved_size = BUFFER_USED_LENGTH(recv_buffer) : 0;

    return ret_value;

}


static void psh_shutdown(struct spi_device *client)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(&client->dev);
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;

    poller_stop(&client->dev, &psh_if_info->poller_worker);

	free_irq(client->irq, psh_if_info->pshc);

	if (psh_if_info->gpio_psh_rst)
		gpio_set_value(psh_if_info->gpio_psh_rst, 0);



	dev_dbg(&psh_if_info->pshc->dev, "PSH_BYT: %s\n", __func__);
}

static struct spi_driver psh_vr_driver = {
	.driver = {
		.name	= "vr-sensor-spi-ipc",
		.owner	= THIS_MODULE,
		.pm = &psh_byt_pm_ops, 
	},
	.probe		= psh_probe,
	.remove     = psh_remove,
	.shutdown	= psh_shutdown,
};

module_spi_driver(psh_vr_driver);

MODULE_LICENSE("GPL v2");
