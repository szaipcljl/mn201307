/*
 * drivers/input/touchscreen/ft5x0x_ts.c
 *
 * FocalTech ft5x0x TouchScreen driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
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
 *    1.0		  2010-01-05			WenFS
 *
 * note: only support mulititouch	Wenfs 2010-10-01
 */

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

#if(defined(CONFIG_I2C_SPRD) ||defined(CONFIG_I2C_SPRD_V1))
#include <soc/sprd/i2c-sprd.h>
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

//#define FT53X6_DBG
#ifdef FT53X6_DBG
#define ENTER printk(KERN_INFO "[FT53X6_DBG] func: %s  line: %04d\n", __func__, __LINE__);
#define PRINT_DBG(x...)  printk(KERN_INFO "[FT53X6_DBG] " x)
#define PRINT_INFO(x...)  printk(KERN_INFO "[FT53X6_INFO] " x)
#define PRINT_WARN(x...)  printk(KERN_INFO "[FT53X6_WARN] " x)
#define PRINT_ERR(format,x...)  printk(KERN_ERR "[FT53X6_ERR] func: %s  line: %04d  info: " format, __func__, __LINE__, ## x)
#else
#define ENTER
#define PRINT_DBG(x...)
#define PRINT_INFO(x...)  printk(KERN_INFO "[FT53X6_INFO] " x)
#define PRINT_WARN(x...)  printk(KERN_INFO "[FT53X6_WARN] " x)
#define PRINT_ERR(format,x...)  printk(KERN_ERR "[FT53X6_ERR] func: %s  line: %04d  info: " format, __func__, __LINE__, ## x)
#endif

#define APK_DEBUG
#define SPRD_AUTO_UPGRADE
#define SYSFS_DEBUG
#define FTS_CTL_IIC
#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //huafeizhou141218 add
#define FTS_GESTRUE
#endif
#include <linux/i2c/focaltech.h>
#include <linux/i2c/focaltech_ex_fun.h>
#include <linux/i2c/focaltech_ctl.h>

#define	USE_WAIT_QUEUE	0
#define	USE_THREADED_IRQ	0
#define	USE_WORK_QUEUE	1 //work queue can avoid background music not fluent.

#define	TOUCH_VIRTUAL_KEYS
#define	MULTI_PROTOCOL_TYPE_B	1
#ifdef CONFIG_ARCH_SCX35LT8
#define	TS_MAX_FINGER		2
#else
#define	TS_MAX_FINGER		5
#endif
#define	FTS_PACKET_LENGTH	128

#if USE_WAIT_QUEUE
static struct task_struct *thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(waiter);
static int tpd_flag = 0;
#endif

#if 0
static unsigned char FT5316_FW[]=
{
#include "ft5316_720p.i"
};

static unsigned char FT5306_FW[]=
{
#include "ft5306_qHD.i"
};

static unsigned char *CTPM_FW = FT5306_FW;
#endif
//static int fw_size;

static struct ft5x0x_ts_data *g_ft5x0x_ts;
static struct i2c_client *this_client;

//static unsigned char suspend_flag = 0;
u8 g_chip_id=0,support_chip_num=11;//huafeizhou150325 add

 struct Upgrade_Info fts_updateinfo[] =
{
        {0x55,"FT5x06",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x03, 1, 2000},
        {0x08,"FT5606",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x06, 100, 2000},
	{0x0a,"FT5x16",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x07, 1, 1500},
	{0x05,"FT6208",TPD_MAX_POINTS_2,AUTO_CLB_NONEED,60, 30, 0x79, 0x05, 10, 2000},
	{0x06,"FT6x06",TPD_MAX_POINTS_2,AUTO_CLB_NONEED,100, 30, 0x79, 0x08, 10, 2000},
	{0x55,"FT5x06i",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x03, 1, 2000},
	{0x14,"FT5336",TPD_MAX_POINTS_5,AUTO_CLB_NEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x13,"FT3316",TPD_MAX_POINTS_5,AUTO_CLB_NEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x12,"FT5436i",TPD_MAX_POINTS_5,AUTO_CLB_NEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x11,"FT5336i",TPD_MAX_POINTS_5,AUTO_CLB_NEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x36,"FT6x36",TPD_MAX_POINTS_2,AUTO_CLB_NONEED,100, 30, 0x79, 0x18, 10, 2000},
};
				
struct Upgrade_Info fts_updateinfo_curr;
#ifdef FTS_GESTRUE
#define GESTURE_LEFT		0x20
#define GESTURE_RIGHT		0x21
#define GESTURE_UP		    0x22
#define GESTURE_DOWN		0x23
#define GESTURE_DOUBLECLICK	0x24
#define GESTURE_O		    0x30
#define GESTURE_W		    0x31
#define GESTURE_M		    0x32
#define GESTURE_E		    0x33
#define GESTURE_C		    0x34
#define GESTURE_a		    0x36  //huafeizhou150206 add  /*used for a,not support A*/
#define GESTURE_S		    0x46
#define GESTURE_V		    0x54
#define GESTURE_v		    0x53  //huafeizhou150206 add  /*used for ^*/
#define GESTURE_Z		    0x41

#include "ft_gesture_lib.h"

#define FTS_GESTRUE_POINTS 255
#define FTS_GESTRUE_POINTS_ONETIME  62
#define FTS_GESTRUE_POINTS_HEADER 8
#define FTS_GESTURE_OUTPUT_ADRESS 0xD3
#define FTS_GESTURE_OUTPUT_UNIT_LENGTH 4

//suspend_state_t get_suspend_state(void);

unsigned short coordinate_x[150] = {0};
unsigned short coordinate_y[150] = {0};
//gesture_int_flag:0->interrupt singal can be processed. 1->interrupt singal is discarded.
//Gesture interrupt is low level triger, so interrupt controller will be repeatedly trigered.
unsigned short gesture_int_flag = 0;//liuhui add.
extern int irq_set_irq_type(unsigned int irq, unsigned int type);
#endif

#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //huafeizhou141218 add
static struct class  *tp_gesture_class;
static struct device *tp_gesture_dev;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);

// Fixme;
static DEVICE_ATTR(enable, 4775, gesture_enable_show, gesture_enable_store);
//static DEVICE_ATTR(mode, 4775, gesture_mode_show, gesture_mode_store);


enum {
    TP_GESTURE_DISABLE = 0,
    TP_GESTURE_ENABLE,
};

enum {
    TP_GESTURE_OFF = 0x00,
    TP_GESTURE_DOBULECLICK = 0x01,
    TP_GESTURE_UP=0x02,
    TP_GESTURE_C=0x04,
    TP_GESTURE_O=0x08,
    TP_GESTURE_E=0x10,
    TP_GESTURE_V=0x20,
    TP_GESTURE_M=0x40,
    TP_GESTURE_W=0x80,
    TP_GESTURE_S=0x100,
    TP_GESTURE_Z=0x200,
    TP_GESTURE_DOWN=0x400,
    TP_GESTURE_LEFT=0x800,
    TP_GESTURE_RIGHT=0x1000,
    TP_GESTURE_v=0x2000,
    TP_GESTURE_a=0x4000,
};

static u16 tp_gesture_enable       = TP_GESTURE_OFF;
static u8 tp_gesture_id       = 0x00;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (sprintf(buf, "tp_gesture_enable = 0x%x! tp_gesture_id=0x%x\n", tp_gesture_enable,tp_gesture_id));
}

static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret       = 0;
	size_t on_off = simple_strtoul(buf, NULL, 10);

#if 1
	tp_gesture_enable=on_off;
#else
    switch (on_off)
    {
        case 0x00:
        {
            tp_gesture_enable = TP_GESTURE_DISABLE;
            break;
        }

        case 0x01:
        {
            tp_gesture_enable = TP_GESTURE_ENABLE;
            break;
        }

        default:
            return -EINVAL;
    }
#endif
	return size;
}

#endif /*CONFIG_GST_TP_GESTURE_SUPPORT*/
#if 0//dennis
/* Attribute */
static ssize_t ft5x0x_show_suspend(struct device* cd,struct device_attribute *attr, char* buf);
static ssize_t ft5x0x_store_suspend(struct device* cd, struct device_attribute *attr,const char* buf, size_t len);
static ssize_t ft5x0x_show_version(struct device* cd,struct device_attribute *attr, char* buf);
static ssize_t ft5x0x_update(struct device* cd, struct device_attribute *attr, const char* buf, size_t len);
#endif
static unsigned char ft5x0x_read_fw_ver(void);

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ft5x0x_ts_suspend(struct early_suspend *handler);
static void ft5x0x_ts_resume(struct early_suspend *handler);
#endif
//static int fts_ctpm_fw_update(void);
//static int fts_ctpm_fw_upgrade_with_i_file(void);
// Add for TP proximity support;
#ifdef TP_PROXIMITY_SENSOR
static int PROXIMITY_SWITCH;
static int PROXIMITY_STATE;

static struct spinlock proximity_switch_lock;
static struct spinlock proximity_state_lock;
#endif

struct ts_event {
	u16	x1;
	u16	y1;
	u16	x2;
	u16	y2;
	u16	x3;
	u16	y3;
	u16	x4;
	u16	y4;
	u16	x5;
	u16	y5;
	u16	pressure;
    u8  touch_point;
};

struct ft5x0x_ts_data {
	spinlock_t irq_lock;
	struct input_dev	*input_dev;
	struct i2c_client	*client;
	struct ts_event	event;
#if USE_WORK_QUEUE
	struct work_struct	pen_event_work;
	struct workqueue_struct	*ts_workqueue;
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct work_struct       resume_work;
	struct workqueue_struct *ts_resume_workqueue;
	struct early_suspend	early_suspend;
#endif
	struct ft5x0x_ts_platform_data	*platform_data;
};

#if 0//dennis
static DEVICE_ATTR(suspend, S_IRUGO | S_IWUSR, ft5x0x_show_suspend, ft5x0x_store_suspend);
static DEVICE_ATTR(update, S_IRUGO | S_IWUSR, ft5x0x_show_version, ft5x0x_update);

static ssize_t ft5x0x_show_suspend(struct device* cd,
				     struct device_attribute *attr, char* buf)
{
	ssize_t ret = 0;

	if(suspend_flag==1)
		sprintf(buf, "FT5x0x Suspend\n");
	else
		sprintf(buf, "FT5x0x Resume\n");

	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t ft5x0x_store_suspend(struct device* cd, struct device_attribute *attr,
		       const char* buf, size_t len)
{
	unsigned long on_off = simple_strtoul(buf, NULL, 10);
	suspend_flag = on_off;

	if(on_off==1)
	{
		pr_info("FT5x0x Entry Suspend\n");
	#ifdef CONFIG_HAS_EARLYSUSPEND
		ft5x0x_ts_suspend(NULL);
	#endif
	}
	else
	{
		pr_info("FT5x0x Entry Resume\n");
	#ifdef CONFIG_HAS_EARLYSUSPEND
		ft5x0x_ts_resume(NULL);
	#endif
	}

	return len;
}

static ssize_t ft5x0x_show_version(struct device* cd,
				     struct device_attribute *attr, char* buf)
{
	ssize_t ret = 0;
	unsigned char uc_reg_value; 

	uc_reg_value = ft5x0x_read_fw_ver();

	sprintf(buf, "ft5x0x firmware id is V%x\n", uc_reg_value);

	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t ft5x0x_update(struct device* cd, struct device_attribute *attr,
		       const char* buf, size_t len)
{
	unsigned long on_off = simple_strtoul(buf, NULL, 10);
	unsigned char uc_reg_value;

	uc_reg_value = ft5x0x_read_fw_ver();

	if(on_off==1)
	{
		pr_info("ft5x0x update, current firmware id is V%x\n", uc_reg_value);
		//fts_ctpm_fw_update();
		fts_ctpm_fw_upgrade_with_i_file();
	}

	return len;
}

static int ft5x0x_create_sysfs(struct i2c_client *client)
{
	int err;
	struct device *dev = &(client->dev);

	err = device_create_file(dev, &dev_attr_suspend);
	err = device_create_file(dev, &dev_attr_update);

	return err;
}
#endif

static int ft5x0x_i2c_rxdata(char *rxdata, int length)
{
	int ret = 0;

	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= rxdata,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
		},
	};

	if (i2c_transfer(this_client->adapter, msgs, 2) != 2) {
		ret = -EIO;
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	}

	return ret;
}

static int ft5x0x_i2c_txdata(char *txdata, int length)
{
	int ret = 0;

	struct i2c_msg msg[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= length,
			.buf	= txdata,
		},
	};

	if (i2c_transfer(this_client->adapter, msg, 1) != 1) {
		ret = -EIO;
		pr_err("%s i2c write error: %d\n", __func__, ret);
	}

	return ret;
}

/***********************************************************************************************
Name	:	 ft5x0x_write_reg

Input	:	addr -- address
                     para -- parameter

Output	:

function	:	write register of ft5x0x

***********************************************************************************************/
static int ft5x0x_write_reg(u8 addr, u8 para)
{
	u8 buf[3];
	int ret = -1;

	buf[0] = addr;
	buf[1] = para;
	ret = ft5x0x_i2c_txdata(buf, 2);
	if (ret < 0) {
		pr_err("write reg failed! %#x ret: %d", buf[0], ret);
		return -1;
	}

	return 0;
}

/***********************************************************************************************
Name	:	ft5x0x_read_reg

Input	:	addr
                     pdata

Output	:

function	:	read register of ft5x0x

***********************************************************************************************/
static int ft5x0x_read_reg(u8 addr, u8 *pdata)
{
	int ret = 0;
	u8 buf[2] = {addr, 0};

	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= buf,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= buf+1,
		},
	};

	if (i2c_transfer(this_client->adapter, msgs, 2) != 2) {
		ret = -EIO;
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	}

	*pdata = buf[1];
	return ret;
}

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

        ft5x0x_write_reg(FT5X0X_REG_PROX_SWITCH, FT5X0X_PROX_OPEN);

        goto OUT;
    }
    else if ((0 == on) && (1 == proximity_switch))
    {
        PROXIMITY_SWITCH = 0;

        ft5x0x_write_reg(FT5X0X_REG_PROX_SWITCH, FT5X0X_PROX_CLOSE);

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
		if(g_chip_id==fts_updateinfo[i].CHIP_ID)
		{
			p_chip_name=fts_updateinfo[i].FTS_NAME;
			break;
		}
	}
	uc_reg_value = ft5x0x_read_fw_ver();
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

static ssize_t virtual_keys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);
	struct ft5x0x_ts_platform_data *pdata = data->platform_data;
	return sprintf(buf,"%s:%s:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d\n"
		,__stringify(EV_KEY), __stringify(KEY_MENU),pdata ->virtualkeys[0],pdata ->virtualkeys[1],pdata ->virtualkeys[2],pdata ->virtualkeys[3]
		,__stringify(EV_KEY), __stringify(KEY_HOMEPAGE),pdata ->virtualkeys[4],pdata ->virtualkeys[5],pdata ->virtualkeys[6],pdata ->virtualkeys[7]
		,__stringify(EV_KEY), __stringify(KEY_BACK),pdata ->virtualkeys[8],pdata ->virtualkeys[9],pdata ->virtualkeys[10],pdata ->virtualkeys[11]
		,__stringify(EV_KEY), __stringify(KEY_LEFTSHIFT),pdata ->virtualkeys[12],pdata ->virtualkeys[13],pdata ->virtualkeys[14],pdata ->virtualkeys[15]);
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

static void ft5x0x_ts_virtual_keys_init(void)
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

/***********************************************************************************************
Name	:	 ft5x0x_read_fw_ver

Input	:	 void

Output	:	 firmware version

function	:	 read TP firmware version

***********************************************************************************************/
static unsigned char ft5x0x_read_fw_ver(void)
{
	unsigned char ver;
	ft5x0x_read_reg(FT5X0X_REG_FIRMID, &ver);
	return(ver);
}

static void ft5x0x_clear_report_data(struct ft5x0x_ts_data *ft5x0x_ts)
{
	int i;

	for(i = 0; i < TS_MAX_FINGER; i++) {
	#if MULTI_PROTOCOL_TYPE_B
		input_mt_slot(ft5x0x_ts->input_dev, i);
		input_mt_report_slot_state(ft5x0x_ts->input_dev, MT_TOOL_FINGER, false);
	#endif
	}
	input_report_key(ft5x0x_ts->input_dev, BTN_TOUCH, 0);
	#if !MULTI_PROTOCOL_TYPE_B
		input_mt_sync(ft5x0x_ts->input_dev);
	#endif
	input_sync(ft5x0x_ts->input_dev);
}

#ifdef FTS_GESTRUE
static void check_gesture(int gesture_id)
{
	struct ft5x0x_ts_data *ft5x0x_ts = i2c_get_clientdata(this_client);	
    	printk("****check_gesture( gesture_id==0x%x)\n ",gesture_id);

	tp_gesture_id=gesture_id;//huafeizhou141220 add
	switch(gesture_id)
	{
		case GESTURE_LEFT:
			if((tp_gesture_enable & TP_GESTURE_LEFT)==TP_GESTURE_LEFT)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F9, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F9, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				//Set gesture_int_flag to avoid interrupt is trigered repeatedly.
				//Normal KEY_F9 can wakeup system, and gesture_int_flag will be clear in resume().
				gesture_int_flag = 1;//liuhui add.
			}
			break;
		case GESTURE_RIGHT:	
			if((tp_gesture_enable & TP_GESTURE_RIGHT)==TP_GESTURE_RIGHT)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F10, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F10, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_UP:
			if((tp_gesture_enable & TP_GESTURE_UP)==TP_GESTURE_UP)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_SCROLLLOCK, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_SCROLLLOCK, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_DOWN:
			if((tp_gesture_enable & TP_GESTURE_DOWN)==TP_GESTURE_DOWN)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F11, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F11, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_DOUBLECLICK:	
			if((tp_gesture_enable & TP_GESTURE_DOBULECLICK)==TP_GESTURE_DOBULECLICK)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_POWER, 1);
			//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_POWER, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_O:	
			if((tp_gesture_enable & TP_GESTURE_O)==TP_GESTURE_O)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F1, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F1, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_W:	
			if((tp_gesture_enable & TP_GESTURE_W)==TP_GESTURE_W)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F2, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F2, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}   
			break;
		case GESTURE_M:		
			if((tp_gesture_enable & TP_GESTURE_M)==TP_GESTURE_M)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F3, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F3, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_E:		
			if((tp_gesture_enable & TP_GESTURE_E)==TP_GESTURE_E)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F4, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F4, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		case GESTURE_C:		
			if((tp_gesture_enable & TP_GESTURE_C)==TP_GESTURE_C)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F5, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F5, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;

		case GESTURE_S:
			if((tp_gesture_enable & TP_GESTURE_S)==TP_GESTURE_S)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F6, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F6, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;

		case GESTURE_V:
			if((tp_gesture_enable & TP_GESTURE_V)==TP_GESTURE_V)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F7, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F7, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;

		case GESTURE_Z:
			if((tp_gesture_enable & TP_GESTURE_Z)==TP_GESTURE_Z)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F8, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F8, 0);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 1);
			//	input_sync(ft5x0x_ts->input_dev);
			//	input_report_key(ft5x0x_ts->input_dev, KEY_U, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;
		//huafeizhou150206 add-s
		case GESTURE_v:
			if((tp_gesture_enable & TP_GESTURE_v)==TP_GESTURE_v)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_F12, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_F12, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;

		case GESTURE_a:
			if((tp_gesture_enable & TP_GESTURE_a)==TP_GESTURE_a)
			{
				input_report_key(ft5x0x_ts->input_dev, KEY_NUMLOCK, 1);
				//msleep(10);
				input_sync(ft5x0x_ts->input_dev);
				input_report_key(ft5x0x_ts->input_dev, KEY_NUMLOCK, 0);
				input_sync(ft5x0x_ts->input_dev);
				gesture_int_flag = 1;
			}
			break;			
		//huafeizhou150206 add-e
		default:
			{
				//Some gesture is not support.
				//The unspported gesture can't wakeup system.
				//Clear gesture_int_flag when interrupt singal disapper.
				//So the system can process next interrupt singal.
				gesture_int_flag = 1;
				msleep(500);//Waiting for interrupt singal disapper.
				gesture_int_flag = 0;
			}
			break;
	}
//Sometimes the system is wakeup by rtc.
//In the condition suspend() and resume() will be called except early suspend.
//If you Draw a gesture in this time, the system can't be wakeup.
//And TP is not in gesture mode.(Not sure what reason be)
//So enable tp gesture here and waiting for next tp gesture.
//TP gesture will be disabled in resume()
	if(gesture_int_flag ==1)
	{
		int ret = fts_write_reg(this_client, 0xd0, 0x01);
		irq_set_irq_type(this_client->irq, IRQF_TRIGGER_LOW);
		if(ret < 0)
			printk("%s, enable tp gesture failed\n", __func__);
	}
}

static int ft5x0x_read_Touchdata(void)
{
    unsigned char buf[FTS_GESTRUE_POINTS * 4] = { 0 };
    int ret = -1;
    int i = 0;
    buf[0] = 0xd3;
    int gestrue_id = 0;
    short pointnum = 0;
    ret = fts_i2c_Read(this_client, buf, 1, buf, FTS_GESTRUE_POINTS_HEADER);
    if (ret < 0)
    {
        printk( "%s read touchdata failed.\n", __func__);
        return ret;
    }
    /* FW ?㊣?車??3?那?那? */
    if (0x24 == buf[0] || 0x20 == buf[0] || 0x21 == buf[0] || 0x22 == buf[0] || 0x23 == buf[0])
    {
        gestrue_id =  buf[0];
        check_gesture(gestrue_id);
        return -1;
    }

    pointnum = (short)(buf[1]) & 0xff;
    buf[0] = 0xd3;
	
	printk( "yedongyue ----%s  buf[0] = %d,pointnum = %d\n", __func__,buf[0],pointnum);
    if((pointnum * 4 + 2+6)<255)
    {
		ret = fts_i2c_Read(this_client, buf, 1, buf, (pointnum * 4 + 2 + 6));
    }
    else
    {
		ret = fts_i2c_Read(this_client, buf, 1, buf, 255);
		ret = fts_i2c_Read(this_client, buf, 0, buf+255, (pointnum * 4 + 2 +6)-255);
    }
    if (ret < 0)
    {
        printk( "%s read touchdata failed.\n", __func__);
        return ret;
    }
   gestrue_id = fetch_object_sample(buf, pointnum);
   printk( "yedongyue ----%s  buf[0] = %d,pointnum = %d,gestrue_id = %d\n", __func__,buf[0],pointnum,gestrue_id);
    for(i = 0;i < pointnum;i++)
    {
        coordinate_x[i] =  (((s16) buf[0 + (4 * i)]) & 0x0F) <<
            8 | (((s16) buf[1 + (4 * i)])& 0xFF);
        coordinate_y[i] = (((s16) buf[2 + (4 * i)]) & 0x0F) <<
            8 | (((s16) buf[3 + (4 * i)]) & 0xFF);
    }
	check_gesture(gestrue_id);
    return -1;
}
#endif
static int ft5x0x_update_data(void)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);
	struct ts_event *event = &data->event;
	u8 buf[33] = {0};
	int ret = -1;
	int i;
	u16 x , y;
	u8 ft_pressure , ft_size;
    #ifdef TP_PROXIMITY_SENSOR
       u8 reg_val = 0;
    #endif

#ifdef FTS_GESTRUE
	if(tp_gesture_enable !=TP_GESTURE_OFF)
	{
		u8 sta;
		i2c_smbus_read_i2c_block_data(this_client, 0xd0, 1, &sta);
		// if((get_suspend_state() == PM_SUSPEND_MEM) && (state ==1))
		 if(sta ==1)
		{
			ft5x0x_read_Touchdata();
			return 0;
		}
	}
#endif
	ret = ft5x0x_i2c_rxdata(buf, 33);

	if (ret < 0) {
		pr_err("%s read_data i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}

	memset(event, 0, sizeof(struct ts_event));
	event->touch_point = buf[2] & 0x07;

#ifdef TP_PROXIMITY_SENSOR

    if (1 == PROXIMITY_SWITCH)
    {
        ft5x0x_read_reg(FT5X0X_REG_PROX_SWITCH, &reg_val);

        spin_lock(&proximity_state_lock);

        if (1 == PROXIMITY_STATE)
        {
            if (FT5X0X_PROX_FAR == buf[1])
            {
                PROXIMITY_STATE = 0; // far;
            }
        }
        else
        {
            if (FT5X0X_PROX_NEAR == buf[1])
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
		if(ft_pressure > 127)
			ft_pressure = 127;
		ft_size = (buf[6*i+8]>>4) & 0x0F;
		if((buf[6*i+3] & 0x40) == 0x0) {
		#if MULTI_PROTOCOL_TYPE_B
			input_mt_slot(data->input_dev, buf[6*i+5]>>4);
			input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, true);
		#endif
			input_report_abs(data->input_dev, ABS_MT_POSITION_X, x);
			input_report_abs(data->input_dev, ABS_MT_POSITION_Y, y);
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
static int touch_event_handler(void *unused)
{
	struct sched_param param = { .sched_priority = 5 };
	sched_setscheduler(current, SCHED_RR, &param);

	pr_info("==%s==\n", __FUNCTION__);
	do {
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, (0 != tpd_flag));
		tpd_flag = 0;
		set_current_state(TASK_RUNNING);
		ft5x0x_update_data();

	} while (!kthread_should_stop());

	return 0;
}
#endif

#if USE_WORK_QUEUE
static void ft5x0x_ts_pen_irq_work(struct work_struct *work)
{
	ft5x0x_update_data();
	enable_irq(this_client->irq);
}
#endif

static irqreturn_t ft5x0x_ts_interrupt(int irq, void *dev_id)
{
#if USE_WAIT_QUEUE
	tpd_flag = 1;
	wake_up_interruptible(&waiter);
	return IRQ_HANDLED;
#endif

#if USE_WORK_QUEUE
	struct ft5x0x_ts_data *ft5x0x_ts = (struct ft5x0x_ts_data *)dev_id;
	unsigned long irqflags;
	spin_lock_irqsave(&ft5x0x_ts->irq_lock, irqflags);
	disable_irq_nosync(ft5x0x_ts->client->irq);
	spin_unlock_irqrestore(&ft5x0x_ts->irq_lock, irqflags);
	if (!work_pending(&ft5x0x_ts->pen_event_work)) {
		#ifdef FTS_GESTRUE
			if(gesture_int_flag == 0)
				queue_work(ft5x0x_ts->ts_workqueue, &ft5x0x_ts->pen_event_work);
		#else
			queue_work(ft5x0x_ts->ts_workqueue, &ft5x0x_ts->pen_event_work);
		#endif
	}
	return IRQ_HANDLED;
#endif

#if USE_THREADED_IRQ
	ft5x0x_update_data();
	return IRQ_HANDLED;
#endif

}

static void ft5x0x_ts_reset(void)
{
	struct ft5x0x_ts_platform_data *pdata = g_ft5x0x_ts->platform_data;

	gpio_direction_output(pdata->reset_gpio_number, 1);
	msleep(10);
	gpio_set_value(pdata->reset_gpio_number, 1);
	msleep(10);
	gpio_set_value(pdata->reset_gpio_number, 0);
	msleep(10);
	gpio_set_value(pdata->reset_gpio_number, 1);
	msleep(200);
}

#if 0
//for future use

struct regulator *vdd28 = NULL;

static void ft53x6_power_off(void)
{
	if(vdd28 != NULL)
		regulator_force_disable(vdd28);
	PRINT_INFO("power off\n");
}

static void ft53x6_power_on(void)
{
	int err = 0;

	if(vdd28 == NULL) {
		vdd28 = regulator_get(NULL, "vdd28");
		if (IS_ERR(vdd28)) {
			PRINT_ERR("regulator_get failed\n");
			return;
		}
		err = regulator_set_voltage(vdd28,2800000,2800000);
		if (err)
			PRINT_ERR("regulator_set_voltage failed\n");
	}
	regulator_enable(vdd28);

	PRINT_INFO("power on\n");
}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ft5x0x_ts_suspend(struct early_suspend *handler)
{
	int ret = -1;
	int count = 5;
	
	pr_info("==%s==\n", __FUNCTION__);
#ifdef TP_PROXIMITY_SENSOR

	if (!!PROXIMITY_SWITCH)
	{
	}
	else
	{
#endif
	#ifdef FTS_GESTRUE
		gesture_int_flag = 0;//liuhui add.
		if(tp_gesture_enable !=TP_GESTURE_OFF)
		{
			printk("***enable tp gesture!!!***\n");
			ret=fts_write_reg(this_client, 0xd0, 0x01);
			irq_set_irq_type(this_client->irq, IRQF_TRIGGER_LOW);//huafeizhou141216 add
			pr_info("==ret=%d==\n", ret);
			//ret = ft5x0x_write_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
			if(ret<0)
			{
				PRINT_ERR("ft5x0x_write_reg fail\n");
			}
			//disable_irq(this_client->irq);
			//ft5x0x_clear_report_data(g_ft5x0x_ts);
			//gpio_set_value(g_ft5x0x_ts->platform_data->reset_gpio_number, 0);//for future use
		}
		else
		{
			printk("***disable tp gesture!!!***\n");
			ret = ft5x0x_write_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
			while(ret == 0 && count != 0) {
			PRINT_ERR("trying to enter hibernate again. ret = %d\n", ret);
			msleep(10);
			ret = ft5x0x_write_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
			count--;
			}
			disable_irq(this_client->irq);
			ft5x0x_clear_report_data(g_ft5x0x_ts);
			//gpio_set_value(g_ft5x0x_ts->platform_data->reset_gpio_number, 0);//for future use
		}
	#else
		ret = ft5x0x_write_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
		while(ret == 0 && count != 0) {
				PRINT_ERR("trying to enter hibernate again. ret = %d\n", ret);
				msleep(10);
				ret = ft5x0x_write_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
				count--;
			}
		disable_irq(this_client->irq);
		ft5x0x_clear_report_data(g_ft5x0x_ts);
		//gpio_set_value(g_ft5x0x_ts->platform_data->reset_gpio_number, 0);//for future use
	#endif
#ifdef TP_PROXIMITY_SENSOR
    }
#endif
}

static void ft5x0x_ts_resume(struct early_suspend *handler)
{
	struct ft5x0x_ts_data  *ft5x0x_ts = (struct ft5x0x_ts_data *)i2c_get_clientdata(this_client);
	queue_work(ft5x0x_ts->ts_resume_workqueue, &ft5x0x_ts->resume_work);
}

static void ft5x0x_ts_resume_work(struct work_struct *work)
{
	int ret;
	unsigned char temp;
	pr_info("==%s==\n", __FUNCTION__);
#ifdef TP_PROXIMITY_SENSOR
	if (!!PROXIMITY_SWITCH)
	{
		PRINT_DBG(">>>>>>>>>>> %s(), do nothing! PROXIMITY_SWITCH is %d. <<<<<\n", __FUNCTION__, PROXIMITY_SWITCH);
		ret = ft5x0x_read_reg(0xA6, &temp);
		if (ret < 0)
		{
			PRINT_DBG(">>>>>>>>>>> %s(), Call reset(). <<<<<<<<<<<<<\n", __FUNCTION__);
			ft5x0x_ts_reset();
		#ifdef FTS_GESTRUE //huafeizhou141218 add
			irq_set_irq_type(this_client->irq, IRQF_TRIGGER_FALLING);//huafeizhou141216 add
			gesture_int_flag = 0;//liuhui add.
		#endif
			enable_irq(this_client->irq);

			if (!!PROXIMITY_SWITCH)
			{
				PRINT_ERR(">>>>>>>>>>> %s(), Enable psensor to fix state error. <<<<<<<<<<<<<\n", __FUNCTION__);
				msleep(500);
				ft5x0x_write_reg(FT5X0X_REG_PROX_SWITCH, FT5X0X_PROX_OPEN); // Enable the proximity feature;
			}
		}

	#ifdef FTS_GESTRUE
		if(tp_gesture_enable !=TP_GESTURE_OFF)
		{
			ret=fts_write_reg(this_client, 0xd0, 0x00);
			irq_set_irq_type(this_client->irq, IRQF_TRIGGER_FALLING);
			gesture_int_flag = 0;//liuhui add.
			if(ret<0)
			{
				PRINT_ERR("ft5x0x_write_reg fail\n");
			}
		}
	#endif
	}
	else
	{
#endif	
		ft5x0x_ts_reset();
		//ft5x0x_write_reg(FT5X0X_REG_PERIODACTIVE, 7);
	#ifdef FTS_GESTRUE //huafeizhou141218 add
		irq_set_irq_type(this_client->irq, IRQF_TRIGGER_FALLING);//huafeizhou141216 add
		gesture_int_flag = 0;//liuhui add.
	#endif
		enable_irq(this_client->irq);
		msleep(2);
		ft5x0x_clear_report_data(g_ft5x0x_ts);
#ifdef TP_PROXIMITY_SENSOR
	}
#endif
}
#endif

static void ft5x0x_ts_hw_init(struct ft5x0x_ts_data *ft5x0x_ts)
{
	struct regulator *reg_vdd;
	struct i2c_client *client = ft5x0x_ts->client;
	struct ft5x0x_ts_platform_data *pdata = ft5x0x_ts->platform_data;

	pr_info("[FST] %s [irq=%d];[rst=%d]\n",__func__,
		pdata->irq_gpio_number,pdata->reset_gpio_number);
	gpio_request(pdata->irq_gpio_number, "ts_irq_pin");
	gpio_request(pdata->reset_gpio_number, "ts_rst_pin");
	gpio_direction_output(pdata->reset_gpio_number, 1);
	gpio_direction_input(pdata->irq_gpio_number);

	reg_vdd = regulator_get(&client->dev, pdata->vdd_name);
	if (!WARN(IS_ERR(reg_vdd), "[FST] ft5x0x_ts_hw_init regulator: failed to get %s.\n", pdata->vdd_name)) {
		if(!strcmp(pdata->vdd_name,"vdd18"))
			regulator_set_voltage(reg_vdd,1800000,1800000);
		if(!strcmp(pdata->vdd_name,"vdd28"))
			regulator_set_voltage(reg_vdd, 2800000, 2800000);
		regulator_enable(reg_vdd);
	}
	msleep(100);
	ft5x0x_ts_reset();
}

void focaltech_get_upgrade_array(struct i2c_client *client)
{

	u8 chip_id;
	u32 i;

	i2c_smbus_read_i2c_block_data(client,FT_REG_CHIP_ID,1,&chip_id);

	printk("%s chip_id = %x\n", __func__, chip_id);

	for(i=0;i<sizeof(fts_updateinfo)/sizeof(struct Upgrade_Info);i++)
	{
		if(chip_id==fts_updateinfo[i].CHIP_ID)
		{
			memcpy(&fts_updateinfo_curr, &fts_updateinfo[i], sizeof(struct Upgrade_Info));
			break;
		}
	}

	if(i >= sizeof(fts_updateinfo)/sizeof(struct Upgrade_Info))
	{
		memcpy(&fts_updateinfo_curr, &fts_updateinfo[0], sizeof(struct Upgrade_Info));
	}
}

#ifdef CONFIG_OF
static struct ft5x0x_ts_platform_data *ft5x0x_ts_parse_dt(struct device *dev)
{
	struct ft5x0x_ts_platform_data *pdata;
	struct device_node *np = dev->of_node;
	int ret,i = 0;
	u32 data[16] = {0};
	u32 tmp_val_u32;
	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		dev_err(dev, "Could not allocate struct ft5x0x_ts_platform_data");
		return NULL;
	}
	pdata->reset_gpio_number = of_get_gpio(np, 0);
	if(pdata->reset_gpio_number < 0){
		dev_err(dev, "fail to get reset_gpio_number\n");
		goto fail;
	}
	pdata->irq_gpio_number = of_get_gpio(np, 1);
	if(pdata->irq_gpio_number < 0){
		dev_err(dev, "fail to get irq_gpio_number\n");
		goto fail;
	}
	ret = of_property_read_string(np, "vdd_name", &pdata->vdd_name);
	if(ret){
		dev_err(dev, "fail to get vdd_name\n");
		goto fail;
	}
	ret = of_property_read_u32_array(np, "virtualkeys", data,16);
	if(ret){
		dev_err(dev, "fail to get virtualkeys\n");
		goto fail;
	}
	for(i = 0; i < 16; i ++){
		pdata->virtualkeys[i] = data[i];
	}
	ret = of_property_read_u32(np, "TP_MAX_X", &tmp_val_u32);
	if(ret){
		dev_err(dev, "fail to get TP_MAX_X\n");
		goto fail;
	}
	pdata->TP_MAX_X = tmp_val_u32;
	ret = of_property_read_u32(np, "TP_MAX_Y", &tmp_val_u32);
	if(ret){
		dev_err(dev, "fail to get TP_MAX_Y\n");
		goto fail;
	}
	pdata->TP_MAX_Y = tmp_val_u32;
	return pdata;
fail:
	kfree(pdata);
	return NULL;
}
#endif

#ifdef GST_TP_COMPATIBLE
static int chip_matching_result = 0;
#endif

static int ft5x0x_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ft5x0x_ts_data *ft5x0x_ts;
	struct input_dev *input_dev;
	struct ft5x0x_ts_platform_data *pdata = client->dev.platform_data;
	int err = 0;
	unsigned char uc_reg_value;
	//u8 chip_id,i;

	pr_info("[FST] %s: probe\n",__func__);
#ifdef CONFIG_OF
	if (client->dev.of_node && !pdata){
		pdata = ft5x0x_ts_parse_dt(&client->dev);
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

	ft5x0x_ts = kzalloc(sizeof(*ft5x0x_ts), GFP_KERNEL);
	if (!ft5x0x_ts)	{
		err = -ENOMEM;
		goto exit_alloc_data_failed;
	}

	g_ft5x0x_ts = ft5x0x_ts;
	ft5x0x_ts->platform_data = pdata;
	this_client = client;
	ft5x0x_ts->client = client;
	ft5x0x_ts_hw_init(ft5x0x_ts);
	i2c_set_clientdata(client, ft5x0x_ts);
	client->irq = gpio_to_irq(pdata->irq_gpio_number);

	#if(defined(CONFIG_I2C_SPRD) || defined(CONFIG_I2C_SPRD_V1))
	sprd_i2c_ctl_chg_clk(client->adapter->nr, 400000);
	#endif

	err = ft5x0x_read_reg(FT5X0X_REG_CIPHER, &uc_reg_value);
	g_chip_id=uc_reg_value;
	support_chip_num=sizeof(fts_updateinfo)/sizeof(struct Upgrade_Info);
	PRINT_INFO("g_chip_id=0x%x,support_chip_num=%d\n",g_chip_id,support_chip_num);
	if (err < 0)
	{
		pr_err("[FST] read chip id error %x\n", uc_reg_value);
		err = -ENODEV;
		goto exit_chip_check_failed;
	}
       
	/* set report rate, about 70HZ */
	//ft5x0x_write_reg(FT5X0X_REG_PERIODACTIVE, 7);
#if USE_WORK_QUEUE
	INIT_WORK(&ft5x0x_ts->pen_event_work, ft5x0x_ts_pen_irq_work);

	ft5x0x_ts->ts_workqueue = create_singlethread_workqueue("focal-work-queue");
	if (!ft5x0x_ts->ts_workqueue) {
		err = -ESRCH;
		goto exit_create_singlethread;
	}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	INIT_WORK(&ft5x0x_ts->resume_work, ft5x0x_ts_resume_work);
	ft5x0x_ts->ts_resume_workqueue = create_singlethread_workqueue("ft5x0x_ts_resume_work");
	if (!ft5x0x_ts->ts_resume_workqueue) {
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
#ifdef TOUCH_VIRTUAL_KEYS
	ft5x0x_ts_virtual_keys_init();
#endif
	ft5x0x_ts->input_dev = input_dev;

	__set_bit(ABS_MT_TOUCH_MAJOR, input_dev->absbit);
	__set_bit(ABS_MT_POSITION_X, input_dev->absbit);
	__set_bit(ABS_MT_POSITION_Y, input_dev->absbit);
	__set_bit(ABS_MT_WIDTH_MAJOR, input_dev->absbit);
	__set_bit(KEY_MENU,  input_dev->keybit);
	__set_bit(KEY_BACK,  input_dev->keybit);
	__set_bit(KEY_HOMEPAGE,  input_dev->keybit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
	__set_bit(KEY_LEFTSHIFT,  input_dev->keybit);
#ifdef TP_PROXIMITY_SENSOR
	__set_bit(ABS_DISTANCE, input_dev->absbit);
#endif

#ifdef FTS_GESTRUE
	__set_bit(KEY_POWER, input_dev->keybit);
	__set_bit(KEY_F1, input_dev->keybit);
	__set_bit(KEY_F2, input_dev->keybit);
	__set_bit(KEY_F3, input_dev->keybit);
	__set_bit(KEY_F4, input_dev->keybit);
	__set_bit(KEY_F5, input_dev->keybit);
	__set_bit(KEY_F6, input_dev->keybit);
	__set_bit(KEY_F7, input_dev->keybit);
	__set_bit(KEY_F8, input_dev->keybit);
	__set_bit(KEY_F9, input_dev->keybit);
	__set_bit(KEY_F10, input_dev->keybit);
	__set_bit(KEY_F11, input_dev->keybit);
	__set_bit(KEY_F12, input_dev->keybit);
	__set_bit(KEY_SCROLLLOCK, input_dev->keybit);
	__set_bit(KEY_NUMLOCK, input_dev->keybit);
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
	#if 0
	/*ft5306's firmware is qhd, ft5316's firmware is 720p*/
	if (uc_reg_value == 0x0a || uc_reg_value == 0x0) {
		input_set_abs_params(input_dev,
			     ABS_MT_POSITION_X, 0, 720, 0, 0);
		input_set_abs_params(input_dev,
			     ABS_MT_POSITION_Y, 0, 1280, 0, 0);
	} else {
		input_set_abs_params(input_dev,
			     ABS_MT_POSITION_X, 0, 540, 0, 0);
		input_set_abs_params(input_dev,
			     ABS_MT_POSITION_Y, 0, 960, 0, 0);
	}
	input_set_abs_params(input_dev,
			     ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(input_dev,
			     ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);

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
		"[FST] ft5x0x_ts_probe: failed to register input device: %s\n",
		dev_name(&client->dev));
		goto exit_input_register_device_failed;
	}

#if USE_THREADED_IRQ
	err = request_threaded_irq(client->irq, NULL, ft5x0x_ts_interrupt, 
		IRQF_TRIGGER_FALLING | IRQF_ONESHOT | IRQF_NO_SUSPEND, client->name, ft5x0x_ts);
#else
	err = request_irq(client->irq, ft5x0x_ts_interrupt,
		IRQF_TRIGGER_FALLING | IRQF_ONESHOT | IRQF_NO_SUSPEND, client->name, ft5x0x_ts);
#endif
	if (err < 0) {
		dev_err(&client->dev, "[FST] ft5x0x_probe: request irq failed %d\n",err);
		goto exit_irq_request_failed;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	ft5x0x_ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ft5x0x_ts->early_suspend.suspend = ft5x0x_ts_suspend;
	ft5x0x_ts->early_suspend.resume	= ft5x0x_ts_resume;
	register_early_suspend(&ft5x0x_ts->early_suspend);
#endif

focaltech_get_upgrade_array(client);

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
#ifdef SYSFS_DEBUG	
fts_create_sysfs(client);
#endif

#ifdef FTS_CTL_IIC	
if (ft_rw_iic_drv_init(client) < 0)	
{
	dev_err(&client->dev, "%s:[FTS] create fts control iic driver failed\n",	__func__);
}
#endif

#ifdef FTS_GESTRUE
	init_para(pdata->TP_MAX_X,pdata->TP_MAX_Y,60,0,0);
    //fts_write_reg(i2c_client, 0xd0, 0x01);
#endif
#ifdef SPRD_AUTO_UPGRADE
	printk("********************Enter CTP Auto Upgrade********************\n");
	fts_ctpm_auto_upgrade(client);
#endif   

#ifdef APK_DEBUG
	ft5x0x_create_apk_debug_channel(client);
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
	kfree(ft5x0x_ts);
exit_alloc_data_failed:
exit_check_functionality_failed:
	ft5x0x_ts = NULL;
	i2c_set_clientdata(client, ft5x0x_ts);
exit_alloc_platform_data_failed:
	return err;
}

static int ft5x0x_ts_remove(struct i2c_client *client)
{
	struct ft5x0x_ts_data *ft5x0x_ts = i2c_get_clientdata(client);

	pr_info("==ft5x0x_ts_remove=\n");
	
	#ifdef SYSFS_DEBUG
	fts_release_sysfs(client);
	#endif
	#ifdef FTS_CTL_IIC	
	ft_rw_iic_drv_exit();
	#endif
	#ifdef APK_DEBUG
	ft5x0x_release_apk_debug_channel();
	#endif
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&ft5x0x_ts->early_suspend);
#endif
	free_irq(client->irq, ft5x0x_ts);
	input_unregister_device(ft5x0x_ts->input_dev);
	input_free_device(ft5x0x_ts->input_dev);
#if USE_WORK_QUEUE
	cancel_work_sync(&ft5x0x_ts->pen_event_work);
	destroy_workqueue(ft5x0x_ts->ts_workqueue);
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
	cancel_work_sync(&ft5x0x_ts->resume_work);
	destroy_workqueue(ft5x0x_ts->ts_resume_workqueue);
#endif
	kfree(ft5x0x_ts);
	ft5x0x_ts = NULL;
	i2c_set_clientdata(client, ft5x0x_ts);

	return 0;
}

static const struct i2c_device_id ft5x0x_ts_id[] = {
	{ FOCALTECH_TS_NAME, 0 },{ }
};

static int ft5x0x_suspend(struct i2c_client *client, pm_message_t mesg)
{
	PRINT_INFO("ft5x0x_suspend\n");
	return 0;
}
static int ft5x0x_resume(struct i2c_client *client)
{
	PRINT_INFO("ft5x0x_resume\n");
	return 0;
}

MODULE_DEVICE_TABLE(i2c, ft5x0x_ts_id);

static const struct of_device_id focaltech_of_match[] = {
       { .compatible = "focaltech,focaltech_ts", },
       { }
};
MODULE_DEVICE_TABLE(of, focaltech_of_match);
static struct i2c_driver ft5x0x_ts_driver = {
	.probe		= ft5x0x_ts_probe,
	.remove		= ft5x0x_ts_remove,
	.id_table	= ft5x0x_ts_id,
	.driver	= {
		.name	= FOCALTECH_TS_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = focaltech_of_match,
	},
	.suspend = NULL,
	.resume = NULL,
};

static int __init ft5x0x_ts_init(void)
{
#ifdef GST_TP_COMPATIBLE
	 i2c_add_driver(&ft5x0x_ts_driver);
	    if (chip_matching_result < 0)
	    {
		    i2c_del_driver(&ft5x0x_ts_driver);
		    i2c_unregister_device(this_client);
	    }

	    return chip_matching_result;

#else
	return i2c_add_driver(&ft5x0x_ts_driver);
#endif
}

static void __exit ft5x0x_ts_exit(void)
{
	i2c_del_driver(&ft5x0x_ts_driver);
}

module_init(ft5x0x_ts_init);
module_exit(ft5x0x_ts_exit);

MODULE_AUTHOR("<wenfs@Focaltech-systems.com>");
MODULE_DESCRIPTION("FocalTech ft5x0x TouchScreen driver");
MODULE_LICENSE("GPL");
