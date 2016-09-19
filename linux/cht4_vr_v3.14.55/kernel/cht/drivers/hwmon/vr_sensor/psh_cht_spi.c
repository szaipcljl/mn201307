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
//#include <linux/acpi_gpio.h>
#include <linux/pm_runtime.h>
//#include <asm/intel_vlv2.h>
#include <linux/workqueue.h>

#include "psh_ia_common.h"
#define GPIO_PSH_INT ((int)275)
#define GPIO_PSH_MCU_RESET (341)

//#define DRV_POLLING_MODE
#define POLLING_HZ (HZ)

/* need a global lock to check the psh driver access */
struct psh_ext_if {
	struct device *hwmon_dev;
	struct spi_device *pshc;
	char psh_frame[LBUF_MAX_CELL_SIZE];

	int gpio_psh_ctl, gpio_psh_rst;
	int gpio_psh_int;

	int irq_disabled;
	
	struct workqueue_struct *wq;
	
	struct work_struct work;
#ifdef DRV_POLLING_MODE	
	struct delayed_work dwork;
#endif 
	
};

void dump_cmd_resp(char *ptr, int len)
{
	struct cmd_resp *resp = (struct cmd_resp *)ptr;
	u32 size = sizeof(struct cmd_resp) + resp->data_len;
	int i;

	printk(KERN_DEBUG"%s, tran_id=%d, type=%d, sensor_id=%d, data_len=%d\n", __func__, resp->tran_id, resp->type, resp->sensor_id, size);

	for(i=0 ; i<len ; i++)
	{
		printk(" %d", ptr[i]);
	}
	printk(KERN_DEBUG"\n");	
}

int read_psh_data(struct psh_ia_priv *ia_data)
{
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
	
	int sequent_dummy = 0;
	static int loop = 0;

#ifdef ENABLE_RPM
	/* We may need to zero all the buffer */
	pm_runtime_get_sync(&psh_if_info->pshc->dev);
#endif 
	psh_if_info->gpio_psh_int = GPIO_PSH_INT;

	int gpio_val = gpio_get_value(psh_if_info->gpio_psh_int);

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

		dev_err(&psh_if_info->pshc->dev, "elapsedTime_t12 = %lld ns, t13 = %lld ns, t34 = %lld ns, t42 = %lld ns\n", 
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
		dev_info(&psh_if_info->pshc->dev, "%s start_stream\n", __func__);
		queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);
	}
	else if (cmd->cmd_id == CMD_STOP_STREAMING)
	{
		dev_info(&psh_if_info->pshc->dev, "%s stop_stream\n", __func__);
		cancel_delayed_work(&psh_if_info->dwork);
	}
#endif 	

	pm_runtime_mark_last_busy(&psh_if_info->pshc->dev);

exit:
	pm_runtime_put_autosuspend(&psh_if_info->pshc->dev);

	return ret;
}

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

static int psh_byt_suspend(struct device *dev)
{
	int ret;
	struct spi_device *client =
		container_of(dev, struct spi_device, dev);

	ret = psh_ia_comm_suspend(dev);
	if (ret)
		return ret;
	disable_irq(client->irq);
#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 0);
#endif 
	enable_irq_wake(client->irq);
	return 0;
}

static int psh_byt_resume(struct device *dev)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(dev);
	struct spi_device *client =
		container_of(dev, struct spi_device, dev);

#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 1);
	read_psh_data(ia_data);
#endif 
	enable_irq(client->irq);
	disable_irq_wake(client->irq);
	return psh_ia_comm_resume(dev);
}

static int psh_byt_runtime_suspend(struct device *dev)
{
	dev_dbg(dev, "PSH_BYT: %s\n", __func__);
#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 0);
#endif 
	return 0;
}

static int psh_byt_runtime_resume(struct device *dev)
{
	dev_dbg(dev, "PSH_BYT: %s\n", __func__);
#ifdef ENABLE_POWER_CTRL_PIN
	psh_byt_toggle_ctl_pin(dev, 1);
#endif 
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
static void poll_sensor_data(struct work_struct *work)
{
	struct psh_ext_if *psh_if_info = 
		container_of(work, struct psh_ext_if, dwork.work);
	
	struct spi_device *client = (struct spi_device *)psh_if_info->pshc;

	struct psh_ia_priv *ia_data =
		(struct psh_ia_priv *)dev_get_drvdata(&client->dev);

	if(ia_data){	
	    read_psh_data(ia_data);
	    queue_delayed_work(psh_if_info->wq, &psh_if_info->dwork, POLLING_HZ);  // polling@100Hz
	}
	else
		dev_err(&psh_if_info->pshc->dev,"%s ia_data is NULL\n", __func__);

}
#endif 	

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

	ret = psh_ia_common_init(&client->dev, &ia_data);
	if (ret) {
		dev_err(&client->dev, "fail to init psh_ia_common\n");
		goto psh_ia_err;
	}

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

    psh_if_info->wq = create_singlethread_workqueue("psh_work");
	if (!psh_if_info->wq) {
	    dev_err(&client->dev, "fail to create workqueue\n");
		goto wq_err;
	}

	INIT_WORK(&psh_if_info->work, psh_work_func);

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
	free_irq(client->irq, psh_if_info->pshc);
	gpio_unexport(psh_if_info->gpio_psh_rst);
	gpio_unexport(psh_if_info->gpio_psh_ctl);
	gpio_free(psh_if_info->gpio_psh_rst);
	gpio_free(psh_if_info->gpio_psh_ctl);
	hwmon_device_unregister(psh_if_info->hwmon_dev);
	psh_ia_common_deinit(&client->dev);

	return 0;
}

static void psh_shutdown(struct spi_device *client)
{
	struct psh_ia_priv *ia_data =
			(struct psh_ia_priv *)dev_get_drvdata(&client->dev);
	struct psh_ext_if *psh_if_info =
			(struct psh_ext_if *)ia_data->platform_priv;

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
