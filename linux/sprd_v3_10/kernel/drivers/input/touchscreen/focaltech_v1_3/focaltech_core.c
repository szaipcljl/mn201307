/*
 * drivers/input/touchscreen/focaltech.c
 *
 * FocalTech focaltech TouchScreen driver.
 *
 * Copyright (c) 2010-2015, Focaltech Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * VERSION      	DATE			AUTHOR
 *    1.0		       2014-09			mshl
 *
 */

 /*******************************************************************************
*
* File Name: focaltech.c
*
* Author: mshl
*
* Created: 2014-09
*
* Modify by mshl on 2015-10-26
*
* Abstract:
*
* Reference:
*
*******************************************************************************/

/******************************************************************************
* Included header files
*******************************************************************************/
#include <linux/firmware.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
//#include <linux/i2c/ft53x6_ts.h>
#include <soc/sprd/regulator.h>
#include <linux/input/mt.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/err.h>
#include "focaltech_core.h"

#if(defined(CONFIG_I2C_SPRD) ||defined(CONFIG_I2C_SPRD_V1))
#include <soc/sprd/i2c-sprd.h>
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

/*******************************************************************************
* Private constant and macro definitions using #define
*******************************************************************************/
//#define FTS_DBG
#ifdef FTS_DBG
#define ENTER printk(KERN_INFO "[FTS_DBG] func: %s  line: %04d\n", __func__, __LINE__);
#define PRINT_DBG(x...)  printk(KERN_INFO "[FTS_DBG] " x)
#define PRINT_INFO(x...)  printk(KERN_INFO "[FTS_INFO] " x)
#define PRINT_WARN(x...)  printk(KERN_INFO "[FTS_WARN] " x)
#define PRINT_ERR(format,x...)  printk(KERN_ERR "[FTS_ERR] func: %s  line: %04d  info: " format, __func__, __LINE__, ## x)
#else
#define ENTER
#define PRINT_DBG(x...)
#define PRINT_INFO(x...)  printk(KERN_INFO "[FTS_INFO] " x)
#define PRINT_WARN(x...)  printk(KERN_INFO "[FTS_WARN] " x)
#define PRINT_ERR(format,x...)  printk(KERN_ERR "[FTS_ERR] func: %s  line: %04d  info: " format, __func__, __LINE__, ## x)
#endif

#define	USE_WAIT_QUEUE	1
#define	USE_THREADED_IRQ	0
#define	USE_WORK_QUEUE	0

#define	TOUCH_VIRTUAL_KEYS
#define	MULTI_PROTOCOL_TYPE_B	1
#define	TS_MAX_FINGER		5

#define	FTS_PACKET_LENGTH		128

/*******************************************************************************
* Private enumerations, structures and unions using typedef
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/
#if USE_WAIT_QUEUE
static struct task_struct *thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(waiter);
static int tpd_flag = 0;
#endif

static int fw_size;

static int bsuspend = 0;

struct fts_ts_data *fts_wq_data;
struct i2c_client *fts_i2c_client;
//static unsigned char suspend_flag = 0;
u8 g_chip_id=0,support_chip_num=17;//huafeizhou150325 add
struct fts_chip_Info {
u8 CHIP_ID;
u8 FTS_NAME[20];
};

struct fts_chip_Info fts_chipInfo[] =
{
    {0x55,"FT5x06"}, //,"FT5x06"
    {0x08,"FT5606"}, //,"FT5606"
	{0x0a,"FT5x16"}, //,"FT5x16"
	{0x06,"FT6x06"}, //,"FT6x06"
	{0x36,"FT6x36"}, //,"FT6x36"
	{0x64,"FT6336GU"}, //,"FT6336GU"
	{0x55,"FT5x06i"}, //,"FT5x06i"
	{0x14,"FT5336"}, //,"FT5336"
	{0x13,"FT3316"}, //,"FT3316"
	{0x12,"FT5436i"}, //,"FT5436i"
	{0x11,"FT5336i"}, //,"FT5336i"
	{0x54,"FT5x46"}, //,"FT5x46"
    {0x58,"FT5822"},//"FT5822"
	{0x59,"FT5x26"},//"FT5x26"
	{0x86,"FT8606"},//"FT8606"
	{0x87,"FT8716"},//"FT8716"
	{0x0E,"FT3x07"}, //,"FT3x07"
};

				
struct input_dev *fts_input_dev;

static unsigned char suspend_flag = 0;

#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //huafeizhou141218 add
extern int irq_set_irq_type(unsigned int irq, unsigned int type);
static struct class  *tp_gesture_class;
static struct device *tp_gesture_dev;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);

// Fixme;
static DEVICE_ATTR(enable, 4775, gesture_enable_show, gesture_enable_store);
//static DEVICE_ATTR(mode, 4775, gesture_mode_show, gesture_mode_store);




u16 ft_gesture_enable       = TP_GESTURE_OFF;
u8 ft_gesture_id       = 0x00;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (sprintf(buf, "ft_gesture_enable = 0x%x! ft_gesture_id=0x%x\n", ft_gesture_enable,ft_gesture_id));
}

static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret       = 0;
	size_t on_off = simple_strtoul(buf, NULL, 10);

#if 1
	ft_gesture_enable=on_off;
#else
    switch (on_off)
    {
        case 0x00:
        {
            ft_gesture_enable = TP_GESTURE_DISABLE;
            break;
        }

        case 0x01:
        {
            ft_gesture_enable = ft_gesture_enable;
            break;
        }

        default:
            return -EINVAL;
    }
#endif
	return size;
}

#endif /*CONFIG_GST_TP_GESTURE_SUPPORT*/
static unsigned char fts_read_fw_ver(void);

#ifdef CONFIG_HAS_EARLYSUSPEND
static void fts_ts_suspend(struct early_suspend *handler);
static void fts_ts_resume(struct early_suspend *handler);
#endif
#ifdef CONFIG_GST_TP_PROXIMITY_SUPPORT
#include <linux/spinlock.h>

#define TP_PROXIMITY_SENSOR
#endif
#ifdef TP_PROXIMITY_SENSOR
static int PROXIMITY_SWITCH;
static int PROXIMITY_STATE;

static struct spinlock proximity_switch_lock;
static struct spinlock proximity_state_lock;

#define FTS_PROX_OPEN    0x01
#define FTS_PROX_CLOSE   0x00
#define FTS_PROX_NEAR    0xC0
#define FTS_PROX_FAR     0xE0
#endif
/*******************************************************************************
* Global variable or extern global variabls/functions
*******************************************************************************/

#ifdef TP_PROXIMITY_SENSOR
static int TP_face_get_mode(void)
{
    PRINT_DBG(">>>> PROXIMITY_SWITCH is %d <<<<\n", PROXIMITY_SWITCH);

    return PROXIMITY_SWITCH;
}

static int TP_face_mode_state(void)
{
    PRINT_DBG(">>>> PROXIMITY_STATE  is %d <<<<\n", PROXIMITY_STATE);

    return PROXIMITY_STATE;
}

static int TP_face_mode_switch(int on)
{
    int proximity_switch;

    proximity_switch = TP_face_get_mode();

    spin_lock(&proximity_switch_lock);

    PRINT_DBG(">>>> begin, proximity_switch is 0x%.2X, on is 0x%.2X. <<<<\n\n",
        PROXIMITY_SWITCH, on);

    if ((1 == on) && (0 == proximity_switch))
    {
        PROXIMITY_SWITCH = 1;

        fts_write_reg(fts_i2c_client,FTS_REG_PROX_SWITCH, FTS_PROX_OPEN);

        goto OUT;
    }
    else if ((0 == on) && (1 == proximity_switch))
    {
        PROXIMITY_SWITCH = 0;

        fts_write_reg(fts_i2c_client,FTS_REG_PROX_SWITCH, FTS_PROX_CLOSE);

        goto OUT;
    }
    else
    {
        spin_unlock(&proximity_switch_lock);

        PRINT_DBG(">>>> On is %d and proximity_switch already is %d, this is invalid! <<<<\n", on, proximity_switch);

        return -EINVAL;
    }


OUT:
    spin_unlock(&proximity_switch_lock);

    PRINT_DBG(">>>> %s(%d), PROXIMITY_SWITCH is %d, PROXIMITY_STATE is %d <<<<\n", __FUNCTION__, on, PROXIMITY_SWITCH, PROXIMITY_STATE);

    PRINT_DBG(">>>> end, proximity_switch is 0x%.2X, on is 0x%.2X. <<<<\n\n",
        PROXIMITY_SWITCH, on);

    return 0;
}

static ssize_t tp_face_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int proximity_switch, proximity_state;

    proximity_switch = TP_face_get_mode();
    proximity_state  = TP_face_mode_state();

	return sprintf(buf,
		__stringify(%d) ":" __stringify(%d) "\n", proximity_switch, proximity_state);
}

static ssize_t tp_face_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    size_t on_off = simple_strtoul(buf, NULL, 10);

    switch (on_off)
    {
        case 0:
        {
            TP_face_mode_switch(0);

            break;
        }
        case 1:
        {
            TP_face_mode_switch(1);

            break;
        }

        default:
            return -EINVAL;
    }

	return count;
}
#endif

static ssize_t tp_information_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned char uc_reg_value; 
	u8 i=0;
	u8 *p_chip_name="unknown!!";
	
	for(i=0;i<support_chip_num;i++)
	{
		if(g_chip_id==fts_chipInfo[i].CHIP_ID)
		{
			p_chip_name=fts_chipInfo[i].FTS_NAME;
			break;
		}
	}
	uc_reg_value = fts_read_fw_ver();
	return sprintf(buf, "IC: %s ID:0x%x\nFirmware version: 0x%.4X.\n",p_chip_name, g_chip_id,uc_reg_value);
}

static struct kobj_attribute tp_information = {
    .attr = {
        .name = "tp_information",
        .mode = S_IRUGO,
    },
    .show  = &tp_information_show,
    .store = NULL,
};
#ifdef TOUCH_VIRTUAL_KEYS

/*******************************************************************************
* Name: virtual_keys_show
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/

static ssize_t virtual_keys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct fts_ts_data *data = i2c_get_clientdata(fts_i2c_client);
	struct fts_platform_data *pdata = data->platform_data;
	return sprintf(buf,"%s:%s:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d\n"
		,__stringify(EV_KEY), __stringify(KEY_MENU),pdata ->virtualkeys[0],pdata ->virtualkeys[1],pdata ->virtualkeys[2],pdata ->virtualkeys[3]
		,__stringify(EV_KEY), __stringify(KEY_HOMEPAGE),pdata ->virtualkeys[4],pdata ->virtualkeys[5],pdata ->virtualkeys[6],pdata ->virtualkeys[7]
		,__stringify(EV_KEY), __stringify(KEY_BACK),pdata ->virtualkeys[8],pdata ->virtualkeys[9],pdata ->virtualkeys[10],pdata ->virtualkeys[11]);
}

static struct kobj_attribute virtual_keys_attr = {
    .attr = {
        .name = "virtualkeys.focaltech_ts",
        .mode = S_IRUGO,
    },
    .show = &virtual_keys_show,
};

#ifdef TP_PROXIMITY_SENSOR
static struct kobj_attribute tp_face_mode_attr = {
    .attr = {
        .name = "facemode",
        .mode = S_IRUGO | S_IWUGO,
    },
    .show  = &tp_face_mode_show, // For read the proximity state by upper level;
    .store = &tp_face_mode_store, // For open/close the proximity switch by upper level;
};
#endif
static struct attribute *properties_attrs[] = {
    &virtual_keys_attr.attr,
#ifdef TP_PROXIMITY_SENSOR // Add facemode attribute file for proximity;
    &tp_face_mode_attr.attr,
#endif
    &tp_information.attr,
    NULL
};

static struct attribute_group properties_attr_group = {
    .attrs = properties_attrs,
};

/*******************************************************************************
* Name: fts_virtual_keys_init
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/

static void fts_virtual_keys_init(void)
{
    int ret = 0;
    struct kobject *properties_kobj;

    pr_info("[FST] %s\n",__func__);

    properties_kobj = kobject_create_and_add("board_properties", NULL);
    if (properties_kobj)
        ret = sysfs_create_group(properties_kobj,
                     &properties_attr_group);
    if (!properties_kobj || ret)
        pr_err("failed to create board_properties\n");
}

#endif

/*******************************************************************************
* Name: fts_i2c_read
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_i2c_read(struct i2c_client *client, char *writebuf, int writelen, char *readbuf, int readlen)
{
	int ret;

	mutex_lock(&i2c_rw_access);
	
	if (writelen > 0) {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = 0,
				 .len = writelen,
				 .buf = writebuf,
			 },
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret < 0)
			dev_err(&client->dev, "%s: i2c read error.\n", __func__);
	} else {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 1);
		if (ret < 0)
			dev_err(&client->dev, "%s:i2c read error.\n", __func__);
	}

	mutex_unlock(&i2c_rw_access);
	
	return ret;
}

/*******************************************************************************
* Name: fts_i2c_write
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_i2c_write(struct i2c_client *client, char *writebuf, int writelen)
{
	int ret;

	struct i2c_msg msgs[] = {
		{
			 .addr = client->addr,
			 .flags = 0,
			 .len = writelen,
			 .buf = writebuf,
		 },
	};
	mutex_lock(&i2c_rw_access);
	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s: i2c write error.\n", __func__);

	mutex_unlock(&i2c_rw_access);
	
	return ret;
}

/*******************************************************************************
* Name: fts_write_reg
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_write_reg(struct i2c_client *client, u8 addr, const u8 val)
{
	u8 buf[2] = {0};

	buf[0] = addr;
	buf[1] = val;

	return fts_i2c_write(client, buf, sizeof(buf));
}

/*******************************************************************************
* Name: fts_read_reg
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_read_reg(struct i2c_client *client, u8 addr, u8 *val)
{
	return fts_i2c_read(client, &addr, 1, val, 1);
}

/*******************************************************************************
* Name: fts_read_fw_ver
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static unsigned char fts_read_fw_ver(void)
{
	unsigned char ver;
	fts_read_reg(fts_i2c_client, FTS_REG_FIRMID, &ver);
	return(ver);
}


/*******************************************************************************
* Name: fts_clear_report_data
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_clear_report_data(struct fts_ts_data *fts)
{
	int i;

	for(i = 0; i < TS_MAX_FINGER; i++) {
	#if MULTI_PROTOCOL_TYPE_B
		input_mt_slot(fts->input_dev, i);
		input_mt_report_slot_state(fts->input_dev, MT_TOOL_FINGER, false);
	#endif
	}
	input_report_key(fts->input_dev, BTN_TOUCH, 0);
	#if !MULTI_PROTOCOL_TYPE_B
		input_mt_sync(fts->input_dev);
	#endif
	input_sync(fts->input_dev);
}

/*******************************************************************************
* Name: fts_update_data
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int fts_update_data(void)
{
	struct fts_ts_data *data = i2c_get_clientdata(fts_i2c_client);
	struct ts_event *event = &data->event;
	u8 buf[33] = {0};
	int ret = -1;
	int i;
	u16 x , y;
	u8 ft_pressure , ft_size;
    #ifdef TP_PROXIMITY_SENSOR
       u8 reg_val = 0;
    #endif

	ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, 33);

	if (ret < 0) {
		pr_err("%s read_data i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}

	memset(event, 0, sizeof(struct ts_event));
	event->touch_point = buf[2] & 0x07;

#ifdef TP_PROXIMITY_SENSOR

    if (1 == PROXIMITY_SWITCH)
    {
        fts_read_reg(fts_i2c_client, FTS_REG_PROX_SWITCH, &reg_val);

        spin_lock(&proximity_state_lock);

        if (1 == PROXIMITY_STATE)
        {
            if (FTS_PROX_FAR == buf[1])
            {
                PROXIMITY_STATE = 0; // far;
            }
        }
        else
        {
            if (FTS_PROX_NEAR == buf[1])
            {
                PROXIMITY_STATE = 1; // close;
            }
        }

        spin_unlock(&proximity_state_lock);

        PRINT_DBG(">>>> reg(0xB0) = 0x%.2X, buf[1] is 0x%.2X, PROXIMITY_STATE = %d <<<<\n", reg_val, buf[1], PROXIMITY_STATE);

        input_report_abs(data->input_dev, ABS_DISTANCE, !PROXIMITY_STATE);
	 input_sync(data->input_dev);
    }

#endif
	for(i = 0; i < TS_MAX_FINGER; i++) {
		if((buf[6*i+3] & 0xc0) == 0xc0)
			continue;
		x = (s16)(buf[6*i+3] & 0x0F)<<8 | (s16)buf[6*i+4];	
		y = (s16)(buf[6*i+5] & 0x0F)<<8 | (s16)buf[6*i+6];
		ft_pressure = buf[6*i+7];
		if(ft_pressure > 127 || ft_pressure == 0)
			ft_pressure = 127;
		ft_size = (buf[6*i+8]>>4) & 0x0F;
		if(ft_size == 0)
		{
			ft_size = 0x09;
		}
		if((buf[6*i+3] & 0x40) == 0x0) {
		#if MULTI_PROTOCOL_TYPE_B
			input_mt_slot(data->input_dev, buf[6*i+5]>>4);
			input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, true);
		#else
			input_report_abs(data->input_dev, ABS_MT_TRACKING_ID, buf[6*i+5]>>4);
		#endif
			input_report_abs(data->input_dev, ABS_MT_POSITION_X, x);
			input_report_abs(data->input_dev, ABS_MT_POSITION_Y, y);
			//input_report_abs(data->input_dev, ABS_MT_PRESSURE, ft_pressure);
			//input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR, ft_size);
			input_report_key(data->input_dev, BTN_TOUCH, 1);
		#if !MULTI_PROTOCOL_TYPE_B
			input_mt_sync(data->input_dev);
		#endif
			pr_debug("===x%d = %d,y%d = %d ====",i, x, i, y);
		}
		else {
		#if MULTI_PROTOCOL_TYPE_B
			input_mt_slot(data->input_dev, buf[6*i+5]>>4);
			input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, false);
		#endif
		}
	}
	if(0 == event->touch_point) {
		for(i = 0; i < TS_MAX_FINGER; i ++) {
			#if MULTI_PROTOCOL_TYPE_B
                            input_mt_slot(data->input_dev, i);
                            input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, false);
			#endif
		}
		input_report_key(data->input_dev, BTN_TOUCH, 0);
		#if !MULTI_PROTOCOL_TYPE_B
			input_mt_sync(data->input_dev);
		#endif

	}
	input_sync(data->input_dev);

	return 0;
}

#if USE_WAIT_QUEUE

/*******************************************************************************
* Name: touch_event_handler
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int touch_event_handler(void *unused)
{
	struct sched_param param = { .sched_priority = 5 };
	sched_setscheduler(current, SCHED_RR, &param);
	u8 state;
	do {
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, (0 != tpd_flag));
		tpd_flag = 0;
		set_current_state(TASK_RUNNING);
	#if FTS_GESTRUE_EN
		if(ft_gesture_enable !=TP_GESTURE_OFF)//huafeizhou160319 add
		{
			if(bsuspend == 1)
			{
				fts_read_reg(fts_i2c_client, 0xd0, &state);
				if( state ==1)
				{
			                fts_read_Gestruedata();
			                continue;
				}
			}
		}
	#endif
		fts_update_data();

	} while (!kthread_should_stop());

	return 0;
}
#endif

#if USE_WORK_QUEUE

/*******************************************************************************
* Name: fts_pen_irq_work
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_pen_irq_work(struct work_struct *work)
{
	fts_update_data();
	enable_irq(fts_i2c_client->irq);
}
#endif

/*******************************************************************************
* Name: fts_interrupt
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static irqreturn_t fts_interrupt(int irq, void *dev_id)
{
#if USE_WAIT_QUEUE
	//disable_irq_nosync(fts_i2c_client->irq);
	tpd_flag = 1;
	wake_up_interruptible(&waiter);
	return IRQ_HANDLED;
#endif

#if USE_WORK_QUEUE
	disable_irq_nosync(fts_i2c_client->irq);
	struct fts_ts_data *fts = (struct fts_ts_data *)dev_id;

	if (!work_pending(&fts->pen_event_work)) {
		queue_work(fts->fts_workqueue, &fts->pen_event_work);
	}
	return IRQ_HANDLED;
#endif

#if USE_THREADED_IRQ
	fts_update_data();
	return IRQ_HANDLED;
#endif

}

/*******************************************************************************
* Name: fts_reset
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_reset(void)
{
	struct fts_platform_data *pdata = fts_wq_data->platform_data;

	gpio_direction_output(pdata->reset_gpio_number, 1);
	msleep(1);
	gpio_set_value(pdata->reset_gpio_number, 0);
	msleep(10);
	gpio_set_value(pdata->reset_gpio_number, 1);
	msleep(200);
}

#ifdef CONFIG_HAS_EARLYSUSPEND

/*******************************************************************************
* Name: fts_ts_suspend
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_ts_suspend(struct early_suspend *handler)
{
	int ret = -1;
	int count = 5;
	pr_info("==%s==\n", __FUNCTION__);

#ifdef TP_PROXIMITY_SENSOR
	if(PROXIMITY_SWITCH)
	{
	    pr_info("***%s(proximity is opened,tp should not suspand!!!)***\n",__func__);
		return 0;
	}
#endif

#if FTS_GESTRUE_EN
	if(ft_gesture_enable !=TP_GESTURE_OFF) //huafeizhou160319 add
	{
		fts_write_reg(fts_i2c_client, 0xd0, 0x01);
		irq_set_irq_type(fts_i2c_client->irq, IRQF_TRIGGER_LOW);//huafeizhou160319 add
		if (fts_updateinfo_curr.CHIP_ID==0x54 || fts_updateinfo_curr.CHIP_ID==0x58 || fts_updateinfo_curr.CHIP_ID==0x86 || fts_updateinfo_curr.CHIP_ID==0x87)
		{
			fts_write_reg(fts_i2c_client, 0xd1, 0xff);
			fts_write_reg(fts_i2c_client, 0xd2, 0xff);
			fts_write_reg(fts_i2c_client, 0xd5, 0xff);
			fts_write_reg(fts_i2c_client, 0xd6, 0xff);
			fts_write_reg(fts_i2c_client, 0xd7, 0xff);
			fts_write_reg(fts_i2c_client, 0xd8, 0xff);
		}
		bsuspend = 1;
		return 0;
	}
#endif
	ret = fts_write_reg(fts_i2c_client, FTS_REG_PMODE, PMODE_HIBERNATE);
	if(ret){
		PRINT_ERR("==fts_suspend==  fts_write_reg fail\n");
	}
	disable_irq(fts_i2c_client->irq);
	fts_clear_report_data(fts_wq_data);
	bsuspend = 1;

	pr_info("***%s!!!)***\n",__func__);
}

/*******************************************************************************
* Name: fts_ts_resume
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_ts_resume(struct early_suspend *handler)
{
	struct fts_ts_data  *fts = (struct fts_ts_data *)i2c_get_clientdata(fts_i2c_client);
	queue_work(fts->fts_resume_workqueue, &fts->resume_work);
}

/*******************************************************************************
* Name: fts_resume_work
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_resume_work(struct work_struct *work)
{
	pr_info("==%s==\n", __FUNCTION__);

#ifdef TP_PROXIMITY_SENSOR
    if ((PROXIMITY_SWITCH) && (bsuspend==0) )
	{
	    pr_info("***%s(tp is not suspand,proximity is already opened)***\n",__func__);
		return 0;
	}
#endif

#if FTS_GESTRUE_EN
	if(ft_gesture_enable !=TP_GESTURE_OFF) //huafeizhou160319 add
	{
		fts_write_reg(fts_i2c_client,0xD0,0x00);
		irq_set_irq_type(fts_i2c_client->irq, IRQF_TRIGGER_FALLING);//huafeizhou160319 add
	}
#endif
	fts_reset();
	enable_irq(fts_i2c_client->irq);
	msleep(2);
	fts_clear_report_data(fts_wq_data);


#ifdef TP_PROXIMITY_SENSOR //huafeizhou160114 add
	 pr_info("***%s(PROXIMITY_SWITCH=%d,bsuspend=%d)***\n",__func__,PROXIMITY_SWITCH,bsuspend);
     if ((PROXIMITY_SWITCH) && (bsuspend==1) )
	 {
	    printk("***proximity need re-open!\n");
	    TP_face_mode_switch(0);

		TP_face_mode_switch(1);
	 }
#endif

	bsuspend = 0;
}
#endif

/*******************************************************************************
* Name: fts_hw_init
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_hw_init(struct fts_ts_data *fts)
{
	struct regulator *reg_vdd;
	struct i2c_client *client = fts->client;
	struct fts_platform_data *pdata = fts->platform_data;

	pr_info("[FST] %s [irq=%d];[rst=%d]\n",__func__,
		pdata->irq_gpio_number,pdata->reset_gpio_number);
	gpio_request(pdata->irq_gpio_number, "ts_irq_pin");
	gpio_request(pdata->reset_gpio_number, "ts_rst_pin");
	gpio_direction_output(pdata->reset_gpio_number, 1);
	gpio_direction_input(pdata->irq_gpio_number);

	reg_vdd = regulator_get(&client->dev, pdata->vdd_name);
	if (!WARN(IS_ERR(reg_vdd), "[FST] fts_hw_init regulator: failed to get %s.\n", pdata->vdd_name)) {
		regulator_set_voltage(reg_vdd, 2800000, 2800000);
		regulator_enable(reg_vdd);
	}
	msleep(100);
	fts_reset();
}

#ifdef CONFIG_OF

/*******************************************************************************
* Name: fts_parse_dt
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static struct fts_platform_data *fts_parse_dt(struct device *dev)
{
	struct fts_platform_data *pdata;
	struct device_node *np = dev->of_node;
	int ret;

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		dev_err(dev, "Could not allocate struct fts_platform_data");
		return NULL;
	}
	pdata->reset_gpio_number = of_get_gpio(np, 0);

	pr_info("[FST] pdata->reset_gpio_number= %d\n", pdata->reset_gpio_number);
	if(pdata->reset_gpio_number < 0){
		dev_err(dev, "fail to get reset_gpio_number\n");
		goto fail;
	}
	pdata->irq_gpio_number = of_get_gpio(np, 1);

	pr_info("[FST] pdata->irq_gpio_number= %d\n", pdata->irq_gpio_number);
	if(pdata->irq_gpio_number < 0){
		dev_err(dev, "fail to get irq_gpio_number\n");
		goto fail;
	}
	ret = of_property_read_string(np, "vdd_name", &pdata->vdd_name);
	if(ret){
		dev_err(dev, "fail to get vdd_name\n");
		goto fail;
	}
	ret = of_property_read_u32_array(np, "virtualkeys", &pdata->virtualkeys,12);
	if(ret){
		dev_err(dev, "fail to get virtualkeys\n");
		goto fail;
	}
	ret = of_property_read_u32(np, "TP_MAX_X", &pdata->TP_MAX_X);

	pr_info("[FST] TP_MAX_X= %d\n", pdata->TP_MAX_X);
	if(ret){
		dev_err(dev, "fail to get TP_MAX_X\n");
		goto fail;
	}
	ret = of_property_read_u32(np, "TP_MAX_Y", &pdata->TP_MAX_Y);

	pr_info("[FST] TP_MAX_Y= %d\n", pdata->TP_MAX_Y);
	if(ret){
		dev_err(dev, "fail to get TP_MAX_Y\n");
		goto fail;
	}

	return pdata;
fail:
	kfree(pdata);
	return NULL;
}
#endif

/*******************************************************************************
* Name: fts_probe
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int fts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct fts_ts_data *fts;
	struct input_dev *input_dev;
	struct fts_platform_data *pdata = client->dev.platform_data;
	int err = 0;
	unsigned char uc_reg_value;
	u8 chip_id,i;

	pr_info("[FST] %s: probe\n",__func__);
#ifdef CONFIG_OF
	struct device_node *np = client->dev.of_node;
	if (np && !pdata){
		pdata = fts_parse_dt(&client->dev);
		if(pdata){
			client->dev.platform_data = pdata;
		}
		else{
			err = -ENOMEM;
			goto exit_alloc_platform_data_failed;
		}
	}
#endif
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}

	fts = kzalloc(sizeof(*fts), GFP_KERNEL);
	if (!fts)	{
		err = -ENOMEM;
		goto exit_alloc_data_failed;
	}

	fts_wq_data = fts;
	fts->platform_data = pdata;
	fts_i2c_client = client;
	fts->client = client;
	fts_hw_init(fts);
	i2c_set_clientdata(client, fts);
	client->irq = gpio_to_irq(pdata->irq_gpio_number);

	#if(defined(CONFIG_I2C_SPRD) ||defined(CONFIG_I2C_SPRD_V1))
	sprd_i2c_ctl_chg_clk(client->adapter->nr, 400000);
	#endif

	err = fts_read_reg(fts_i2c_client, FTS_REG_CIPHER, &uc_reg_value);
	g_chip_id=uc_reg_value;	
	support_chip_num=sizeof(fts_chipInfo)/sizeof(struct fts_chip_Info);
    pr_info("[FST] read chip id 0x%x\n", uc_reg_value);
	if (err < 0)
	{
		pr_err("[FST] read chip id error %x\n", uc_reg_value);
		err = -ENODEV;
		goto exit_chip_check_failed;
	}
       
#if USE_WORK_QUEUE
	INIT_WORK(&fts->pen_event_work, fts_pen_irq_work);

	fts->fts_workqueue = create_singlethread_workqueue("focal-work-queue");
	if (!fts->fts_workqueue) {
		err = -ESRCH;
		goto exit_create_singlethread;
	}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	INIT_WORK(&fts->resume_work, fts_resume_work);
	fts->fts_resume_workqueue = create_singlethread_workqueue("fts_resume_work");
	if (!fts->fts_resume_workqueue) {
		err = -ESRCH;
		goto exit_create_singlethread;
	}
#endif
	input_dev = input_allocate_device();
	if (!input_dev) {
		err = -ENOMEM;
		dev_err(&client->dev, "[FST] failed to allocate input device\n");
		goto exit_input_dev_alloc_failed;
	}

	fts_input_dev = input_dev;
	
#ifdef TOUCH_VIRTUAL_KEYS
	fts_virtual_keys_init();
#endif
	fts->input_dev = input_dev;

	__set_bit(ABS_MT_TOUCH_MAJOR, input_dev->absbit);
	__set_bit(ABS_MT_POSITION_X, input_dev->absbit);
	__set_bit(ABS_MT_POSITION_Y, input_dev->absbit);
	__set_bit(ABS_MT_WIDTH_MAJOR, input_dev->absbit);
	__set_bit(KEY_MENU,  input_dev->keybit);
	__set_bit(KEY_BACK,  input_dev->keybit);
	__set_bit(KEY_HOMEPAGE,  input_dev->keybit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
#ifdef TP_PROXIMITY_SENSOR
	__set_bit(ABS_DISTANCE, input_dev->absbit);
#endif

#if MULTI_PROTOCOL_TYPE_B
	input_mt_init_slots(input_dev, TS_MAX_FINGER,0);
#endif
	input_set_abs_params(input_dev,ABS_MT_POSITION_X, 0, pdata->TP_MAX_X, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_POSITION_Y, 0, pdata->TP_MAX_Y, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
#ifdef TP_PROXIMITY_SENSOR
    input_set_abs_params(input_dev, ABS_DISTANCE, 0, 1, 0, 0);
#endif
	//input_set_abs_params(input_dev,ABS_MT_PRESSURE, 0, 127, 0,0);
#if !MULTI_PROTOCOL_TYPE_B
	input_set_abs_params(input_dev,ABS_MT_TRACKING_ID, 0, 255, 0, 0);
#endif

	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);

#ifdef TP_PROXIMITY_SENSOR
    spin_lock_init(&proximity_switch_lock);
    spin_lock_init(&proximity_state_lock);
#endif
	input_dev->name = FOCALTECH_TS_NAME;
	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev,
		"[FST] fts_probe: failed to register input device: %s\n",
		dev_name(&client->dev));
		goto exit_input_register_device_failed;
	}

#if USE_THREADED_IRQ
	err = request_threaded_irq(client->irq, NULL, fts_interrupt, 
		IRQF_TRIGGER_FALLING | IRQF_ONESHOT | IRQF_NO_SUSPEND, client->name, fts);
#else
	err = request_irq(client->irq, fts_interrupt,
		IRQF_TRIGGER_FALLING | IRQF_ONESHOT | IRQF_NO_SUSPEND, client->name, fts);
#endif
	if (err < 0) {
		dev_err(&client->dev, "[FST] ft5x0x_probe: request irq failed %d\n",err);
		goto exit_irq_request_failed;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	fts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	fts->early_suspend.suspend = fts_ts_suspend;
	fts->early_suspend.resume	= fts_ts_resume;
	register_early_suspend(&fts->early_suspend);
#endif

fts_get_upgrade_array();

#ifdef CONFIG_GST_TP_GESTURE_SUPPORT
	tp_gesture_class = class_create(THIS_MODULE, "ctp_class");
	if (IS_ERR(tp_gesture_class))
	{
	    pr_err("Failed to create class(gesture)!\n");
	    goto exit_fail_create_gesture_class;
	}

	tp_gesture_dev = device_create(tp_gesture_class, NULL, 0, NULL, "ctp_dev");
	if (IS_ERR(tp_gesture_dev))
	{
	    pr_err("Failed to create device(gesture)!\n");
	    goto exit_fail_create_gesture_device;
	}

	if (device_create_file(tp_gesture_dev, &dev_attr_enable) < 0)
	{
	    pr_err("Failed to create file(gesture_store)!\n");
	    goto exit_fail_create_gesture_device_file;
	}
#endif // Endif CONFIG_GST_TP_GESTURE_SUPPORT
#ifdef FTS_SYSFS_DEBUG
fts_create_sysfs(client);
#endif

#ifdef FTS_CTL_IIC	
if (fts_rw_iic_drv_init(client) < 0)	
{
	dev_err(&client->dev, "%s:[FTS] create fts control iic driver failed\n",	__func__);
}
#endif

#if FTS_GESTRUE_EN
	fts_Gesture_init(input_dev);
	 if (fts_updateinfo_curr.CHIP_ID != 0x54 &&  fts_updateinfo_curr.CHIP_ID != 0x58 &&  fts_updateinfo_curr.CHIP_ID != 0x86 &&  fts_updateinfo_curr.CHIP_ID != 0x87)
	 {
	  	init_para(pdata->TP_MAX_X,pdata->TP_MAX_Y,0,0,0);
	 }
#endif

#ifdef FTS_APK_DEBUG
	fts_create_apk_debug_channel(client);
#endif


#ifdef FTS_AUTO_UPGRADE
	printk("********************Enter CTP Auto Upgrade********************\n");
	fts_ctpm_auto_upgrade(client);
#endif  


#if USE_WAIT_QUEUE
	thread = kthread_run(touch_event_handler, 0, "focal-wait-queue");
	if (IS_ERR(thread))
	{
		err = PTR_ERR(thread);
		PRINT_ERR("failed to create kernel thread: %d\n", err);
	}
#endif

	return 0;
#ifdef CONFIG_GST_TP_GESTURE_SUPPORT
exit_fail_create_gesture_device_file:
    device_remove_file(tp_gesture_dev, &dev_attr_enable);

exit_fail_create_gesture_device:
    device_destroy(tp_gesture_class, tp_gesture_dev);
    tp_gesture_dev = NULL;

exit_fail_create_gesture_class:
    class_destroy(tp_gesture_class);
    tp_gesture_class = NULL;
#endif // Endif CONFIG_GST_TP_GESTURE_SUPPORT
exit_irq_request_failed:
	input_unregister_device(input_dev);
exit_input_register_device_failed:
	input_free_device(input_dev);
exit_input_dev_alloc_failed:
exit_create_singlethread:
exit_chip_check_failed:
	gpio_free(pdata->irq_gpio_number);
	gpio_free(pdata->reset_gpio_number);
	kfree(fts);
exit_alloc_data_failed:
exit_check_functionality_failed:
	fts = NULL;
	i2c_set_clientdata(client, fts);
exit_alloc_platform_data_failed:
	return err;
}

/*******************************************************************************
* Name: fts_remove
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int fts_remove(struct i2c_client *client)
{
	struct fts_ts_data *fts = i2c_get_clientdata(client);

	pr_info("==fts_remove=\n");
	
	#ifdef FTS_APK_DEBUG
		fts_release_apk_debug_channel();
	#endif

	#ifdef FTS_SYSFS_DEBUG
		fts_release_sysfs(fts_i2c_client);
	#endif

	#ifdef FTS_CTL_IIC
		fts_rw_iic_drv_exit();
	#endif
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&fts->early_suspend);
#endif
	free_irq(client->irq, fts);
	input_unregister_device(fts->input_dev);
	input_free_device(fts->input_dev);
#if USE_WORK_QUEUE
	cancel_work_sync(&fts->pen_event_work);
	destroy_workqueue(fts->fts_workqueue);
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
	cancel_work_sync(&fts->resume_work);
	destroy_workqueue(fts->fts_resume_workqueue);
#endif
	kfree(fts);
	fts = NULL;
	i2c_set_clientdata(client, fts);

	return 0;
}

static const struct i2c_device_id fts_id[] = {
	{ FOCALTECH_TS_NAME, 0 },{ }
};

/*******************************************************************************
* Name: fts_suspend
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int fts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	PRINT_INFO("fts_suspend\n");
	return 0;
}

/*******************************************************************************
* Name: fts_resume
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int fts_resume(struct i2c_client *client)
{
	PRINT_INFO("fts_resume\n");
	return 0;
}

MODULE_DEVICE_TABLE(i2c, fts_id);

static const struct of_device_id focaltech_of_match[] = {
       { .compatible = "focaltech,focaltech_ts", },
       { }
};
MODULE_DEVICE_TABLE(of, focaltech_of_match);
static struct i2c_driver fts_driver = {
	.probe		= fts_probe,
	.remove		= fts_remove,
	.id_table	= fts_id,
	.driver	= {
		.name	= FOCALTECH_TS_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = focaltech_of_match,
	},
	.suspend = fts_suspend,
	.resume = fts_resume,
};

/*******************************************************************************
* Name: fts_init
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static int __init fts_init(void)
{
	return i2c_add_driver(&fts_driver);
}

/*******************************************************************************
* Name: fts_exit
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void __exit fts_exit(void)
{
	i2c_del_driver(&fts_driver);
}

module_init(fts_init);
module_exit(fts_exit);

MODULE_AUTHOR("<mshl>");
MODULE_DESCRIPTION("FocalTech fts TouchScreen driver");
MODULE_LICENSE("GPL");

