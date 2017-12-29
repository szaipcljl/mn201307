	/*++
     *
     * Copyright (c) 2012-2022 ChipOne Technology (Beijing) Co., Ltd. All Rights Reserved.
     * This PROPRIETARY SOFTWARE is the property of ChipOne Technology (Beijing) Co., Ltd.
     * and may contains trade secrets and/or other confidential information of ChipOne
     * Technology (Beijing) Co., Ltd. This file shall not be disclosed to any third party,
     * in whole or in part, without prior written consent of ChipOne.
     * THIS PROPRIETARY SOFTWARE & ANY RELATED DOCUMENTATION ARE PROVIDED AS IS,
     * WITH ALL FAULTS, & WITHOUT WARRANTY OF ANY KIND. CHIPONE DISCLAIMS ALL EXPRESS OR
     * IMPLIED WARRANTIES.
     *
     * File Name:    icn85xx.c
     * Abstract:
     *           input driver.
     * Author:       Zhimin Tian
     * Date :        08,14,2013
     * Version:      1.0
     * History :
     * 2012,10,30, V0.1 first version
     * --*/

#include <linux/i2c/icn85xx.h>
#include "icn85xx_fw.h"
/*
 * int icn85xx_ts_int_gpio;// = 1;
 * int icn85xx_ts_rst_gpio;// = 0;
 */

#if CTP_CHARGER_DETECT
#include <linux/power_supply.h>
#endif

#if SUPPORT_CHECK_ESD
struct hrtimer             icn85xx_esd_timer;         
struct delayed_work        icn85xx_esd_event_work;
struct workqueue_struct   *icn85xx_esd_workqueue;
#endif
#define ICN_GESTURE

#ifdef ICN_GESTURE
#include <linux/suspend.h>
#include <linux/wakelock.h>
#include <linux/irq.h>
#define FTS_GESTRUE_EN 1

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
    TP_GESTURE_L=0x8000,	
};
u8 icn85xx_gesture_enable = 0;
static int ft_gesture_enable       = TP_GESTURE_OFF;
static int ft_gesture_id       = 0x00;
#define  KEY_GESTURE_U			KEY_POWER//KEY_U
#define  KEY_GESTURE_UP		KEY_SCROLLLOCK//KEY_UP
#define  KEY_GESTURE_DOWN		KEY_F11//KEY_DOWN
#define  KEY_GESTURE_LEFT		KEY_F9//KEY_LEFT 
#define  KEY_GESTURE_RIGHT		KEY_F10//KEY_RIGHT
#define  KEY_GESTURE_O			KEY_F1//KEY_O
#define  KEY_GESTURE_E			KEY_F4//KEY_E
#define  KEY_GESTURE_C			KEY_F5//KEY_C
#define  KEY_GESTURE_M			KEY_F3//KEY_M 
#define  KEY_GESTURE_L			KEY_L//KEY_L
#define  KEY_GESTURE_W			KEY_F2//KEY_W
#define  KEY_GESTURE_S			KEY_F6//KEY_S 
#define  KEY_GESTURE_V			KEY_F7//KEY_V
#define  KEY_GESTURE_Z			KEY_F8//KEY_Z
		
#define	TP_GESTURE_OFF				    0x00
#define GESTURE_UP                      0x11
#define GESTURE_C                       0x12
#define GESTURE_O                       0x13
#define GESTURE_M                       0x14
#define GESTURE_W                       0x15
#define GESTURE_E                       0x16
#define GESTURE_S                       0x17
#define GESTURE_B                       0x18
#define GESTURE_T                       0x19
#define GESTURE_H                       0x1A
#define GESTURE_F                       0x1b
#define GESTURE_X                       0x1c
#define GESTURE_Z                       0x1d
#define GESTURE_V                       0x1e
#define GESTURE_D_TAP                   0x50
#define ICN85XX_REG_PMODE				0x04

#define GESTURE_DOUBLECLICK	 			0x50
#define GESTURE_DOWN                    0x21
#define GESTURE_RIGHT                   0x31
#define GESTURE_LEFT                    0x41



static int gesture_id=0;             //Gesture ID
static int gesture_status=0;         //Gesture status

#endif

/* --------------song-------------------------------- */

#define  TPD_PROXIMITY
#ifdef TPD_PROXIMITY
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>


#define ICN85XX_PS_DEVICE	"tpd_proximity" /* "ap3212c_pls" */
#define ICN85XX_PS_INPUT_DEV	"SPRD_WW_CTP"   /* "proximity" */

static int PROXIMITY_STATE = 0;
int		icn85xx_proximity_flag	= 0;
static int	icn85xx_ps_opened	= 0;
static int	ps_state		= 0;
static int	tpd_halt		= 0;

#ifdef TPD_PROXIMITY
struct input_dev *ps_input_dev;
/* extern struct i2c_client *g_I2cClient; */
#endif

#define LTR_IOCTL_MAGIC		0x1C
#define LTR_IOCTL_GET_PFLAG	_IOR( LTR_IOCTL_MAGIC, 1, int )
#define LTR_IOCTL_GET_LFLAG	_IOR( LTR_IOCTL_MAGIC, 2, int )
#define LTR_IOCTL_SET_PFLAG	_IOW( LTR_IOCTL_MAGIC, 3, int )
#define LTR_IOCTL_SET_LFLAG	_IOW( LTR_IOCTL_MAGIC, 4, int )
#define LTR_IOCTL_GET_DATA	_IOW( LTR_IOCTL_MAGIC, 5, unsigned char )
#endif

/* --------------------------------------------------- */

#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //houdaijun160707 add
extern int irq_set_irq_type(unsigned int irq, unsigned int type);
static struct class  *tp_gesture_class;
static struct device *tp_gesture_dev;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);

// Fixme;
static DEVICE_ATTR(enable, 4775, gesture_enable_show, gesture_enable_store);
//static DEVICE_ATTR(mode, 4775, gesture_mode_show, gesture_mode_store);






static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (sprintf(buf, "ft_gesture_enable = 0x%x! ft_gesture_id=0x%x\n", ft_gesture_enable,gesture_id));
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


#if COMPILE_FW_WITH_DRIVER
static char firmware[128] = "icn85xx_firmware";
#else
#if SUPPORT_SENSOR_ID
static char firmware[128] = { 0 };
#else
/*
 * static char firmware[128] = {"/misc/modules/ICN87xx.bin"};
 * static char firmware[128] = {"/system/etc/ICN87xx.bin"};
 */
static char firmware[128] = { "/system/bin/ICN87xx.bin" };
#endif
#endif

#if SUPPORT_SENSOR_ID
char	cursensor_id, tarsensor_id, id_match;
char	invalid_id = 0;

struct sensor_id {
	char		value;
	const char	bin_name[128];
	unsigned char	*fw_name;
	int		size;
};

static struct sensor_id sensor_id_table[] = {
	{ 0x00, "/misc/modules/ICN8505_00_name1.BIN", fw_00_ht_0528, sizeof(fw_00_ht_0528) }, /* default bin or fw */
	{ 0x02, "/misc/modules/ICN8505_02_name3.BIN", fw_02_lh_0528, sizeof(fw_02_lh_0528) },

	/* if you want support other sensor id value ,please add here */
};
#endif
static struct i2c_client	*this_client;
short				log_basedata[COL_NUM][ROW_NUM] = { { 0, 0 } };
short				log_rawdata[COL_NUM][ROW_NUM] = { { 0, 0 } };
short				log_diffdata[COL_NUM][ROW_NUM] = { { 0, 0 } };
unsigned int			log_on_off = 0;
static void icn85xx_log( char diff );


static int icn85xx_testTP( char type, int para1, int para2, int para3 );


extern void setbootmode( char bmode );


static int		suspend_status = 0;
static struct wake_lock gestrue_wakelock;
static void icn85xx_clear_report_data( struct icn85xx_ts_data *icn85xx_ts );


/* song */
#define CFG_ICN_DETECT_UP_EVENT 1  /* 0 */
#if CFG_ICN_DETECT_UP_EVENT
static struct timer_list	_st_up_evnet_timer;
static unsigned int		_sui_last_point_cnt = 0;
#endif

#if VIRTUAL_KEY_VALUE
static int	pre_key			= 0;
static int	touch_key_array[]	= { KEY_BACK, KEY_MENU, KEY_HOMEPAGE };
#endif

#if CTP_CHARGER_DETECT
extern int power_supply_is_system_supplied( void );

struct power_supply	*batt_psy		= NULL;
static unsigned char	is_charger_status	= 0;
static unsigned char	pre_charger_status	= 0;

#endif

u8 g_chip_type=0  ,g_fwVersion = 0;  //add by zhouhua 

/* yank added */
void icn85xx_charge_mode( void )
{
	//printk( "yank---%s\n", __func__ );
	icn85xx_write_reg( ICN85xx_REG_PMODE, 0x55 );
}


EXPORT_SYMBOL( icn85xx_charge_mode );

void icn85xx_discharge_mode( void )
{
	//printk( "yank---%s\n", __func__ );
	icn85xx_write_reg( ICN85xx_REG_PMODE, 0x66 );
}


EXPORT_SYMBOL( icn85xx_discharge_mode );


static enum hrtimer_restart chipone_timer_func( struct hrtimer *timer );


#if SUPPORT_SYSFS
/*
 * static ssize_t icn85xx_show_update(struct device* cd,struct device_attribute *attr, char* buf);
 * static ssize_t icn85xx_store_update(struct device* cd, struct device_attribute *attr, const char* buf, size_t len);
 */
static ssize_t icn85xx_store_update( struct device_driver *drv, const char *buf, size_t count );


static ssize_t icn85xx_show_update( struct device_driver *drv, char* buf );


static ssize_t icn85xx_show_process( struct device* cd, struct device_attribute *attr, char* buf );


static ssize_t icn85xx_store_process( struct device* cd, struct device_attribute *attr, const char* buf, size_t len );


/*
 * static DEVICE_ATTR(update, S_IRUGO | S_IWUSR, icn85xx_show_update, icn85xx_store_update);
 * static DEVICE_ATTR(process, S_IRUGO | S_IWUSR, icn85xx_show_process, icn85xx_store_process);
 */
static DRIVER_ATTR( icn_update, S_IRUGO | S_IWUSR, icn85xx_show_update, icn85xx_store_update );
static DRIVER_ATTR( icn_process, S_IRUGO | S_IWUSR, icn85xx_show_process, icn85xx_store_process );  /* change by carlos_hu */

static ssize_t icn85xx_show_process( struct device* cd, struct device_attribute *attr, char* buf )
{
	ssize_t ret = 0;
	sprintf( buf, "icn85xx process\n" );
	ret = strlen( buf ) + 1;
	return(ret);
}


static ssize_t icn85xx_store_process( struct device* cd, struct device_attribute *attr,
				      const char* buf, size_t len )
{
	struct icn85xx_ts_data	*icn85xx_ts	= i2c_get_clientdata( this_client );
	unsigned long		on_off		= simple_strtoul( buf, NULL, 10 );

	log_on_off = on_off;
	memset( &log_basedata[0][0], 0, COL_NUM * ROW_NUM * 2 );
	if ( on_off == 0 )
	{
		icn85xx_ts->work_mode = 0;
	}else if ( (on_off == 1) || (on_off == 2) || (on_off == 3) )
	{
		if ( (icn85xx_ts->work_mode == 0) && (icn85xx_ts->use_irq == 1) )
		{
			hrtimer_init( &icn85xx_ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
			icn85xx_ts->timer.function = chipone_timer_func;
			hrtimer_start( &icn85xx_ts->timer, ktime_set( CTP_START_TIMER / 1000, (CTP_START_TIMER % 1000) * 1000000 ), HRTIMER_MODE_REL );
		}
		icn85xx_ts->work_mode = on_off;
	}else if ( on_off == 10 )
	{
		icn85xx_ts->work_mode = 4;
		mdelay( 10 );
		/* printk("update baseline\n"); */
		icn85xx_write_reg( 4, 0x30 );
		icn85xx_ts->work_mode = 0;
	}else  {
		icn85xx_ts->work_mode = 0;
	}


	return(len);
}


#define PARA_STATE_CMD		0X00                    /* add by carlos_hu */
#define PARA_STATE_DATA1	0X01                    /* add by carlos_hu */
#define PARA_STATE_DATA2	0X02                    /* add by carlos_hu */
#define PARA_STATE_DATA3	0X03                    /* add by carlos_hu */

#define UPDATE_CMD_NONE 0x00                            /* add by carlos_hu */

static char	update_cmd	= UPDATE_CMD_NONE;      /* add by carlos_hu */
static int	update_data1	= 0;                    /* add by carlos_hu */
static int	update_data2	= 0;                    /* add by carlos_hu */
static int	update_data3	= 0;                    /* add by carlos_hu */

static ssize_t icn85xx_show_update( struct device_driver *drv,
				    char* buf )
{
	ssize_t ret = 0;

	switch ( update_cmd )
	{
	case 'u':
	case 'U':
		sprintf( buf, firmware );
		ret = strlen( buf ) + 1;
		icn85xx_trace( "firmware: %s\n", firmware );

		break;
	case 't':
	case 'T':
		icn85xx_trace( "cmd t,T\n" );

		break;
	case 'r':
	case 'R':
		icn85xx_trace( "cmd r,R\n" );

		break;
	case 'd':
	case 'D':
		icn85xx_trace( "cmd d,D\n" );

		break;
	case 'c':
	case 'C':
		icn85xx_trace( "cmd c,C, %d, %d, %d\n", update_data1, update_data2, update_data3 );
		sprintf( buf, "%02x:%08x:%08x\n", update_data1, update_data2, update_data3 );
		ret = strlen( buf ) + 1;
		break;
	case 'e':
	case 'E':
		icn85xx_trace( "cmd e,E, %d, %d, %d\n", update_data1, update_data2, update_data3 );
		sprintf( buf, "%02x:%08x:%08x\n", update_data1, update_data2, update_data3 );
		ret = strlen( buf ) + 1;
		break;
	default:
		icn85xx_trace( "this conmand is unknow!!\n" );
		break;
	}

	return(ret);
}


/*  update cmd:
 * *u:/mnt/aaa/bbb/ccc   or U:/mnt/aaa/bbb/ccc, update firmware
 * *t or T, reset tp
 * *r or R, printf rawdata
 * *d or D, printf diffdata
 * *c:100 or C:100, Consistence test
 * *e:Vol:Max:Min or E:Vol:Max:Min, diffdata test
 *        you can get the diffdata test result, and you should send e/E cmd before this cmd
 *        result formate:   result:Max:Min=%02x:%08x:%08x
 *        when cmd e/E before this cmd and Max=Min=0, you can get Max/Min diffdata
 */

static ssize_t icn85xx_store_update( struct device_driver *drv, const char *buf, size_t count )
{
	icn85xx_trace( "count: %d, update: %s\n", count, buf );
	int			err		= 0;
	int			i		= 0, j = 0;
	char			para_state	= PARA_STATE_CMD;
	char			cmd[16]		= { 0 };
	char			data1[128]	= { 0 };
	char			data2[16]	= { 0 };
	char			data3[16]	= { 0 };
	struct icn85xx_ts_data	*icn85xx_ts	= i2c_get_clientdata( this_client );
	char			* p;
	p = cmd;
	for ( i = 0; i < count; i++ )
	{
		if ( buf[i] == ':' )
		{
			if ( PARA_STATE_CMD == para_state )
			{
				p[j]		= '\0';
				para_state	= PARA_STATE_DATA1;
				p		= data1;
				j		= 0;
			}else if ( PARA_STATE_DATA1 == para_state )
			{
				p[j]		= '\0';
				para_state	= PARA_STATE_DATA2;
				p		= data2;
				j		= 0;
			}else if ( PARA_STATE_DATA2 == para_state )
			{
				p[j]		= '\0';
				para_state	= PARA_STATE_DATA3;
				p		= data3;
				j		= 0;
			}
		}else  {
			p[j++] = buf[i];
		}
	}
	p[j] = '\0';

	update_cmd = cmd[0];
	switch ( update_cmd )
	{
	case 'u':
	case 'U':
		icn85xx_trace( "firmware: %s, %d\n", firmware, strlen( firmware ) );
		for ( i = 0; i < 40; i++ )
			//printk( "0x%2x ", firmware[i] );
		//printk( "\n" );
		memset( firmware, 0, 128 );
		memcpy( firmware, data1, strlen( data1 ) - 1 );
		icn85xx_trace( "firmware: %s, %d\n", firmware, strlen( firmware ) );
		icn85xx_trace( "fwVersion : 0x%x\n", icn85xx_read_fw_Ver( firmware ) );
		icn85xx_trace( "current version: 0x%x\n", icn85xx_readVersion() );
		if ( (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_87) || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87) )
		{
			if ( R_OK == icn87xx_fw_update( firmware ) )
			{
				//printk( "87 update ok\n" );
			}else  {
				//printk( "87 update error\n" );
			}
		}else  {
			if ( R_OK == icn85xx_fw_update( firmware ) )
			{
				//printk( "update ok\n" );
			}else  {
				//printk( "update error\n" );
			}
		}
		break;
	case 't':
	case 'T':
		icn85xx_trace( "cmd t,T\n" );
		icn85xx_ts_reset();
		break;
	case 'r':
	case 'R':
		icn85xx_trace( "cmd r,R\n" );
		icn85xx_log( 0 );
		break;
	case 'd':
	case 'D':
		icn85xx_trace( "cmd d,D\n" );
		icn85xx_log( 1 );
		break;
	case 'c':
	case 'C':
		update_data1 = simple_strtoul( data1, NULL, 10 );
		icn85xx_trace( "cmd c,C, %d\n", update_data1 );
		update_data1	= icn85xx_testTP( 0, update_data1, 0, 0 );
		update_data2	= 0;
		update_data3	= 0;
		icn85xx_trace( "cmd c,C, result: %d\n", update_data1 );
		break;
	case 'e':
	case 'E':
		update_data1	= simple_strtoul( data1, NULL, 10 );
		update_data2	= simple_strtoul( data2, NULL, 10 );
		update_data3	= simple_strtoul( data3, NULL, 10 );
		icn85xx_trace( "cmd e,E, %d, %d, %d, %d\n", update_data1, update_data2, update_data3 );
		update_data1 = icn85xx_testTP( 1, update_data1, update_data2, update_data3 );
		break;
	default:
		//printk( "this conmand is unknow!!\n" );
		break;
	}

	return(count);
}


static int icn85xx_create_sysfs( struct i2c_client *client )
{
	int		err;
	struct device	*dev = &(client->dev);
	icn85xx_trace( "%s: \n", __func__ );
	/*
	 *  err = device_create_file(dev, &dev_attr_update);
	 * err = device_create_file(dev, &dev_attr_process);
	 */
	return(err);
}


static void icn85xx_remove_sysfs( struct i2c_client *client )
{
	struct device *dev = &(client->dev);
	icn85xx_trace( "%s: \n", __func__ );
	/*
	 * device_remove_file(dev, &dev_attr_update);
	 * device_remove_file(dev, &dev_attr_process);
	 */
}


static struct attribute			*icn_drv_attr[] = {
	&driver_attr_icn_update.attr,
	&driver_attr_icn_process.attr,
	NULL
};
static struct attribute_group		icn_drv_attr_grp = {
	.attrs	= icn_drv_attr,
};
static const struct attribute_group	*icn_drv_grp[] = {
	&icn_drv_attr_grp,
	NULL
};
#endif

#if SUPPORT_PROC_FS

pack_head			cmd_head;
static struct proc_dir_entry	*icn85xx_proc_entry;
int				DATA_LENGTH = 0;
GESTURE_DATA			structGestureData;
STRUCT_PANEL_PARA_H		g_structPanelPara;

static ssize_t icn85xx_tool_write( struct file *file, const char __user * buffer, size_t count, loff_t * ppos )
{
	int			ret = 0;
	int			i;
	unsigned short		addr;
	unsigned int		prog_addr;
	char			retvalue;
	struct task_struct	*thread = NULL;

	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
	proc_info( "%s \n", __func__ );
	if ( down_interruptible( &icn85xx_ts->sem ) )
	{
		return(-1);
	}
	ret = copy_from_user( &cmd_head, buffer, CMD_HEAD_LENGTH );
	if ( ret )
	{
		proc_error( "copy_from_user failed.\n" );
		goto write_out;
	}else  {
		ret = CMD_HEAD_LENGTH;
	}

	proc_info( "wr  :0x%02x.\n", cmd_head.wr );
	proc_info( "flag:0x%02x.\n", cmd_head.flag );
	proc_info( "circle  :%d.\n", (int) cmd_head.circle );
	proc_info( "times   :%d.\n", (int) cmd_head.times );
	proc_info( "retry   :%d.\n", (int) cmd_head.retry );
	proc_info( "data len:%d.\n", (int) cmd_head.data_len );
	proc_info( "addr len:%d.\n", (int) cmd_head.addr_len );
	proc_info( "addr:0x%02x%02x.\n", cmd_head.addr[0], cmd_head.addr[1] );
	proc_info( "len:%d.\n", (int) count );
	proc_info( "data:0x%02x%02x.\n", buffer[CMD_HEAD_LENGTH], buffer[CMD_HEAD_LENGTH + 1] );
	if ( 1 == cmd_head.wr ) /* write para */
	{
		proc_info( "cmd_head_.wr == 1  \n" );
		ret = copy_from_user( &cmd_head.data[0], &buffer[CMD_HEAD_LENGTH], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_from_user failed.\n" );
			goto write_out;
		}
		/* need copy to g_structPanelPara */

		memcpy( &g_structPanelPara, &cmd_head.data[0], cmd_head.data_len );
		/* write para to tp */
		for ( i = 0; i < cmd_head.data_len; )
		{
			int size = ( (i + 64) > cmd_head.data_len) ? (cmd_head.data_len - i) : 64;
			ret = icn85xx_i2c_txdata( 0x8000 + i, &cmd_head.data[i], size );
			if ( ret < 0 )
			{
				proc_error( "write para failed!\n" );
				goto write_out;
			}
			i = i + 64;
		}

		ret			= cmd_head.data_len + CMD_HEAD_LENGTH;
		icn85xx_ts->work_mode	= 5; /* reinit */
		//printk( "reinit tp\n" );
		icn85xx_write_reg( 0, 1 );
		mdelay( 100 );
		icn85xx_write_reg( 0, 0 );
		mdelay( 100 );
		icn85xx_ts->work_mode = 0;
		goto write_out;
	}else if ( 3 == cmd_head.wr ) /* set update file */
	{
		proc_info( "cmd_head_.wr == 3  \n" );
		ret = copy_from_user( &cmd_head.data[0], &buffer[CMD_HEAD_LENGTH], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_from_user failed.\n" );
			goto write_out;
		}
		ret = cmd_head.data_len + CMD_HEAD_LENGTH;
		memset( firmware, 0, 128 );
		memcpy( firmware, &cmd_head.data[0], cmd_head.data_len );
		proc_info( "firmware : %s\n", firmware );
	}else if ( 5 == cmd_head.wr ) /* start update */
	{
		proc_info( "cmd_head_.wr == 5 \n" );
		icn85xx_update_status( 1 );
		/* ret = kernel_thread(icn85xx_fw_update,firmware,CLONE_KERNEL); */
		thread = kthread_run( icn85xx_fw_update, firmware, "icn_update" );
		/* thread = kthread_run(gup_update_proc, "update", "fl update"); */
		if ( IS_ERR( thread ) )
		{
			ret = PTR_ERR( thread );
			//printk( "failed to create kernel thread: %d\n", ret );
		}

		icn85xx_trace( "the kernel_thread result is:%d\n", ret );
	}else if ( 11 == cmd_head.wr )                                  /* write hostcomm */
	{
		icn85xx_ts->work_mode		= cmd_head.flag;        /* for gesture test,you should set flag=6 */
		structGestureData.u8Status	= 0;

		ret = copy_from_user( &cmd_head.data[0], &buffer[CMD_HEAD_LENGTH], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_from_user failed.\n" );
			goto write_out;
		}
		addr = (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		icn85xx_write_reg( addr, cmd_head.data[0] );
	}else if ( 13 == cmd_head.wr ) /* adc enable */
	{
		proc_info( "cmd_head_.wr == 13  \n" );
		icn85xx_ts->work_mode = 4;
		mdelay( 10 );
		/* set col */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8ColNum ), 1 );
		/* u8RXOrder[0] = u8RXOrder[cmd_head.addr[0]]; */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8RXOrder[0] ), g_structPanelPara.u8RXOrder[cmd_head.addr[0]] );
		/* set row */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8RowNum ), 1 );
		/* u8TXOrder[0] = u8TXOrder[cmd_head.addr[1]]; */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8TXOrder[0] ), g_structPanelPara.u8TXOrder[cmd_head.addr[1]] );
		/* scan mode */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8ScanMode ), 0 );
		/* bit */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u16BitFreq ), 0xD0 );
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u16BitFreq ) + 1, 0x07 );
		/* freq */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u16FreqCycleNum[0] ), 0x64 );
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u16FreqCycleNum[0] ) + 1, 0x00 );
		/* pga */
		icn85xx_write_reg( 0x8000 + STRUCT_OFFSET( STRUCT_PANEL_PARA_H, u8PgaGain ), 0x0 );

		/* config mode */
		icn85xx_write_reg( 0, 0x2 );

		mdelay( 1 );
		icn85xx_read_reg( 2, &retvalue );
		//printk( "retvalue0: %d\n", retvalue );
		while ( retvalue != 1 )
		{
			//printk( "retvalue: %d\n", retvalue );
			mdelay( 1 );
			icn85xx_read_reg( 2, &retvalue );
		}

		if ( icn85xx_goto_progmode() != 0 )
		{
			//printk( "icn85xx_goto_progmode() != 0 error\n" );
			goto write_out;
		}

		icn85xx_prog_write_reg( 0x040870, 1 );
	}else if ( 15 == cmd_head.wr ) /* write hostcomm multibyte */
	{
		proc_info( "cmd_head_.wr == 15  \n" );
		ret = copy_from_user( &cmd_head.data[0], &buffer[CMD_HEAD_LENGTH], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_from_user failed.\n" );
			goto write_out;
		}
		addr = (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		//printk( "wr, addr: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", addr, cmd_head.data[0], cmd_head.data[1], cmd_head.data[2], cmd_head.data[3] );
		ret = icn85xx_i2c_txdata( addr, &cmd_head.data[0], cmd_head.data_len );
		if ( ret < 0 )
		{
			proc_error( "write hostcomm multibyte failed!\n" );
			goto write_out;
		}
	}else if ( 17 == cmd_head.wr ) /* write iic porgmode multibyte */
	{
		proc_info( "cmd_head_.wr == 17  \n" );
		ret = copy_from_user( &cmd_head.data[0], &buffer[CMD_HEAD_LENGTH], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_from_user failed.\n" );
			goto write_out;
		}
		prog_addr = (cmd_head.flag << 16) | (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		icn85xx_goto_progmode();
		ret = icn85xx_prog_i2c_txdata( prog_addr, &cmd_head.data[0], cmd_head.data_len );
		if ( ret < 0 )
		{
			proc_error( "write hostcomm multibyte failed!\n" );
			goto write_out;
		}
	}

write_out:
	up( &icn85xx_ts->sem );
	proc_info( "icn85xx_tool_write write_out  \n" );
	return(count);
}


static ssize_t icn85xx_tool_read( struct file *file, char __user * buffer, size_t count, loff_t * ppos )
{
	int			i, j;
	int			ret = 0;
	char			row, column, retvalue, max_column;
	unsigned short		addr;
	unsigned int		prog_addr;
	struct icn85xx_ts_data	*icn85xx_ts = i2c_get_clientdata( this_client );
	if ( down_interruptible( &icn85xx_ts->sem ) )
	{
		return(-1);
	}
	proc_info( "%s: count:%d, off:%d, cmd_head.data_len: %d\n", __func__, count, (int) (*ppos), (int) cmd_head.data_len );
	if ( cmd_head.wr % 2 )
	{
		ret = 0;
		proc_info( "cmd_head_.wr == 1111111  \n" );
		goto read_out;
	}else if ( 0 == cmd_head.wr ) /* read para */
	{
		/* read para */
		proc_info( "cmd_head_.wr == 0  \n" );
		ret = icn85xx_i2c_rxdata( 0x8000, &g_structPanelPara, cmd_head.data_len );
		if ( ret < 0 )
		{
			icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
		}
		ret = copy_to_user( buffer, (void *) (&g_structPanelPara), cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}

		goto read_out;
	}else if ( 2 == cmd_head.wr ) /* get update status */
	{
		proc_info( "cmd_head_.wr == 2  \n" );
		retvalue = icn85xx_get_status();
		proc_info( "status: %d\n", retvalue );
		ret = copy_to_user( buffer, (void *) (&retvalue), 1 );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}
	}else if ( 4 == cmd_head.wr ) /* read rawdata */
	{
		/*
		 * icn85xx_read_reg(0x8004, &row);
		 * icn85xx_read_reg(0x8005, &column);
		 */
		proc_info( "cmd_head_.wr == 4  \n" );
		row		= cmd_head.addr[1];
		column		= cmd_head.addr[0];
		max_column	= (cmd_head.flag == 0) ? (24) : (cmd_head.flag);
		/* scan tp rawdata */
		icn85xx_write_reg( 4, 0x20 );
		mdelay( 1 );
		for ( i = 0; i < 1000; i++ )
		{
			mdelay( 1 );
			icn85xx_read_reg( 2, &retvalue );
			if ( retvalue == 1 )
				break;
		}

		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x2000 + i * (max_column) * 2, (char *) &log_rawdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
			ret = copy_to_user( &buffer[column * 2 * i], (void *) (&log_rawdata[i][0]), column * 2 );
			if ( ret )
			{
				proc_error( "copy_to_user failed.\n" );
				goto read_out;
			}
		}

		/* finish scan tp rawdata */
		icn85xx_write_reg( 2, 0x0 );
		icn85xx_write_reg( 4, 0x21 );
		goto read_out;
	}else if ( 6 == cmd_head.wr ) /* read diffdata */
	{
		proc_info( "cmd_head_.wr == 6   \n" );
		row		= cmd_head.addr[1];
		column		= cmd_head.addr[0];
		max_column	= (cmd_head.flag == 0) ? (24) : (cmd_head.flag);
		/* scan tp rawdata */
		icn85xx_write_reg( 4, 0x20 );
		mdelay( 1 );

		for ( i = 0; i < 1000; i++ )
		{
			mdelay( 1 );
			icn85xx_read_reg( 2, &retvalue );
			if ( retvalue == 1 )
				break;
		}

		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x3000 + (i + 1) * (max_column + 2) * 2 + 2, (char *) &log_diffdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
			ret = copy_to_user( &buffer[column * 2 * i], (void *) (&log_diffdata[i][0]), column * 2 );
			if ( ret )
			{
				proc_error( "copy_to_user failed.\n" );
				goto read_out;
			}
		}
		/* finish scan tp rawdata */
		icn85xx_write_reg( 2, 0x0 );
		icn85xx_write_reg( 4, 0x21 );

		goto read_out;
	}else if ( 8 == cmd_head.wr ) /* change TxVol, read diff */
	{
		proc_info( "cmd_head_.wr == 8  \n" );
		row		= cmd_head.addr[1];
		column		= cmd_head.addr[0];
		max_column	= (cmd_head.flag == 0) ? (24) : (cmd_head.flag);
		/* scan tp rawdata */
		icn85xx_write_reg( 4, 0x20 );
		mdelay( 1 );
		for ( i = 0; i < 1000; i++ )
		{
			mdelay( 1 );
			icn85xx_read_reg( 2, &retvalue );
			if ( retvalue == 1 )
				break;
		}

		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x2000 + i * (max_column) * 2, (char *) &log_rawdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
		}

		/* finish scan tp rawdata */
		icn85xx_write_reg( 2, 0x0 );
		icn85xx_write_reg( 4, 0x21 );

		icn85xx_write_reg( 4, 0x12 );

		/* scan tp rawdata */
		icn85xx_write_reg( 4, 0x20 );
		mdelay( 1 );
		for ( i = 0; i < 1000; i++ )
		{
			mdelay( 1 );
			icn85xx_read_reg( 2, &retvalue );
			if ( retvalue == 1 )
				break;
		}

		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x2000 + i * (max_column) * 2, (char *) &log_diffdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}

			for ( j = 0; j < column; j++ )
			{
				log_basedata[i][j] = log_rawdata[i][j] - log_diffdata[i][j];
			}
			ret = copy_to_user( &buffer[column * 2 * i], (void *) (&log_basedata[i][0]), column * 2 );
			if ( ret )
			{
				proc_error( "copy_to_user failed.\n" );
				goto read_out;
			}
		}

		/* finish scan tp rawdata */
		icn85xx_write_reg( 2, 0x0 );
		icn85xx_write_reg( 4, 0x21 );

		icn85xx_write_reg( 4, 0x10 );

		goto read_out;
	}else if ( 10 == cmd_head.wr ) /* read adc data */
	{
		proc_info( "cmd_head_.wr == 10  \n" );
		if ( cmd_head.flag == 0 )
		{
			icn85xx_prog_write_reg( 0x040874, 0 );
		}
		icn85xx_prog_read_page( 2500 * cmd_head.flag, (char *) &log_diffdata[0][0], cmd_head.data_len );
		ret = copy_to_user( buffer, (void *) (&log_diffdata[0][0]), cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}

		if ( cmd_head.flag == 9 )
		{
			/* reload code */
			if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86) )
			{
				if ( R_OK == icn85xx_fw_update( firmware ) )
				{
					icn85xx_ts->code_loaded_flag = 1;
					icn85xx_trace( "ICN85XX_WITHOUT_FLASH, reload code ok\n" );
				}else  {
					icn85xx_ts->code_loaded_flag = 0;
					icn85xx_trace( "ICN85XX_WITHOUT_FLASH, reload code error\n" );
				}
			}else  {
				icn85xx_bootfrom_flash( icn85xx_ts->ictype );
				msleep( 150 );


/*
 *              memset(&cmd_head.data[0], 0, 3);
 *              ret = icn85xx_i2c_rxdata(0xa,&cmd_head.data[0],1);
 *              if(ret < 0)
 *              {
 *                  proc_error("normal read id failed.\n");
 *              }
 *              proc_info("nomal mode id: 0x%x\n", cmd_head.data[0]);
 *
 *              icn85xx_prog_i2c_rxdata(0x40000,&cmd_head.data[0],3);
 *              if(ret < 0)
 *              {
 *                  proc_error("prog mode read id failed.\n");
 *              }
 *              proc_info("prog mode id: 0x%x 0x%x\n", cmd_head.data[1], cmd_head.data[2]);
 */
			}
			icn85xx_ts->work_mode = 0;
		}
	}else if ( 12 == cmd_head.wr ) /* read hostcomm */
	{
		proc_info( "cmd_head_.wr == 12  \n" );
		addr = (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		icn85xx_read_reg( addr, &retvalue );
		ret = copy_to_user( buffer, (void *) (&retvalue), 1 );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}
	}else if ( 14 == cmd_head.wr ) /* read adc status */
	{
		proc_info( "cmd_head_.wr == 14  \n" );
		icn85xx_prog_read_reg( 0x4085E, &retvalue );
		ret = copy_to_user( buffer, (void *) (&retvalue), 1 );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}
		//printk( "0x4085E: 0x%x\n", retvalue );
	}else if ( 16 == cmd_head.wr ) /* read gesture data */
	{
		proc_info( "cmd_head_.wr == 16  \n" );
		ret = copy_to_user( buffer, (void *) (&structGestureData), sizeof(structGestureData) );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}

		if ( structGestureData.u8Status == 1 )
			structGestureData.u8Status = 0;
	}else if ( 18 == cmd_head.wr ) /* read hostcomm multibyte */
	{
		proc_info( "cmd_head_.wr == 18  \n" );
		addr	= (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		ret	= icn85xx_i2c_rxdata( addr, &cmd_head.data[0], cmd_head.data_len );
		if ( ret < 0 )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}
		ret = copy_to_user( buffer, &cmd_head.data[0], cmd_head.data_len );
		if ( ret )
		{
			icn85xx_error( "read hostcomm multibyte failed: %d\n", ret );
		}
		//printk( "rd, addr: 0x%x, data_len: %d, data: 0x%x\n", addr, cmd_head.data_len, cmd_head.data[0] );
		goto read_out;
	}else if ( 20 == cmd_head.wr ) /* read iic porgmode multibyte */
	{
		proc_info( "cmd_head_.wr == 20  \n" );
		prog_addr = (cmd_head.flag << 16) | (cmd_head.addr[1] << 8) | cmd_head.addr[0];
		icn85xx_goto_progmode();

		ret = icn85xx_prog_i2c_rxdata( prog_addr, &cmd_head.data[0], cmd_head.data_len );
		if ( ret < 0 )
		{
			icn85xx_error( "read iic porgmode multibyte failed: %d\n", ret );
		}
		ret = copy_to_user( buffer, &cmd_head.data[0], cmd_head.data_len );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}


		icn85xx_bootfrom_sram();
		goto read_out;
	}else if ( 22 == cmd_head.wr ) /* read ictype */
	{
		proc_info( "cmd_head_.wr == 22  \n" );
		ret = copy_to_user( buffer, (void *) (&icn85xx_ts->ictype), 1 );
		if ( ret )
		{
			proc_error( "copy_to_user failed.\n" );
			goto read_out;
		}
	}
read_out:
	up( &icn85xx_ts->sem );
	proc_info( "%s out: %d, cmd_head.data_len: %d\n\n", __func__, count, cmd_head.data_len );
	return(cmd_head.data_len);
}


static const struct file_operations icn85xx_proc_fops = {
	.owner	= THIS_MODULE,
	.read	= icn85xx_tool_read,
	.write	= icn85xx_tool_write,
};
void init_proc_node( void )
{
	int i;
	memset( &cmd_head, 0, sizeof(cmd_head) );
	cmd_head.data = NULL;

	i = 5;
	while ( (!cmd_head.data) && i )
	{
		cmd_head.data = kzalloc( i * DATA_LENGTH_UINT, GFP_KERNEL );
		if ( NULL != cmd_head.data )
		{
			break;
		}
		i--;
	}
	if ( i )
	{
		/* DATA_LENGTH = i * DATA_LENGTH_UINT + GTP_ADDR_LENGTH; */
		DATA_LENGTH = i * DATA_LENGTH_UINT;
		icn85xx_trace( "alloc memory size:%d.\n", DATA_LENGTH );
	}else  {
		proc_error( "alloc for memory failed.\n" );
		return;
	}

	icn85xx_proc_entry = proc_create( ICN85XX_ENTRY_NAME, 0666, NULL, &icn85xx_proc_fops );
	if ( icn85xx_proc_entry == NULL )
	{
		proc_error( "Couldn't create proc entry!\n" );
		return;
	}else  {
		icn85xx_trace( "Create proc entry success!\n" );
	}

	return;
}


void uninit_proc_node( void )
{
	kfree( cmd_head.data );
	cmd_head.data = NULL;
	remove_proc_entry( ICN85XX_ENTRY_NAME, NULL );
}


#endif



#if TOUCH_VIRTUAL_KEYS
static ssize_t virtual_keys_show( struct kobject *kobj, struct kobj_attribute *attr, char *buf )
{
	return(sprintf( buf,
			__stringify( EV_KEY ) ":" __stringify( KEY_MENU ) ":60:870:100:110"
			":" __stringify( EV_KEY ) ":" __stringify( KEY_HOMEPAGE ) ":245:870:130:110"
			":" __stringify( EV_KEY ) ":" __stringify( KEY_BACK ) ":420:870:100:110"
			"\n" ) );
}


/*     ":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":900:890:20:20" */
static struct kobj_attribute virtual_keys_attr = {
	.attr		= {
		.name	= "virtualkeys.icn85xx",
		.mode	= S_IRUGO,
	},
	.show		= &virtual_keys_show,
};


static void icn85xx_ts_virtual_keys_init( void )
{
	int		ret;
	struct kobject	*properties_kobj;
	properties_kobj = kobject_create_and_add( "board_properties", NULL );
	if ( properties_kobj )
		ret = sysfs_create_group( properties_kobj,
					  &properties_attr_group );
	if ( !properties_kobj || ret )
		pr_err( "failed to create board_properties\n" );
}

#endif





#ifdef TPD_PROXIMITY
/************************  song ***********************************************************************/

static int TP_face_get_mode(void)
{
    //PRINT_DBG(">>>> PROXIMITY_SWITCH is %d <<<<\n", icn85xx_proximity_flag);

    return icn85xx_proximity_flag;
}

static int TP_face_mode_state(void)
{
    //PRINT_DBG(">>>> PROXIMITY_STATE  is %d <<<<\n", ps_state);

    return ps_state;
}



static int TP_face_mode_switch( int on ) /* /enable  ctp sensor */
{
	unsigned char	cmd_ps_data;
	int		len;

	//printk( "TP_face_mode_switch on=%d", on );


	if ( 1 == on )
	{
		cmd_ps_data		= 0xa0;
		icn85xx_proximity_flag	= 1;
	}else  {
		cmd_ps_data		= 0xa1;
		icn85xx_proximity_flag	= 0;
	}

	len = icn85xx_write_reg( ICN85xx_REG_PMODE, cmd_ps_data );
	//if ( 0 > len )
		//printk( "[ICN85xx] ps enable ( %d ) faild\n", on );
	//else
		//printk( "[ICN85xx] pls ctp ps %s:  enable = ( %d ), sucess.\n", __func__, on );

	return(0);
}


static ssize_t tp_face_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) //  20150229  add by zhouhua
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

static int icn85xx_ps_open( struct inode *inode, struct file *file )
{
	//printk( "%s\n", __func__ );
	if ( icn85xx_ps_opened )
		return(-EBUSY);
	icn85xx_ps_opened = 1;
	return(0);
}


static int icn85xx_ps_release( struct inode *inode, struct file *file )
{
	//printk( "%s", __func__ );
	icn85xx_ps_opened = 0;
	return(TP_face_mode_switch( 0 ) );
}


static int icn85xx_ps_ioctl( struct file *file, unsigned int cmd, unsigned long arg )
{
	void __user	*argp = (void __user *) arg;
	int		flag;
	/* unsigned char data; */
	//printk( "%s: cmd %d", __func__, _IOC_NR( cmd ) );

	/* get ioctl parameter */
	switch ( cmd )
	{
	case LTR_IOCTL_SET_PFLAG:
	case LTR_IOCTL_SET_LFLAG:
		if ( copy_from_user( &flag, argp, sizeof(flag) ) )
		{
			return(-EFAULT);
		}
		if ( flag < 0 || flag > 1 )
		{
			return(-EINVAL);
		}
		//printk( "%s: set flag=%d", __func__, flag );
		break;
	default:
		break;
	}

	/* handle ioctl */
	switch ( cmd )
	{
	case LTR_IOCTL_GET_PFLAG:
		/* flag = atomic_read(&p_flag); */
		break;

	case LTR_IOCTL_GET_LFLAG:
		/* flag = atomic_read(&l_flag); */
		break;

	case LTR_IOCTL_GET_DATA:
		break;

	case LTR_IOCTL_SET_PFLAG:
		//printk( "LTR_IOCTL_SET_PFLAG" );
		/* atomic_set(&p_flag, flag); */
		if ( flag == 1 )
		{
			icn85xx_proximity_flag = 1;
			TP_face_mode_switch( 1 );
			//printk( "phc TP_face_mode_switch 1\n" );
		}else if ( flag == 0 )
		{
			icn85xx_proximity_flag = 0;
			TP_face_mode_switch( 0 );
			//printk( "phc TP_face_mode_switch 0\n" );
		}
		break;

	case LTR_IOCTL_SET_LFLAG:

		break;

	default:
		pr_err( "%s: invalid cmd %d\n", __func__, _IOC_NR( cmd ) );
		return(-EINVAL);
	}

	/* report ioctl */
	switch ( cmd )
	{
	case LTR_IOCTL_GET_PFLAG:
	case LTR_IOCTL_GET_LFLAG:

		break;

	case LTR_IOCTL_GET_DATA:

		break;

	default:
		break;
	}

	return(0);
}


static struct file_operations	icn85xx_ps_fops = {
	.owner		= THIS_MODULE,
	.open		= icn85xx_ps_open,
	.release	= icn85xx_ps_release,
	.unlocked_ioctl = icn85xx_ps_ioctl,
};
static struct miscdevice	icn85xx_ps_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= ICN85XX_PS_DEVICE,
	.fops	= &icn85xx_ps_fops,
};



static ssize_t tp_information_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) //add by zhouhua
{
	
	 
	u8 *p_chip_name="unknown!!";
	
	
	
		if(g_chip_type==0x11  ||  g_chip_type==0x22 || g_chip_type==0x33 || g_chip_type==0x44|| g_chip_type==0x55 || g_chip_type==0x66 )
		{
			p_chip_name = "ICN85xx";
			
		}
	
	
	return sprintf(buf, "IC: %s ID:0x%x\nFirmware version: 0x%.4X.\n",p_chip_name, g_chip_type,g_fwVersion);
}

static struct kobj_attribute tp_information = {
    .attr = {
        .name = "tp_information",
        .mode = S_IRUGO,
    },
    .show  = &tp_information_show,
    .store = NULL,
};








#ifdef TPD_PROXIMITY     //20150229 add by zhouhua
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
	//&virtual_keys_attr.attr,
       &tp_information.attr,
#ifdef TPD_PROXIMITY // Add facemode attribute file for proximity;
    &tp_face_mode_attr.attr,
#endif
	NULL
};

static struct attribute_group properties_attr_group = {
	.attrs	= properties_attrs,
};



static void Sysfs_icn85xx_facemode_init( void )
{
	int		ret;
	struct kobject	*properties_kobj;
	properties_kobj = kobject_create_and_add( "board_properties", NULL );
	if ( properties_kobj )
		ret = sysfs_create_group( properties_kobj,
					  &properties_attr_group );
	if ( !properties_kobj || ret )
		pr_err( "failed to create board_properties\n" );
}





/* ---------------------------------------------------------------------
*
*   Chipone panel related driver
*
*
*  ----------------------------------------------------------------------*/


/***********************************************************************************************
*  Name    :   icn85xx_ts_wakeup
*  Input   :   void
*  Output  :   ret
*  function    : this function is used to wakeup tp
***********************************************************************************************/
void icn85xx_ts_wakeup( void )
{
}


/***********************************************************************************************
*  Name    :   icn85xx_ts_reset
*  Input   :   void
*  Output  :   ret
*  function    : this function is used to reset tp, you should not delete it
***********************************************************************************************/
void icn85xx_ts_reset( void )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
	/* set reset func */
/*    printk("%s, line = %d\n", __func__, __LINE__); */
	icn85xx_trace( "%s\n", __func__ );
#if 0   /* no rst gpio */

#else
	if ( gpio_is_valid( icn85xx_ts->rst_gpio ) )
	{
		//printk( "%s, %d\n", __func__, __LINE__ );
		gpio_direction_output( icn85xx_ts->rst_gpio, 1 );
		mdelay( 40 );
		gpio_direction_output( icn85xx_ts->rst_gpio, 0 );
		mdelay( 80 );
		gpio_direction_output( icn85xx_ts->rst_gpio, 1 );
		mdelay( 40 );
	}
#endif
}


void icn85xx_set_prog_addr( void )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
#if 0
	icn85xx_ts_reset();
#else
	if ( gpio_is_valid( icn85xx_ts->irq_gpio ) )
	{
		gpio_direction_output( icn85xx_ts->irq_gpio, 0 );
		//printk( "%s, %d\n", __func__, __LINE__ );
		mdelay( 30 );
	}

	icn85xx_ts_reset();
	/*
	 * gpio_direction_output(icn85xx_ts->irq_gpio,1);
	 * mdelay(30);
	 */
	gpio_direction_input( icn85xx_ts->irq_gpio );
#endif
}


/***********************************************************************************************
*  Name    :   icn85xx_irq_disable
*  Input   :   void
*  Output  :   ret
*  function    : this function is used to disable irq
***********************************************************************************************/
void icn85xx_irq_disable( void )
{
	unsigned long		irqflags;
	struct icn85xx_ts_data	*icn85xx_ts = i2c_get_clientdata( this_client );

	spin_lock_irqsave( &icn85xx_ts->irq_lock, irqflags );
	if ( !icn85xx_ts->irq_is_disable )
	{
		icn85xx_ts->irq_is_disable = 1;
		disable_irq_nosync( icn85xx_ts->irq );
		/* disable_irq(icn85xx_ts->irq); */
	}
	spin_unlock_irqrestore( &icn85xx_ts->irq_lock, irqflags );
}


/***********************************************************************************************
*  Name    :   icn85xx_irq_enable
*  Input   :   void
*  Output  :   ret
*  function    : this function is used to enable irq
***********************************************************************************************/
void icn85xx_irq_enable( void )
{
	unsigned long		irqflags	= 0;
	struct icn85xx_ts_data	*icn85xx_ts	= i2c_get_clientdata( this_client );

	spin_lock_irqsave( &icn85xx_ts->irq_lock, irqflags );
	if ( icn85xx_ts->irq_is_disable )
	{
		enable_irq( icn85xx_ts->irq );
		icn85xx_ts->irq_is_disable = 0;
	}
	spin_unlock_irqrestore( &icn85xx_ts->irq_lock, irqflags );
}


/***********************************************************************************************
*  Name    :   icn85xx_prog_i2c_rxdata
*  Input   :   addr
* *rxdata
*           length
*  Output  :   ret
*  function    : read data from icn85xx, prog mode
***********************************************************************************************/
int icn85xx_prog_i2c_rxdata( unsigned int addr, char *rxdata, int length )
{
	int	ret	= -1;
	int	retries = 0;
#if 0
	struct i2c_msg msgs[] = {
		{
			.addr	= ICN85XX_PROG_IIC_ADDR, /* this_client->addr, */
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
#if SUPPORT_ROCKCHIP
			.scl_rate = ICN85XX_I2C_SCL,
#endif
		},
	};

	icn85xx_prog_i2c_txdata( addr, NULL, 0 );
	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msgs, 1 );
		if ( ret == 1 )
			break;
		retries++;
	}
	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#else
	unsigned char	tmp_buf[3];
	struct i2c_msg	msgs[] = {
		{
			.addr	= ICN85XX_PROG_IIC_ADDR,        /* this_client->addr, */
			.flags	= 0,
			.len	= 3,
			.buf	= tmp_buf,
		},
		{
			.addr	= ICN85XX_PROG_IIC_ADDR,        /* this_client->addr, */
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
		},
	};

	tmp_buf[0]	= (unsigned char) (addr >> 16);
	tmp_buf[1]	= (unsigned char) (addr >> 8);
	tmp_buf[2]	= (unsigned char) (addr);


	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msgs, 2 );
		if ( ret == 2 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#endif
	return(ret);
}


int icn87xx_prog_i2c_rxdata( unsigned int addr, char *rxdata, int length )
{
	int	ret	= -1;
	int	retries = 0;
#if 0
	struct i2c_msg msgs[] = {
		{
			.addr	= ICN85XX_PROG_IIC_ADDR, /* this_client->addr, */
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
#if SUPPORT_ROCKCHIP
			.scl_rate = ICN85XX_I2C_SCL,
#endif
		},
	};

	icn85xx_prog_i2c_txdata( addr, NULL, 0 );
	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msgs, 1 );
		if ( ret == 1 )
			break;
		retries++;
	}
	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#else
	unsigned char	tmp_buf[2];
	struct i2c_msg	msgs[] = {
		{
			.addr	= ICN87XX_PROG_IIC_ADDR,        /* this_client->addr, */
			.flags	= 0,
			.len	= 2,
			.buf	= tmp_buf,
		},
		{
			.addr	= ICN87XX_PROG_IIC_ADDR,        /* this_client->addr, */
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
		},
	};

	tmp_buf[0]	= (unsigned char) (addr >> 8);
	tmp_buf[1]	= (unsigned char) (addr);


	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msgs, 2 );
		if ( ret == 2 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#endif
	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_prog_i2c_txdata
*  Input   :   addr
* *rxdata
*           length
*  Output  :   ret
*  function    : send data to icn85xx , prog mode
***********************************************************************************************/
int icn85xx_prog_i2c_txdata( unsigned int addr, char *txdata, int length )
{
	int		ret = -1;
	char		tmp_buf[128];
	int		retries = 0;
	struct i2c_msg	msg[]	= {
		{
			.addr	= ICN85XX_PROG_IIC_ADDR, /* this_client->addr, */
			.flags	= 0,
			.len	= length + 3,
			.buf	= tmp_buf,
		},
	};

	if ( length > 125 )
	{
		icn85xx_error( "%s too big datalen = %d!\n", __func__, length );
		return(-1);
	}

	tmp_buf[0]	= (unsigned char) (addr >> 16);
	tmp_buf[1]	= (unsigned char) (addr >> 8);
	tmp_buf[2]	= (unsigned char) (addr);


	if ( length != 0 && txdata != NULL )
	{
		memcpy( &tmp_buf[3], txdata, length );
	}

	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msg, 1 );
		if ( ret == 1 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c write error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
	return(ret);
}


int icn87xx_prog_i2c_txdata( unsigned int addr, char *txdata, int length )
{
	int		ret = -1;
	char		tmp_buf[128];
	int		retries = 0;
	struct i2c_msg	msg[]	= {
		{
			.addr	= ICN85XX_PROG_IIC_ADDR, /* this_client->addr, */
			.flags	= 0,
			.len	= length + 2,
			.buf	= tmp_buf,
		},
	};

	if ( length > 125 )
	{
		icn85xx_error( "%s too big datalen = %d!\n", __func__, length );
		return(-1);
	}

	tmp_buf[0]	= (unsigned char) (addr >> 8);
	tmp_buf[1]	= (unsigned char) (addr);


	if ( length != 0 && txdata != NULL )
	{
		memcpy( &tmp_buf[2], txdata, length );
	}

	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msg, 1 );
		if ( ret == 1 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c write error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_prog_write_reg
*  Input   :   addr -- address
*           para -- parameter
*  Output  :
*  function    :   write register of icn85xx, prog mode
***********************************************************************************************/
int icn85xx_prog_write_reg( unsigned int addr, char para )
{
	char	buf[3];
	int	ret = -1;

	buf[0]	= para;
	ret	= icn85xx_prog_i2c_txdata( addr, buf, 1 );
	if ( ret < 0 )
	{
		icn85xx_error( "%s write reg failed! %#x ret: %d\n", __func__, buf[0], ret );
		return(-1);
	}

	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_prog_read_reg
*  Input   :   addr
*           pdata
*  Output  :
*  function    :   read register of icn85xx, prog mode
***********************************************************************************************/
int icn85xx_prog_read_reg( unsigned int addr, char *pdata )
{
	int ret = -1;
	ret = icn85xx_prog_i2c_rxdata( addr, pdata, 1 );
	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_prog_read_page
*  Input   :   Addr
*           Buffer
*  Output  :
*  function    :   read large register of icn85xx, in prog mode
***********************************************************************************************/
int icn85xx_prog_read_page( unsigned int Addr, unsigned char *Buffer, unsigned int Length )
{
	int		ret		= 0;
	unsigned int	StartAddr	= Addr;
	while ( Length )
	{
		if ( Length > MAX_LENGTH_PER_TRANSFER )
		{
			ret		= icn85xx_prog_i2c_rxdata( StartAddr, Buffer, MAX_LENGTH_PER_TRANSFER );
			Length		-= MAX_LENGTH_PER_TRANSFER;
			Buffer		+= MAX_LENGTH_PER_TRANSFER;
			StartAddr	+= MAX_LENGTH_PER_TRANSFER;
		}else  {
			ret		= icn85xx_prog_i2c_rxdata( StartAddr, Buffer, Length );
			Length		= 0;
			Buffer		+= Length;
			StartAddr	+= Length;
			break;
		}
		icn85xx_error( "\n icn85xx_prog_read_page StartAddr:0x%x, length: %d\n", StartAddr, Length );
	}
	if ( ret < 0 )
	{
		icn85xx_error( "\n icn85xx_prog_read_page failed! StartAddr:  0x%x, ret: %d\n", StartAddr, ret );
		return(ret);
	}else     {
		//printk( "\n icn85xx_prog_read_page, StartAddr 0x%x, Length: %d\n", StartAddr, Length );
		return(ret);
	}
}


/***********************************************************************************************
*  Name    :   icn85xx_i2c_rxdata
*  Input   :   addr
* *rxdata
*           length
*  Output  :   ret
*  function    : read data from icn85xx, normal mode
***********************************************************************************************/
int icn85xx_i2c_rxdata( unsigned short addr, char *rxdata, int length )
{
	int		ret	= -1;
	int		retries = 0;
	unsigned char	tmp_buf[2];
#if 0
	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 2,
			.buf	= tmp_buf,
#if SUPPORT_ROCKCHIP
			.scl_rate = ICN85XX_I2C_SCL,
#endif
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
#if SUPPORT_ROCKCHIP
			.scl_rate = ICN85XX_I2C_SCL,
#endif
		},
	};

	tmp_buf[0]	= (unsigned char) (addr >> 8);
	tmp_buf[1]	= (unsigned char) (addr);


	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msgs, 2 );
		if ( ret == 2 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#else

	tmp_buf[0]	= (unsigned char) (addr >> 8);
	tmp_buf[1]	= (unsigned char) (addr);

	while ( retries < IIC_RETRY_NUM )
	{
		/*  ret = i2c_transfer(this_client->adapter, msgs, 2); */
		ret = i2c_master_send( this_client, tmp_buf, 2 );
		if ( ret < 0 )
			return(ret);
		ret = i2c_master_recv( this_client, rxdata, length );
		if ( ret < 0 )
			return(ret);
		if ( ret == length )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c read error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}
#endif

	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_i2c_txdata
*  Input   :   addr
* *rxdata
*           length
*  Output  :   ret
*  function    : send data to icn85xx , normal mode
***********************************************************************************************/
int icn85xx_i2c_txdata( unsigned short addr, char *txdata, int length )
{
	int		ret = -1;
	unsigned char	tmp_buf[128];
	int		retries = 0;

	struct i2c_msg msg[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= length + 2,
			.buf	= tmp_buf,
		},
	};

	if ( length > 125 )
	{
		icn85xx_error( "%s too big datalen = %d!\n", __func__, length );
		return(-1);
	}

	tmp_buf[0]	= (unsigned char) (addr >> 8);
	tmp_buf[1]	= (unsigned char) (addr);

	if ( length != 0 && txdata != NULL )
	{
		memcpy( &tmp_buf[2], txdata, length );
	}

	while ( retries < IIC_RETRY_NUM )
	{
		ret = i2c_transfer( this_client->adapter, msg, 1 );
		if ( ret == 1 )
			break;
		retries++;
	}

	if ( retries >= IIC_RETRY_NUM )
	{
		icn85xx_error( "%s i2c write error: %d\n", __func__, ret );
/*        icn85xx_ts_reset(); */
	}

	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_write_reg
*  Input   :   addr -- address
*           para -- parameter
*  Output  :
*  function    :   write register of icn85xx, normal mode
***********************************************************************************************/
int icn85xx_write_reg( unsigned short addr, char para )
{
	char	buf[3];
	int	ret = -1;

	buf[0]	= para;
	ret	= icn85xx_i2c_txdata( addr, buf, 1 );
	if ( ret < 0 )
	{
		icn85xx_error( "write reg failed! %#x ret: %d\n", buf[0], ret );
		return(-1);
	}

	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_read_reg
*  Input   :   addr
*           pdata
*  Output  :
*  function    :   read register of icn85xx, normal mode
***********************************************************************************************/
int icn85xx_read_reg( unsigned short addr, char *pdata )
{
	int ret = -1;
	ret = icn85xx_i2c_rxdata( addr, pdata, 1 );
	if ( ret < 0 )
	{
		icn85xx_error( "addr: 0x%x: 0x%x\n", addr, *pdata );
	}
	return(ret);
}


/***********************************************************************************************
*  Name    :   icn85xx_log
*  Input   :   0: rawdata, 1: diff data
*  Output  :   err type
*  function    :   calibrate param
***********************************************************************************************/
static void icn85xx_log( char diff )
{
	char	row	= 0;
	char	column	= 0;

	char	rowMax		= 0;                                            /* add by carlos_hu */
	char	columnMax	= 0;                                            /* add by carlos_hu */


	int	i, j, ret;
	char	retvalue = 0;

	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client ); /* add by carlos_hu */

/*----------add by carlos_hu----------*/
	if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_85) )
	{
		rowMax		= 36;
		columnMax	= 24;
	}else if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_86) )
	{
		rowMax		= 42;
		columnMax	= 30;
	}else if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_87) )
	{
		rowMax		= 23;
		columnMax	= 12;
	}
	icn85xx_read_reg( 0x8004, &row );
	icn85xx_read_reg( 0x8005, &column );
	/* scan tp rawdata */
	icn85xx_write_reg( 4, 0x20 );
	mdelay( 1 );
	for ( i = 0; i < 1000; i++ )
	{
		mdelay( 1 );
		icn85xx_read_reg( 2, &retvalue );
		if ( retvalue == 1 )
			break;
	}
	if ( diff == 0 )
	{
		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x2000 + i * columnMax * 2, (char *) &log_rawdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
			icn85xx_rawdatadump( &log_rawdata[i][0], column, columnMax );
		}
	}
	if ( diff == 1 )
	{
		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x3000 + (i + 1) * (columnMax + 2) * 2 + 2, (char *) &log_diffdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
			icn85xx_rawdatadump( &log_diffdata[i][0], column, columnMax );
		}
	}else if ( diff == 2 )
	{
		for ( i = 0; i < row; i++ )
		{
			ret = icn85xx_i2c_rxdata( 0x2000 + i * columnMax * 2, (char *) &log_rawdata[i][0], column * 2 );
			if ( ret < 0 )
			{
				icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			}
			if ( (log_basedata[0][0] != 0) || (log_basedata[0][1] != 0) )
			{
				for ( j = 0; j < column; j++ )
				{
					log_rawdata[i][j] = log_basedata[i][j] - log_rawdata[i][j];
				}
			}
			icn85xx_rawdatadump( &log_rawdata[i][0], column, columnMax );
		}
		if ( (log_basedata[0][0] == 0) && (log_basedata[0][1] == 0) )
		{
			memcpy( &log_basedata[0][0], &log_rawdata[0][0], COL_NUM * ROW_NUM * 2 );
		}
	}

	/* finish scan tp rawdata */
	icn85xx_write_reg( 2, 0x0 );
	icn85xx_write_reg( 4, 0x21 );
}


static int icn85xx_testTP( char type, int para1, int para2, int para3 )
{
	char			retvalue = 0; /* ok */
	char			ret;
	char			row		= 0;
	char			column		= 0;
	char			rowMax		= 0;
	char			columnMax	= 0;
	char			ScanMode	= 0;
	char			ictype		= 85;
	int			diffMax		= 0;
	int			diffMin		= 100000;
	char			vol, regValue;
	int			i, j;
	int			regAddr;
	struct icn85xx_ts_data	*icn85xx_ts = i2c_get_clientdata( this_client );

	if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_85) )
	{
		rowMax		= 36;
		columnMax	= 24;
		ictype		= 85;
	}else if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_86) )
	{
		rowMax		= 42;
		columnMax	= 30;
		ictype		= 86;
	}else if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_87) )
	{
		rowMax		= 23;
		columnMax	= 12;
		ictype		= 87;
	}

	icn85xx_read_reg( 0x8004, &row );
	icn85xx_read_reg( 0x8005, &column );

	icn85xx_ts->work_mode = 4; /* idle */
	if ( type == 0 )
	{
		memset( &log_basedata[0][0], 0, COL_NUM * ROW_NUM * 2 );
		icn85xx_log( 0 );
		for ( i = 0; i < row; i++ )
		{
			for ( j = 0; j < (column - 1); j++ )
			{
				log_basedata[i][j] = log_rawdata[i][j + 1] - log_rawdata[i][j];
			}
		}

		for ( i = 0; i < (row - 1); i++ )
		{
			for ( j = 0; j < (column - 1); j++ )
			{
				log_rawdata[i][j] = log_basedata[i + 1][j] - log_basedata[i][j];
				if ( abs( log_rawdata[i][j] ) > abs( para1 ) )
				{
					retvalue = 1; /* error */
				}
			}
			icn85xx_rawdatadump( &log_rawdata[i][0], column - 1, columnMax );
		}
	}else if ( type == 1 )
	{
		memset( &log_rawdata[0][0], 0, COL_NUM * ROW_NUM * 2 );

		if ( ictype == 86 )
		{
			icn85xx_read_reg( 0x8000 + 87, &ScanMode );
			icn85xx_write_reg( 0x8000 + 87, 0 ); /* change scanmode */


			//printk( "reinit tp1, ScanMode: %d\n", ScanMode );
			icn85xx_write_reg( 0, 1 );
			mdelay( 100 );
			icn85xx_write_reg( 0, 0 );
			mdelay( 100 );
			regAddr = 0x040b38;
			ret	= icn85xx_i2c_txdata( 0xf000, &regAddr, 4 );
			if ( ret < 0 )
			{
				icn85xx_error( "1 write reg failed! ret: %d\n", ret );
				return(-1);
			}
			ret = icn85xx_i2c_rxdata( 0xf004, &regValue, 1 );
			//printk( "regValue: 0x%x\n", regValue );
			ret = icn85xx_i2c_txdata( 0xf000, &regAddr, 4 );
			if ( ret < 0 )
			{
				icn85xx_error( "2 write reg failed! ret: %d\n", ret );
				return(-1);
			}
			regAddr = regValue & 0xfe;
			ret	= icn85xx_i2c_txdata( 0xf004, &regAddr, 1 );
			if ( ret < 0 )
			{
				icn85xx_error( "3 write reg failed! ret: %d\n", ret );
				return(-1);
			}
		}
		mdelay( 100 );
		for ( i = 0; i < 5; i++ )
			icn85xx_log( 0 );

		vol = (para1 & 0xff) | 0x10;
		//printk( "-------vol: 0x%x\n", vol );
		icn85xx_write_reg( 4, vol );
		memcpy( &log_basedata[0][0], &log_rawdata[0][0], COL_NUM * ROW_NUM * 2 );
		mdelay( 100 );
		for ( i = 0; i < 5; i++ )
			icn85xx_log( 0 );

		icn85xx_write_reg( 4, 0x10 );

		if ( ictype == 86 )
		{
			icn85xx_write_reg( 0x8000 + 87, ScanMode );

			//printk( "reinit tp2\n" );
			icn85xx_write_reg( 0, 1 );
			mdelay( 100 );
			icn85xx_write_reg( 0, 0 );
			mdelay( 100 );

			regAddr = 0x040b38;
			ret	= icn85xx_i2c_txdata( 0xf000, &regAddr, 4 );
			if ( ret < 0 )
			{
				icn85xx_error( "4 write reg failed! ret: %d\n", ret );
				return(-1);
			}
			ret = icn85xx_i2c_txdata( 0xf004, &regValue, 1 );
			if ( ret < 0 )
			{
				icn85xx_error( "5 write reg failed! ret: %d\n", ret );
				return(-1);
			}
		}
		update_data2	= para2 * 120 / 100;
		update_data3	= para3 * 80 / 100;
		//printk( "-------diff data: %d, %d\n", update_data2, update_data3 );
		for ( i = 0; i < row; i++ )
		{
			for ( j = 0; j < column; j++ )
			{
				log_diffdata[i][j] = log_basedata[i][j] - log_rawdata[i][j];
				if ( log_diffdata[i][j] > diffMax )
				{
					diffMax = log_diffdata[i][j];
				}else if ( log_diffdata[i][j] < diffMin )
				{
					diffMin = log_diffdata[i][j];
				}

				if ( (update_data2 > 0) && (update_data3 > 0) ) /* make sure Max/Min > 0 */
				{
					if ( (log_diffdata[i][j] > update_data2) || (log_diffdata[i][j] < update_data3) )
					{
						retvalue = 1;                   /* error */
					}
				}
			}

			icn85xx_rawdatadump( &log_diffdata[i][0], column, columnMax );
		}

		update_data2	= diffMax;
		update_data3	= diffMin;
	}
	icn85xx_ts->work_mode = 0;

	return(retvalue);
}


/***********************************************************************************************
*  Name    :   icn85xx_iic_test
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/
static int icn85xx_iic_test( void )
{
	struct icn85xx_ts_data	*icn85xx_ts	= i2c_get_clientdata( this_client );
	int			ret		= -1;
	char			value		= 0;
	char			buf[3];
	int			retry = 0;
	int			flashid;
	icn85xx_ts->ictype = 0;
	icn85xx_trace( "====%s begin=====.  \n", __func__ );
	icn85xx_ts->ictype = ICTYPE_UNKNOWN;

	while ( retry++ < 3 )
	{
		ret = icn85xx_read_reg( 0xa, &value );
		if ( ret > 0 )
		{
			if ( value == 0x85 )
			{
				icn85xx_ts->ictype = ICN85XX_WITH_FLASH_85;
				setbootmode( ICN85XX_WITH_FLASH_85 );
				return(ret);
			}else if ( (value == 0x86) || (value == 0x88) )
			{
				icn85xx_ts->ictype = ICN85XX_WITH_FLASH_86;
				setbootmode( ICN85XX_WITH_FLASH_86 );
				return(ret);
			}else if ( value == 0x87 )
			{
				icn85xx_ts->ictype = ICN85XX_WITH_FLASH_87;
				setbootmode( ICN85XX_WITH_FLASH_87 );
				return(ret);
			}
		}

		icn85xx_error( "iic test error! retry = %d\n", retry );
		msleep( 3 );
	}

	/*
	 * add junfuzhang 20131211
	 * force ic enter progmode
	 */
	icn85xx_goto_progmode();
	msleep( 10 );

	retry = 0;
	while ( retry++ < 3 )
	{
		buf[0]	= buf[1] = buf[2] = 0x0;
		ret	= icn85xx_prog_i2c_txdata( 0x040000, buf, 3 );
		if ( ret < 0 )
		{
			icn85xx_error( "write reg failed! ret: %d\n", ret );
			return(ret);
		}
		ret = icn85xx_prog_i2c_rxdata( 0x040000, buf, 3 );
		icn85xx_trace( "icn85xx_check_progmod: %d, 0x%2x, 0x%2x, 0x%2x\n", ret, buf[0], buf[1], buf[2] );
		if ( ret > 0 )
		{
			/* if(value == 0x85) */
			if ( (buf[2] == 0x85) && (buf[1] == 0x05) )
			{
				flashid = icn85xx_read_flashid();
				if ( (MD25D40_ID1 == flashid) || (MD25D40_ID2 == flashid)
				     || (MD25D20_ID1 == flashid) || (MD25D20_ID2 == flashid)
				     || (GD25Q10_ID == flashid) || (MX25L512E_ID == flashid)
				     || (MD25D05_ID == flashid) || (MD25D10_ID == flashid) )
				{
					icn85xx_ts->ictype = ICN85XX_WITH_FLASH_85;
					setbootmode( ICN85XX_WITH_FLASH_85 );
				}else  {
					icn85xx_ts->ictype = ICN85XX_WITHOUT_FLASH_85;
					setbootmode( ICN85XX_WITHOUT_FLASH_85 );
				}
				return(ret);
			}else if ( (buf[2] == 0x85) && (buf[1] == 0x0e) )
			{
				flashid = icn85xx_read_flashid();
				if ( (MD25D40_ID1 == flashid) || (MD25D40_ID2 == flashid)
				     || (MD25D20_ID1 == flashid) || (MD25D20_ID2 == flashid)
				     || (GD25Q10_ID == flashid) || (MX25L512E_ID == flashid)
				     || (MD25D05_ID == flashid) || (MD25D10_ID == flashid) )
				{
					icn85xx_ts->ictype = ICN85XX_WITH_FLASH_86;
					setbootmode( ICN85XX_WITH_FLASH_86 );
				}else  {
					icn85xx_ts->ictype = ICN85XX_WITHOUT_FLASH_86;
					setbootmode( ICN85XX_WITHOUT_FLASH_86 );
				}
				return(ret);
			}else  { /* for ICNT87 */
				ret = icn87xx_prog_i2c_rxdata( 0xf001, buf, 2 );
				if ( ret > 0 )
				{
					if ( buf[1] == 0x87 )
					{
						flashid = icn87xx_read_flashid();
						//printk( "icnt87 flashid: 0x%x\n", flashid );
						if ( 0x114051 == flashid )
						{
							icn85xx_ts->ictype = ICN85XX_WITH_FLASH_87;
							setbootmode( ICN85XX_WITH_FLASH_87 );
						}else  {
							icn85xx_ts->ictype = ICN85XX_WITHOUT_FLASH_87;
							setbootmode( ICN85XX_WITHOUT_FLASH_87 );
						}
						return(ret);
					}
				}
			}
		}
		icn85xx_error( "iic2 test error! %d\n", retry );
		msleep( 3 );
	}

	return(ret);
}

#ifdef ICN_GESTURE
/*
void kpd_touchpanel_gesture_handler(int key_code)
{
            struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata(this_client);
            input_report_key(icn85xx_ts->input_dev, key_code, 1);
            input_sync(icn85xx_ts->input_dev);
            input_report_key(icn85xx_ts->input_dev, key_code, 0);
            input_sync(icn85xx_ts->input_dev);
}
*/
//*********************************************************************************//
static void icn85xx_gesture_handler(viod)
{
	int ret;
	int ges_key = 0;
	int buf[2]={0};
	
	icn85xx_trace("%s, %d\n", __FUNCTION__, __LINE__);
	
	//if(suspend_status)
	//{	
		ret = icn85xx_i2c_rxdata(0x7000, buf, 1);	
		if(ret < 0)
		{
			icn85xx_trace("%s read_data i2c_rxdata failed[Read  gesture code]: %d\n", __func__, ret);
			return ret;
		}
		icn85xx_trace("----twd.gestrue_en==true--- 0x7000=0x%x -------\n",buf[0]);

		gesture_id=buf[0];
		
		icn85xx_trace("[icn85xx_gesture_handler]  gesture_id=0x%x\n",gesture_id);
		struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata(this_client);
		switch(gesture_id)
		{
			case GESTURE_LEFT:
				if((ft_gesture_enable & TP_GESTURE_LEFT)==TP_GESTURE_LEFT)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_LEFT, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_LEFT, 0);
	                input_sync(icn85xx_ts->input_dev);	    		   
				}
	            break;
	        case GESTURE_RIGHT:
				if((ft_gesture_enable & TP_GESTURE_RIGHT)==TP_GESTURE_RIGHT)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_RIGHT, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_RIGHT, 0);
	                input_sync(icn85xx_ts->input_dev);
				}
			    break;
	        case GESTURE_UP:
				if((ft_gesture_enable & TP_GESTURE_UP)==TP_GESTURE_UP)
				{			
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_UP, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_UP, 0);
	                input_sync(icn85xx_ts->input_dev);  
				}                                
	            break;
	        case GESTURE_DOWN:
				if((ft_gesture_enable & TP_GESTURE_DOWN)==TP_GESTURE_DOWN)
				{			
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_DOWN, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_DOWN, 0);
	                input_sync(icn85xx_ts->input_dev);
				}
	            break;
	        case GESTURE_DOUBLECLICK:
				if((ft_gesture_enable & TP_GESTURE_DOBULECLICK)==TP_GESTURE_DOBULECLICK)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_U, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_U, 0);
	                input_sync(icn85xx_ts->input_dev);
			
				}
	            break;
	        case GESTURE_O:
				if((ft_gesture_enable & TP_GESTURE_O)==TP_GESTURE_O)
				{			
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_O, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_O, 0);
	                input_sync(icn85xx_ts->input_dev);
			 
				}
	            break;
	        case GESTURE_W:
				if((ft_gesture_enable & TP_GESTURE_W)==TP_GESTURE_W)
				{			
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_W, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_W, 0);
	                input_sync(icn85xx_ts->input_dev);
			  
				}
	            break;
	        case GESTURE_M:
				if((ft_gesture_enable & TP_GESTURE_M)==TP_GESTURE_M)
				{			
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_M, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_M, 0);
	                input_sync(icn85xx_ts->input_dev);
			 
				}
	            break;
	        case GESTURE_E:
				if((ft_gesture_enable & TP_GESTURE_E)==TP_GESTURE_E)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_E, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_E, 0);
	                input_sync(icn85xx_ts->input_dev);
			
				}
	            break;
	        case GESTURE_C:
				if((ft_gesture_enable & TP_GESTURE_C)==TP_GESTURE_C)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_C, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_C, 0);
	                input_sync(icn85xx_ts->input_dev);
			
				}
	            break;
/*	        case GESTURE_L:
				if((ft_gesture_enable & TP_GESTURE_L)==TP_GESTURE_L)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_L, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_L, 0);
	                input_sync(icn85xx_ts->input_dev);
		
				}
	            break;
*/
	        case GESTURE_S:
				if((ft_gesture_enable & TP_GESTURE_S)==TP_GESTURE_S)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_S, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_S, 0);
	                input_sync(icn85xx_ts->input_dev);
		
				}
	            break;
	        case GESTURE_V:
				if((ft_gesture_enable & TP_GESTURE_V)==TP_GESTURE_V)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_V, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_V, 0);
	                input_sync(icn85xx_ts->input_dev);

				}
	            break;
	        case GESTURE_Z:
				if((ft_gesture_enable & TP_GESTURE_Z)==TP_GESTURE_Z)
				{
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_Z, 1);
	                input_sync(icn85xx_ts->input_dev);
	                input_report_key(icn85xx_ts->input_dev, KEY_GESTURE_Z, 0);
	                input_sync(icn85xx_ts->input_dev);

				}
	            break;
	        default:
	                break;

	} 
	return ;
}
//*********************************************************************************//
#endif


/***********************************************************************************************
*  Name    :   icn85xx_ts_release
*  Input   :   void
*  Output  :
*  function    : touch release
***********************************************************************************************/
static void icn85xx_ts_release( void )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
	icn85xx_info( "==icn85xx_ts_release ==\n" );
	input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 0 );
	input_sync( icn85xx_ts->input_dev );
}


/***********************************************************************************************
*  Name    :   icn85xx_report_value_A
*  Input   :   void
*  Output  :
*  function    : reprot touch ponit
***********************************************************************************************/
static void icn85xx_report_value_A( void )
{
	struct icn85xx_ts_data	*icn85xx_ts			= i2c_get_clientdata( this_client );
	char			buf[POINT_NUM * POINT_SIZE + 3] = { 0 };
	int			ret				= -1;
	int			i;
	icn85xx_info( "==icn85xx_report_value_A ==\n" );
#if TOUCH_VIRTUAL_KEYS
	unsigned char		button;
	static unsigned char	button_last;
#endif
	
	
#ifdef ICN_GESTURE
		if(icn85xx_gesture_enable)
		{
			icn85xx_gesture_handler();
			return 0;
		}
#endif


	ret = icn85xx_i2c_rxdata( 0x1000, buf, POINT_NUM * POINT_SIZE + 2 );
	if ( ret < 0 )
	{
		icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
		return;
	}
#if TOUCH_VIRTUAL_KEYS
	button = buf[0];
	icn85xx_info( "%s: button=%d\n", __func__, button );

	if ( (button_last != 0) && (button == 0) )
	{
		icn85xx_ts_release();
		button_last = button;
		return;
	}
	if ( button != 0 )
	{
		switch ( button )
		{
		case ICN_VIRTUAL_BUTTON_HOME:
			icn85xx_info( "ICN_VIRTUAL_BUTTON_HOME down\n" );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 200 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_X, 250 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, 900 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_WIDTH_MAJOR, 1 );
			input_mt_sync( icn85xx_ts->input_dev );
			input_sync( icn85xx_ts->input_dev );
			break;
		case ICN_VIRTUAL_BUTTON_BACK:
			icn85xx_info( "ICN_VIRTUAL_BUTTON_BACK down\n" );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 200 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_X, 60 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, 900 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_WIDTH_MAJOR, 1 );
			input_mt_sync( icn85xx_ts->input_dev );
			input_sync( icn85xx_ts->input_dev );
			break;
		case ICN_VIRTUAL_BUTTON_MENU:
			icn85xx_info( "ICN_VIRTUAL_BUTTON_MENU down\n" );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 200 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_X, 460 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, 900 );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_WIDTH_MAJOR, 1 );
			input_mt_sync( icn85xx_ts->input_dev );
			input_sync( icn85xx_ts->input_dev );
			break;
		default:
			icn85xx_info( "other gesture\n" );
			break;
		}
		button_last = button;
		return;
	}
#endif

	icn85xx_ts->point_num = buf[1];
	if ( icn85xx_ts->point_num == 0 )
	{
		icn85xx_ts_release();
		return;
	}
	for ( i = 0; i < icn85xx_ts->point_num; i++ )
	{
		if ( buf[8 + POINT_SIZE * i] != 4 )
		{
			break;
		}else  {
		}
	}

	if ( i == icn85xx_ts->point_num )
	{
		icn85xx_ts_release();
		return;
	}

	for ( i = 0; i < icn85xx_ts->point_num; i++ )
	{
		icn85xx_ts->point_info[i].u8ID		= buf[2 + POINT_SIZE * i];
		icn85xx_ts->point_info[i].u16PosX	= (buf[4 + POINT_SIZE * i] << 8) + buf[3 + POINT_SIZE * i];
		icn85xx_ts->point_info[i].u16PosY	= (buf[6 + POINT_SIZE * i] << 8) + buf[5 + POINT_SIZE * i];
		icn85xx_ts->point_info[i].u8Pressure	= buf[7 + POINT_SIZE * i];
		icn85xx_ts->point_info[i].u8EventId	= buf[8 + POINT_SIZE * i];

		if ( 1 == icn85xx_ts->revert_x_flag )
		{
			icn85xx_ts->point_info[i].u16PosX = icn85xx_ts->screen_max_x - icn85xx_ts->point_info[i].u16PosX;
		}
		if ( 1 == icn85xx_ts->revert_y_flag )
		{
			icn85xx_ts->point_info[i].u16PosY = icn85xx_ts->screen_max_y - icn85xx_ts->point_info[i].u16PosY;
		}

		icn85xx_info( "u8ID %d\n", icn85xx_ts->point_info[i].u8ID );
		icn85xx_info( "u16PosX %d\n", icn85xx_ts->point_info[i].u16PosX );
		icn85xx_info( "u16PosY %d\n", icn85xx_ts->point_info[i].u16PosY );
		icn85xx_info( "u8Pressure %d\n", icn85xx_ts->point_info[i].u8Pressure );
		icn85xx_info( "u8EventId %d\n", icn85xx_ts->point_info[i].u8EventId );


		input_report_abs( icn85xx_ts->input_dev, ABS_MT_TRACKING_ID, icn85xx_ts->point_info[i].u8ID );
		input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, icn85xx_ts->point_info[i].u8Pressure );
		input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_X, icn85xx_ts->point_info[i].u16PosX );
		input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, icn85xx_ts->point_info[i].u16PosY );
		input_report_abs( icn85xx_ts->input_dev, ABS_MT_WIDTH_MAJOR, 1 );
		input_mt_sync( icn85xx_ts->input_dev );
		icn85xx_point_info( "point: %d ===x = %d,y = %d, press = %d ====\n", i, icn85xx_ts->point_info[i].u16PosX, icn85xx_ts->point_info[i].u16PosY, icn85xx_ts->point_info[i].u8Pressure );
	}

	input_sync( icn85xx_ts->input_dev );
}


/***********************************************************************************************
*  Name    :   icn85xx_report_value_B
*  Input   :   void
*  Output  :
*  function    : reprot touch ponit
***********************************************************************************************/
#if CTP_REPORT_PROTOCOL
static void icn85xx_report_value_B( void )
{
	struct icn85xx_ts_data	*icn85xx_ts			= i2c_get_clientdata( this_client );
	char			buf[POINT_NUM * POINT_SIZE + 3] = { 0 };
	static unsigned char	finger_last[POINT_NUM + 1]	= { 0 };
	unsigned char		finger_current[POINT_NUM + 1]	= { 0 };
	unsigned int		position			= 0;
	int			temp				= 0;
	int			ret				= -1;
	icn85xx_info( "==icn85xx_report_value_B ==\n" );

#if VIRTUAL_KEY_VALUE

	unsigned int	i		= 0;
	unsigned int	key_value	= 0;
#endif
	
#ifdef ICN_GESTURE
		if(icn85xx_gesture_enable)
		{
			icn85xx_gesture_handler();
			return 0;
		}
#endif

	ret = icn85xx_i2c_rxdata( 0x1000, buf, POINT_NUM * POINT_SIZE + 2 );
	if ( ret < 0 )
	{
		icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
		return;
	}
/* printk("proximity_flag~~ = %d, buf[0]= %x 11111111111111\n",icn85xx_proximity_flag, buf[0]); */

#ifdef  TPD_PROXIMITY
	//printk( "proximity_flag~~ = %d, buf[0]= %x \n", icn85xx_proximity_flag, buf[0] );
	if ( icn85xx_proximity_flag == 1 )
	{
		if ( 1 == (buf[0] >> 7) )
		{
			ps_state = 1;
	//		input_report_abs( ps_input_dev, ABS_DISTANCE, 0 );
	//		input_sync( ps_input_dev );
			printk( "**************(buf[0] >>7)  = %x  tp is near*****************\n", (buf[0] >> 7) );
			/*  return; */
		}else if ( 0 == (buf[0] >> 7) )
		{
			ps_state = 0;
	//		input_report_abs( ps_input_dev, ABS_DISTANCE, 1 );
	//		input_sync( ps_input_dev );
			printk( "**************(buf[0] >>7) = %x  tp is far******************\n", (buf[0] >> 7) );
		}

		input_report_abs( ps_input_dev, ABS_DISTANCE, !ps_state );
		input_sync( ps_input_dev );
	}
#endif


#if VIRTUAL_KEY_VALUE
	key_value = buf[0];

	//printk( "[1430] : key_value=%d\n", key_value );
	switch ( key_value )
	{
	case 0x04:
		input_report_key( icn85xx_ts->input_dev, KEY_BACK, 1 );
		input_sync( icn85xx_ts->input_dev );
		pre_key = KEY_BACK;
		return;
		break;
	case 0x02:
		input_report_key( icn85xx_ts->input_dev, KEY_HOMEPAGE, 1 );
		input_sync( icn85xx_ts->input_dev );
		pre_key = KEY_HOMEPAGE;
		return;
		break;
	case 0x01:
		input_report_key( icn85xx_ts->input_dev, KEY_MENU, 1 );
		input_sync( icn85xx_ts->input_dev );
		pre_key = KEY_MENU;
		return;
		break;
		
	case 0x00:
		if ( pre_key > 0 )
		{
			//printk( "[1952] : key_up  pre_key=%d\n", pre_key );
			input_report_key( icn85xx_ts->input_dev, pre_key, 0 );
			input_sync( icn85xx_ts->input_dev );
			pre_key = 0;
			return;
		}
		break;
	default:
		//printk( "xxxxxxxxxxxxxxxxxxxxxxxxxxxx" );
		break;
	}

	/* pre_key = key_value; */
#endif

#if CTP_CHARGER_DETECT

	if ( !batt_psy )
	{
		batt_psy = power_supply_get_by_name( "usb" );
		icn85xx_info( "battery supply is not found" );
	}else  {
		is_charger_status = (unsigned char) power_supply_is_system_supplied();

		icn85xx_info( "is_charger_status %d\n", is_charger_status );

		if ( is_charger_status != pre_charger_status )
		{
			pre_charger_status = is_charger_status;
			if ( is_charger_status )
				icn85xx_write_reg( 0x04, 0x55 );
			else
				icn85xx_write_reg( 0x04, 0x66 );

			icn85xx_info( "is_charger_status= %d,pre_charger_status= \n", is_charger_status, pre_charger_status );
		}
	}
	
#endif

	icn85xx_ts->point_num = buf[1];
	if ( icn85xx_ts->point_num > POINT_NUM )
	{
		return;
	}

#if CFG_ICN_DETECT_UP_EVENT
	_sui_last_point_cnt = icn85xx_ts->point_num;
#endif


	if ( icn85xx_ts->point_num > 0 )
	{
		for ( position = 0; position < icn85xx_ts->point_num; position++ )
		{
			temp = buf[2 + POINT_SIZE * position] + 1;

			finger_current[temp]			= 1;
			icn85xx_ts->point_info[temp].u8ID	= buf[2 + POINT_SIZE * position];
			icn85xx_ts->point_info[temp].u16PosX	= (buf[4 + POINT_SIZE * position] << 8) + buf[3 + POINT_SIZE * position];
			icn85xx_ts->point_info[temp].u16PosY	= (buf[6 + POINT_SIZE * position] << 8) + buf[5 + POINT_SIZE * position];
			icn85xx_ts->point_info[temp].u8Pressure = buf[7 + POINT_SIZE * position];
			icn85xx_ts->point_info[temp].u8EventId	= buf[8 + POINT_SIZE * position];

			if ( icn85xx_ts->point_info[temp].u8EventId == 0x04 )
				finger_current[temp] = 0;

#if 0
			if ( 1 == icn85xx_ts->revert_x_flag )
			{
				icn85xx_ts->point_info[temp].u16PosX = icn85xx_ts->screen_max_x - icn85xx_ts->point_info[temp].u16PosX;
			}
			if ( 1 == icn85xx_ts->revert_y_flag )
			{
				icn85xx_ts->point_info[temp].u16PosY = icn85xx_ts->screen_max_y - icn85xx_ts->point_info[temp].u16PosY;
			}
#endif

			icn85xx_info( "temp %d\n", temp );
			icn85xx_info( "u8ID %d\n", icn85xx_ts->point_info[temp].u8ID );
			icn85xx_info( "u16PosX %d\n", icn85xx_ts->point_info[temp].u16PosX );
			icn85xx_info( "u16PosY %d\n", icn85xx_ts->point_info[temp].u16PosY );
			icn85xx_info( "u8Pressure %d\n", icn85xx_ts->point_info[temp].u8Pressure );
			icn85xx_info( "u8EventId %d\n", icn85xx_ts->point_info[temp].u8EventId );
			/* icn85xx_info("u8Pressure %d\n", icn85xx_ts->point_info[temp].u8Pressure*16); */
		}
	}else  {
		for ( position = 1; position < POINT_NUM + 1; position++ )
		{
			finger_current[position] = 0;
		}
		icn85xx_info( "no touch\n" );
	}

	for ( position = 1; position < POINT_NUM + 1; position++ )
	{
		if ( (finger_current[position] == 0) && (finger_last[position] != 0) )
		{
			input_mt_slot( icn85xx_ts->input_dev, position - 1 );
			input_mt_report_slot_state( icn85xx_ts->input_dev, MT_TOOL_FINGER, false );
			icn85xx_point_info( "one touch up: %d\n", position );
		}else if ( finger_current[position] )
		{
			input_mt_slot( icn85xx_ts->input_dev, position - 1 );
			input_mt_report_slot_state( icn85xx_ts->input_dev, MT_TOOL_FINGER, true );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 1 );
			/* input_report_abs(icn85xx_ts->input_dev, ABS_MT_PRESSURE, icn85xx_ts->point_info[position].u8Pressure); */
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_PRESSURE, 200 );
			/*
			 * if(icn85xx_ts->xy_swap){
			 *      input_report_abs(icn85xx_ts->input_dev, ABS_MT_POSITION_X, icn85xx_ts->point_info[position].u16PosY);
			 *      input_report_abs(icn85xx_ts->input_dev, ABS_MT_POSITION_Y, icn85xx_ts->point_info[position].u16PosX);
			 * }
			 */
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_X, icn85xx_ts->point_info[position].u16PosX );
			input_report_abs( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, icn85xx_ts->point_info[position].u16PosY );
			//printk( "===position: %d, x = %d,y = %d, press = %d ====\n", position, icn85xx_ts->point_info[position].u16PosX, icn85xx_ts->point_info[position].u16PosY, icn85xx_ts->point_info[position].u8Pressure );
			/* input_report_key(icn85xx_ts->input_dev, BTN_TOUCH,1 ); */
		}
	}

	input_sync( icn85xx_ts->input_dev );

	for ( position = 1; position < POINT_NUM + 1; position++ )
	{
		finger_last[position] = finger_current[position];
	}
}


#endif


/***********************************************************************************************
*  Name    :   icn85xx_ts_pen_irq_work
*  Input   :   void
*  Output  :
*  function    : work_struct
***********************************************************************************************/
static void icn85xx_ts_pen_irq_work( struct work_struct *work )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
#if SUPPORT_PROC_FS
	if ( down_interruptible( &icn85xx_ts->sem ) )
	{
		return;
	}
#endif

	if ( icn85xx_ts->work_mode == 0 )
	{
#if CTP_REPORT_PROTOCOL
		icn85xx_report_value_B();
#else
		icn85xx_report_value_A();
#endif


		if ( icn85xx_ts->use_irq )
		{
			icn85xx_irq_enable();
		}
		if ( log_on_off == 4 )
		{
			//printk( "normal raw data\n" );
			icn85xx_log( 0 );                                               /* raw data */
		}else if ( log_on_off == 5 )
		{
			//printk( "normal diff data\n" );
			icn85xx_log( 1 );                                               /* diff data */
		}else if ( log_on_off == 6 )
		{
			//printk( "normal raw2diff\n" );
			icn85xx_log( 2 );                                               /* diff data */
		}
	}else if ( icn85xx_ts->work_mode == 1 )
	{
		//printk( "raw data\n" );
		icn85xx_log( 0 );                                                       /* raw data */
	}else if ( icn85xx_ts->work_mode == 2 )
	{
		//printk( "diff data\n" );
		icn85xx_log( 1 );                                                       /* diff data */
	}else if ( icn85xx_ts->work_mode == 3 )
	{
		//printk( "raw2diff data\n" );
		icn85xx_log( 2 );                                                       /* diff data */
	}else if ( icn85xx_ts->work_mode == 4 )                                         /* idle */
	{
		;
	}else if ( icn85xx_ts->work_mode == 5 )                                         /* write para, reinit */
	{
		//printk( "reinit tp\n" );
		icn85xx_write_reg( 0, 1 );
		mdelay( 100 );
		icn85xx_write_reg( 0, 0 );
		icn85xx_ts->work_mode = 0;
	}else if ( (icn85xx_ts->work_mode == 6) || (icn85xx_ts->work_mode == 7) )       /* gesture test mode */
	{
#if SUPPORT_PROC_FS
		char	buf[sizeof(structGestureData)]	= { 0 };
		int	ret				= -1;
		int	i;
		ret = icn85xx_i2c_rxdata( 0x7000, buf, 2 );
		if ( ret < 0 )
		{
			icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			return;
		}
		structGestureData.u8Status	= 1;
		structGestureData.u8Gesture	= buf[0];
		structGestureData.u8GestureNum	= buf[1];
		proc_info( "structGestureData.u8Gesture: 0x%x\n", structGestureData.u8Gesture );
		proc_info( "structGestureData.u8GestureNum: %d\n", structGestureData.u8GestureNum );

		ret = icn85xx_i2c_rxdata( 0x7002, buf, structGestureData.u8GestureNum * 6 );
		if ( ret < 0 )
		{
			icn85xx_error( "%s read_data i2c_rxdata failed: %d\n", __func__, ret );
			return;
		}

		for ( i = 0; i < structGestureData.u8GestureNum; i++ )
		{
			structGestureData.point_info[i].u16PosX		= (buf[1 + 6 * i] << 8) + buf[0 + 6 * i];
			structGestureData.point_info[i].u16PosY		= (buf[3 + 6 * i] << 8) + buf[2 + 6 * i];
			structGestureData.point_info[i].u8EventId	= buf[5 + 6 * i];
			proc_info( "(%d, %d, %d)", structGestureData.point_info[i].u16PosX, structGestureData.point_info[i].u16PosY, structGestureData.point_info[i].u8EventId );
		}
		proc_info( "\n" );
		if ( icn85xx_ts->use_irq )
		{
			icn85xx_irq_enable();
		}

		if ( ( (icn85xx_ts->work_mode == 7) && (structGestureData.u8Gesture == 0xFB) )
		     || (icn85xx_ts->work_mode == 6) )
		{
			proc_info( "return normal mode\n" );
			icn85xx_ts->work_mode = 0; /* return normal mode */
		}

#endif
	}


#if SUPPORT_PROC_FS
	up( &icn85xx_ts->sem );
#endif
}


/***********************************************************************************************
*  Name    :   chipone_timer_func
*  Input   :   void
*  Output  :
*  function    : Timer interrupt service routine.
***********************************************************************************************/
static enum hrtimer_restart chipone_timer_func( struct hrtimer *timer )
{
	struct icn85xx_ts_data *icn85xx_ts = container_of( timer, struct icn85xx_ts_data, timer );
	queue_work( icn85xx_ts->ts_workqueue, &icn85xx_ts->pen_event_work );
	/* icn85xx_info("chipone_timer_func\n"); */
	if ( icn85xx_ts->use_irq == 1 )
	{
		if ( (icn85xx_ts->work_mode == 1) || (icn85xx_ts->work_mode == 2) || (icn85xx_ts->work_mode == 3) )
		{
			hrtimer_start( &icn85xx_ts->timer, ktime_set( CTP_POLL_TIMER / 1000, (CTP_POLL_TIMER % 1000) * 1000000 ), HRTIMER_MODE_REL );
		}
	}else  {
		hrtimer_start( &icn85xx_ts->timer, ktime_set( CTP_POLL_TIMER / 1000, (CTP_POLL_TIMER % 1000) * 1000000 ), HRTIMER_MODE_REL );
	}
	return(HRTIMER_NORESTART);
}


/***********************************************************************************************
*  Name    :   icn85xx_ts_interrupt
*  Input   :   void
*  Output  :
*  function    : interrupt service routine
***********************************************************************************************/
static irqreturn_t icn85xx_ts_interrupt( int irq, void *dev_id )
{
	struct icn85xx_ts_data *icn85xx_ts = dev_id;

	icn85xx_info( "==========------icn85xx_ts TS Interrupt-----============\n" );


#if CFG_ICN_DETECT_UP_EVENT
	del_timer( &_st_up_evnet_timer );
	_st_up_evnet_timer.expires = jiffies + 3 * HZ / 10; /* 300ms */
	add_timer( &_st_up_evnet_timer );
#endif

	if ( icn85xx_ts->use_irq )
		icn85xx_irq_disable();


	if ( !work_pending( &icn85xx_ts->pen_event_work ) )
	{
		icn85xx_info( "Enter work\n" );
		queue_work( icn85xx_ts->ts_workqueue, &icn85xx_ts->pen_event_work );
	}

	return(IRQ_HANDLED);
}


static void icn85xx_clear_report_data( struct icn85xx_ts_data *icn85xx_ts )
{
	int i;

	for ( i = 0; i < POINT_NUM; i++ )
	{
#if CTP_REPORT_PROTOCOL
		input_mt_slot( icn85xx_ts->input_dev, i );
		input_mt_report_slot_state( icn85xx_ts->input_dev, MT_TOOL_FINGER, false );
#endif
	}
	input_report_key( icn85xx_ts->input_dev, BTN_TOUCH, 0 );
#if !CTP_REPORT_PROTOCOL
	input_mt_sync( icn85xx_ts->input_dev );
#endif
	input_sync( icn85xx_ts->input_dev );
}


#if CFG_ICN_DETECT_UP_EVENT
static void icn85xx_force_read_up_event( unsigned long data )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
	if ( _sui_last_point_cnt > 0 )
	{
		/* icn85xx_trace("[icn] miss up envent, force send here.\n"); */
		//printk( "[icn] miss up envent, force send here 1111 .\n" );
		icn85xx_clear_report_data( icn85xx_ts );
	}else  {
		/* icn85xx_trace("[icn] miss up envent, force send here 2222.\n"); */
		//printk( "[icn] miss up envent, force send here 2222.\n" );
	}
}


#endif


/* #ifdef CONFIG_HAS_EARLYSUSPEND */


/***********************************************************************************************
*  Name    :   icn85xx_ts_suspend
*  Input   :   void
*  Output  :
*  function    : tp enter sleep mode
***********************************************************************************************/
static void icn85xx_ts_suspend( struct early_suspend *handler )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( this_client );
	icn85xx_trace( "icn85xx_ts_suspend\n" );
	//printk( "[wsl]----icn85xx_proximity_flag=:%d", icn85xx_proximity_flag );

#if SUPPORT_CHECK_ESD 
	icn85xx_ts->icn_is_suspend = 1;
	spin_lock(&icn85xx_ts->esd_lock);       
	if (icn85xx_ts->esd_running) 
	{                       
		icn85xx_ts->esd_running = 0;                       
		spin_unlock(&icn85xx_ts->esd_lock);                        
		icn85xx_trace("icn85xx Esd cancelled");                       
		cancel_delayed_work_sync(&icn85xx_esd_event_work);
	} 
	else 
	{                        
		spin_unlock(&icn85xx_ts->esd_lock);
	}        	 
#endif

#ifdef TPD_PROXIMITY
	//printk( "%s   TPD_PROXIMITY: icn85xx_proximity_flag=%d  \n", __func__, icn85xx_proximity_flag );
	if ( icn85xx_proximity_flag == 1 )
	{
		tpd_halt = 1;
		//printk( "%s : TP enter into sleep mode  ps return!\n", __func__ );
		return;
	}
	tpd_halt = 0;
#endif

#ifdef ICN_GESTURE
		if(ft_gesture_enable !=TP_GESTURE_OFF){	
			irq_set_irq_type(icn85xx_ts->irq,IRQF_TRIGGER_LOW); //IRQ_TYPE_EDGE_BOTH IRQ_TYPE_LEVEL_LOW
			icn85xx_write_reg(ICN85XX_REG_PMODE, 0x40); //gesture mode
			icn85xx_gesture_enable = 1;
			return;
		}
#endif

	if ( icn85xx_ts->use_irq )
	{
		icn85xx_irq_disable();
	}else  {
		hrtimer_cancel( &icn85xx_ts->timer );
	}
	flush_workqueue( icn85xx_ts->ts_workqueue );
#if SUSPEND_POWER_OFF
	/*
	 * power off
	 * todo
	 */

	/* reset flag if ic is flashless when power off */
	if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86)
	     || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87) )

	{
		icn85xx_ts->code_loaded_flag = 0;
	}
#else
	//printk( "%s : ------------PMODE_HIBERNATE----\n", __func__ );
	icn85xx_write_reg( ICN85xx_REG_PMODE, PMODE_HIBERNATE );
	//printk( "%s : ------------PMODE_HIBERNATE---end-\n", __func__ );
#endif

	icn85xx_clear_report_data( icn85xx_ts );
}


/***********************************************************************************************
*  Name    :   icn85xx_ts_resume
*  Input   :   void
*  Output  :
*  function    : wakeup tp or reset tp
***********************************************************************************************/
static void icn85xx_ts_resume( struct early_suspend *handler )
{
	struct icn85xx_ts_data	*icn85xx_ts = i2c_get_clientdata( this_client );
	int			i;
	int			ret		= -1;
	int			retry		= 5;
	int			need_update_fw	= false;
	unsigned char		value;
	icn85xx_trace( "==icn85xx_ts_resume== \n" );

#ifdef TPD_PROXIMITY
	//printk( "%s   TPD_PROXIMITY: icn85xx_proximity_flag=%d  \n", __func__, icn85xx_proximity_flag );
	if ( (1 == icn85xx_proximity_flag) && (1 == tpd_halt) ) /*  */
	{
		//printk( "%s : ps_state=%d !", __func__, ps_state );
		ps_state = 1;
		return;
	}
#endif

#ifdef ICN_GESTURE
		if(ft_gesture_enable !=TP_GESTURE_OFF){
			irq_set_irq_type(icn85xx_ts->irq,IRQF_TRIGGER_LOW);
			if(icn85xx_ts->ictype != ICN85XX_WITHOUT_FLASH_87)
			{
				icn85xx_write_reg(ICN85XX_REG_PMODE, 0x41);    //gesture mode
			}
			icn85xx_gesture_enable = 0;
		}
#endif	

	/* report touch release */
#if CTP_REPORT_PROTOCOL
	for ( i = 0; i < POINT_NUM; i++ )
	{
		input_mt_slot( icn85xx_ts->input_dev, i );
		input_mt_report_slot_state( icn85xx_ts->input_dev, MT_TOOL_FINGER, false );
	}
#else
	icn85xx_ts_release();
#endif

	if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86) )
	{
		while ( retry-- && !need_update_fw )
		{
			icn85xx_ts_reset();
			icn85xx_bootfrom_sram();
			msleep( 50 );
			ret = icn85xx_read_reg( 0xa, &value );
			if ( ret > 0 )
			{
				need_update_fw = false;
				break;
			}
		}
		if ( retry <= 0 )
			need_update_fw = true;

		if ( need_update_fw )
		{
			if ( R_OK == icn85xx_fw_update( firmware ) )
			{
				icn85xx_ts->code_loaded_flag = 1;
				icn85xx_trace( "ICN85XX_WITHOUT_FLASH, reload code ok\n" );
			}else  {
				icn85xx_ts->code_loaded_flag = 0;
				icn85xx_trace( "ICN85XX_WITHOUT_FLASH, reload code error\n" );
			}
		}
	}else if ( icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87 )
	{
		icn85xx_trace( "==icn85xx_ts_resume=6666666============= \n" );
		while ( retry-- && !need_update_fw )
		{
			/* icn85xx_ts_reset(); */
			icn85xx_set_prog_addr();
			/* icn85xx_write_reg(ICN85xx_REG_PMODE, 0xff); */
			icn87xx_boot_sram();
			msleep( 50 );
			ret = icn85xx_read_reg( 0xa, &value );
			if ( ret > 0 )
			{
				need_update_fw = false;
				break;
			}
		}
		icn85xx_trace( "==icn85xx_ts_resume=777777777= \n" );
		if ( retry <= 0 )
			need_update_fw = true;

		if ( need_update_fw )
		{
			if ( R_OK == icn87xx_fw_update( firmware ) )
			{
				icn85xx_ts->code_loaded_flag = 1;
				icn85xx_trace( "ICN87XX_WITHOUT_FLASH, reload code ok\n" );
			}else  {
				icn85xx_ts->code_loaded_flag = 0;
				icn85xx_trace( "ICN87XX_WITHOUT_FLASH, reload code error\n" );
			}
		}

#if CTP_CHARGER_DETECT

		batt_psy = power_supply_get_by_name( "usb" );
		if ( !batt_psy )
		{
			icn85xx_info( "battery supply is not found" );
		}else  {
			is_charger_status = (unsigned char) power_supply_is_system_supplied();

			icn85xx_info( "is_charger_status %d\n", is_charger_status );

			if ( is_charger_status )
			{
				icn85xx_write_reg( 0x04, 0x55 );
				icn85xx_info( "is_charger_status= %d\n", is_charger_status );
			}else  {
				icn85xx_write_reg( 0x04, 0x66 );
				icn85xx_info( "is_charger_status= %d\n", is_charger_status );
			}
		}


#endif

		icn85xx_clear_report_data( icn85xx_ts );
	}else  {
		icn85xx_write_reg( ICN85xx_REG_PMODE, 0xff );
		/* icn85xx_ts_reset(); */
	}

	if ( icn85xx_ts->use_irq )
	{
		icn85xx_irq_enable();
	}else  {
		hrtimer_start( &icn85xx_ts->timer, ktime_set( CTP_START_TIMER / 1000, (CTP_START_TIMER % 1000) * 1000000 ), HRTIMER_MODE_REL );
	}
	icn85xx_ts->work_mode = 0;

#ifdef TPD_PROXIMITY
	//printk( "icn85xx_ts_resume: icn85xx_proximity_flag=%d, tpd_halt=%d\n", icn85xx_proximity_flag, tpd_halt );
	if ( 1 == icn85xx_proximity_flag )
	{
		TP_face_mode_switch( 1 );
		//printk( "==icn85xx_ts_resume== by powerkey\n" );
		return;
	}
#endif


#if SUPPORT_CHECK_ESD
	icn85xx_ts->icn_is_suspend = 0;
	spin_lock(&icn85xx_ts->esd_lock);
	if (!icn85xx_ts->esd_running)
	{
		icn85xx_ts->esd_running = 1;
		spin_unlock(&icn85xx_ts->esd_lock);
		icn85xx_trace("Esd started");
		queue_delayed_work(icn85xx_esd_workqueue, &icn85xx_esd_event_work, icn85xx_ts->clk_tick_cnt);
	}
#endif

}


/* #endif */


/***********************************************************************************************
*  Name    :   icn85xx_request_io_port
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/


/*
 * static int icn85xx_request_io_port(struct icn85xx_ts_data *icn85xx_ts)
 * {
 *
 * #if SUPPORT_ROCKCHIP
 *  icn85xx_ts->screen_max_x = SCREEN_MAX_X;
 *  icn85xx_ts->screen_max_y = SCREEN_MAX_Y;
 *  icn85xx_ts->irq = gpio_to_irq(icn85xx_ts_int_gpio); //maybe need changed
 * //icn85xx_ts->rst_gpio = icn85xx_ts_rst_gpio; //maybe need changed
 * #endif
 * //printk("icn85xx_ts->irq = %d, icn85xx_ts->rst = %d\n", icn85xx_ts->irq,icn85xx_ts->rst);
 *  return ;
 *
 * }
 */


/***********************************************************************************************
*  Name    :   icn85xx_free_io_port
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/
static void icn85xx_free_io_port( struct icn85xx_ts_data *icn85xx_ts )
{
	return;
}


/***********************************************************************************************
*  Name    :   icn85xx_request_irq
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/
static int icn85xx_request_irq( struct icn85xx_ts_data *icn85xx_ts )
{
	int err = -1;

#if 1   /* SUPPORT_ROCKCHIP */
	icn85xx_error( "icn85xx_request_irq: gpio_to_irq0001 %d=\n", icn85xx_ts->irq_gpio );
	gpio_direction_input( icn85xx_ts->irq_gpio );
	icn85xx_ts->irq = gpio_to_irq( icn85xx_ts->irq_gpio );
	icn85xx_error( "icn85xx_request_irq: gpio_to_irq0002 %d=\n", icn85xx_ts->irq );

/*    gpio_direction_input(icn85xx_ts->irq); */
	err = request_irq( icn85xx_ts->irq, icn85xx_ts_interrupt, IRQ_TYPE_LEVEL_LOW
		#ifdef ICN_GESTURE
		|IRQF_ONESHOT|IRQF_NO_SUSPEND
		#endif
		, "icn85xx_ts", icn85xx_ts );
	if ( err < 0 )
	{
		icn85xx_ts->use_irq = 0;
		icn85xx_error( "icn85xx_ts_probe: request irq failed\n" );
		return(err);
	}else  {
		icn85xx_irq_disable();
		icn85xx_ts->use_irq = 1;
	}
#endif

	return(0);
}


/***********************************************************************************************
*  Name    :   icn85xx_free_irq
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/
static void icn85xx_free_irq( struct icn85xx_ts_data *icn85xx_ts )
{
	if ( icn85xx_ts )
	{
		if ( icn85xx_ts->use_irq )
		{
			free_irq( icn85xx_ts->irq, icn85xx_ts );
		}else  {
			hrtimer_cancel( &icn85xx_ts->timer );
		}
	}
}


/***********************************************************************************************
*  Name    :   icn85xx_request_input_dev
*  Input   :   void
*  Output  :
*  function    : 0 success,
***********************************************************************************************/
static int icn85xx_request_input_dev( struct icn85xx_ts_data *icn85xx_ts )
{
	int			ret = -1;
	struct input_dev	*input_dev;

	input_dev = input_allocate_device();
	if ( !input_dev )
	{
		icn85xx_error( "failed to allocate input device\n" );
		return(-ENOMEM);
	}
	icn85xx_ts->input_dev = input_dev;

	icn85xx_ts->input_dev->evbit[0] = BIT_MASK( EV_SYN ) | BIT_MASK( EV_KEY ) | BIT_MASK( EV_ABS );
#if CTP_REPORT_PROTOCOL
	/* #if (LINUX_VERSION_CODE > KERNEL_VERSION(3,6,0)) */
#if 1     /* (LINUX_VERSION_CODE > KERNEL_VERSION(3,6,0)) */
	__set_bit( INPUT_PROP_DIRECT, icn85xx_ts->input_dev->propbit );
	input_mt_init_slots( icn85xx_ts->input_dev, POINT_NUM, 0 );
#else
	input_mt_init_slots( icn85xx_ts->input_dev, POINT_NUM * 2 );
#endif
#else
	set_bit( ABS_MT_TOUCH_MAJOR, icn85xx_ts->input_dev->absbit );
	set_bit( ABS_MT_POSITION_X, icn85xx_ts->input_dev->absbit );
	set_bit( ABS_MT_POSITION_Y, icn85xx_ts->input_dev->absbit );
	set_bit( ABS_MT_WIDTH_MAJOR, icn85xx_ts->input_dev->absbit );
#endif
	input_set_abs_params( icn85xx_ts->input_dev, ABS_MT_POSITION_X, 0, icn85xx_ts->screen_max_x, 0, 0 );
	input_set_abs_params( icn85xx_ts->input_dev, ABS_MT_POSITION_Y, 0, icn85xx_ts->screen_max_y, 0, 0 );
	input_set_abs_params( icn85xx_ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0 );
	input_set_abs_params( icn85xx_ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0 );
	/* input_set_abs_params(icn85xx_ts->input_dev, ABS_MT_TRACKING_ID, 0, POINT_NUM*2, 0, 0); */

#ifdef TPD_PROXIMITY
       input_set_abs_params(input_dev, ABS_DISTANCE, 0, 1, 0, 0);
#endif	

#ifdef ICN_GESTURE
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_LEFT);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_RIGHT);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_UP);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_DOWN);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_O);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_W);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_M);
//		input_set_capability(input_dev, EV_KEY, KEY_A);
//		input_set_capability(input_dev, EV_KEY, KEY_B);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_L);
//		input_set_capability(input_dev, EV_KEY, KEY_G); 
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_E);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_C);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_V);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_S);
		input_set_capability(input_dev, EV_KEY, KEY_GESTURE_Z);
		input_set_capability(input_dev, EV_KEY, KEY_POWER);

	__set_bit(KEY_GESTURE_RIGHT, input_dev->keybit);
	__set_bit(KEY_GESTURE_LEFT, input_dev->keybit);
	__set_bit(KEY_GESTURE_UP, input_dev->keybit);
	__set_bit(KEY_GESTURE_DOWN, input_dev->keybit);
	__set_bit(KEY_GESTURE_U, input_dev->keybit);
	__set_bit(KEY_GESTURE_O, input_dev->keybit);
	__set_bit(KEY_GESTURE_E, input_dev->keybit);
	__set_bit(KEY_GESTURE_C, input_dev->keybit);
	__set_bit(KEY_GESTURE_M, input_dev->keybit);
	__set_bit(KEY_GESTURE_W, input_dev->keybit);
	__set_bit(KEY_GESTURE_L, input_dev->keybit);
	__set_bit(KEY_GESTURE_S, input_dev->keybit);
	__set_bit(KEY_GESTURE_V, input_dev->keybit);
	__set_bit(KEY_GESTURE_Z, input_dev->keybit);

#endif	
	
	__set_bit( KEY_MENU, input_dev->keybit );
	__set_bit( KEY_BACK, input_dev->keybit );
	__set_bit( KEY_HOME, input_dev->keybit );
	__set_bit( KEY_HOMEPAGE, input_dev->keybit );
	__set_bit( KEY_SEARCH, input_dev->keybit );

#ifdef TPD_PROXIMITY
	__set_bit(ABS_DISTANCE, input_dev->absbit);
#endif

#ifdef TPD_PROXIMITY
       input_set_abs_params(input_dev, ABS_DISTANCE, 0, 1, 0, 0);
        
             
           Sysfs_icn85xx_facemode_init();
#endif	
	input_dev->name = "ctp_dev";
	ret		= input_register_device( input_dev );
	if ( ret )
	{
		icn85xx_error( "Register %s input device failed\n", input_dev->name );
		input_free_device( input_dev );
		return(-ENODEV);
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	icn85xx_trace( "==register_early_suspend =\n" );
	icn85xx_ts->early_suspend.level		= EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	icn85xx_ts->early_suspend.suspend	= icn85xx_ts_suspend;
	icn85xx_ts->early_suspend.resume	= icn85xx_ts_resume;
	register_early_suspend( &icn85xx_ts->early_suspend );
#endif
	return(0);
}


#if SUPPORT_SENSOR_ID
static void read_sensor_id( void )
{
	int i, ret;
	/* icn85xx_trace("scan sensor id value begin sensor_id_num = %d\n",(sizeof(sensor_id_table)/sizeof(sensor_id_table[0]))); */
	ret = icn85xx_read_reg( 0x10, &cursensor_id );
	if ( ret > 0 )
	{
		icn85xx_trace( "cursensor_id= 0x%x\n", cursensor_id );
	}else         {
		icn85xx_error( "icn85xx read cursensor_id failed.\n" );
		cursensor_id = -1;
	}

	ret = icn85xx_read_reg( 0x1e, &tarsensor_id );
	if ( ret > 0 )
	{
		icn85xx_trace( "tarsensor_id= 0x%x\n", tarsensor_id );
		tarsensor_id = -1;
	}else         {
		icn85xx_error( "icn85xx read tarsensor_id failed.\n" );
	}
	ret = icn85xx_read_reg( 0x1f, &id_match );
	if ( ret > 0 )
	{
		icn85xx_trace( "match_flag= 0x%x\n", id_match ); /* 1: match; 0:not match */
	}else      {
		icn85xx_error( "icn85xx read id_match failed.\n" );
		id_match = -1;
	}
	/* scan sensor id value */
	icn85xx_trace( "begin to scan id table,find correct fw or bin. sensor_id_num = %d\n", (sizeof(sensor_id_table) / sizeof(sensor_id_table[0]) ) );

	for ( i = 0; i < (sizeof(sensor_id_table) / sizeof(sensor_id_table[0]) ); i++ ) /* not change tp */
	{
		if ( cursensor_id == sensor_id_table[i].value )
		{
#if COMPILE_FW_WITH_DRIVER
			icn85xx_set_fw( sensor_id_table[i].size, sensor_id_table[i].fw_name );
#else
			strcpy( firmware, sensor_id_table[i].bin_name );
			icn85xx_trace( "icn85xx matched firmware = %s\n", firmware );
#endif
			icn85xx_trace( "icn85xx matched id = 0x%x\n", sensor_id_table[i].value );
			invalid_id = 1;
			break;
		}else         {
			invalid_id = 0;
			icn85xx_trace( "icn85xx not matched id%d= 0x%x\n", i, sensor_id_table[i].value );
			/* icn85xx_trace("not match sensor_id_table[%d].value= 0x%x,bin_name = %s\n",i,sensor_id_table[i].value,sensor_id_table[i].bin_name); */
		}
	}
}


static void compare_sensor_id( void )
{
	int retry = 5;

	read_sensor_id();       /* select sensor id */

	if ( 0 == invalid_id )  /* not compare sensor id,update default fw or bin */
	{
		icn85xx_trace( "not compare sensor id table,update default: invalid_id= %d, cursensor_id= %d\n", invalid_id, cursensor_id );
#if COMPILE_FW_WITH_DRIVER
		icn85xx_set_fw( sensor_id_table[0].size, sensor_id_table[0].fw_name );
#else
		strcpy( firmware, sensor_id_table[0].bin_name );
		icn85xx_trace( "match default firmware = %s\n", firmware );
#endif

		while ( retry > 0 )
		{
			if ( R_OK == icn85xx_fw_update( firmware ) )
			{
				icn85xx_trace( "icn85xx upgrade default firmware ok\n" );
				break;
			}
			retry--;
			icn85xx_error( "icn85xx_fw_update default firmware failed.\n" );
		}
	}

	if ( (1 == invalid_id) && (0 == id_match) ) /* tp is changed,update current fw or bin */
	{
		icn85xx_trace( "icn85xx detect tp is changed!!! invalid_id= %d,id_match= %d,\n", invalid_id, id_match );
		while ( retry > 0 )
		{
			if ( R_OK == icn85xx_fw_update( firmware ) )
			{
				icn85xx_trace( "icn85xx upgrade cursensor id firmware ok\n" );
				break;
			}
			retry--;
			icn85xx_error( "icn85xx_fw_update current id firmware failed.\n" );
		}
	}
}


#endif

static void icn85xx_update( struct icn85xx_ts_data *icn85xx_ts )
{
	short	fwVersion	= 0;
	short	curVersion	= 0;
	int	retry		= 0;

	if ( (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_86) )
	{
#if (COMPILE_FW_WITH_DRIVER && !SUPPORT_SENSOR_ID)
		icn85xx_set_fw( sizeof(icn85xx_fw), &icn85xx_fw[0] );
#endif

#if SUPPORT_SENSOR_ID
		while ( 0 == invalid_id )       /* reselect sensor id */
		{
			compare_sensor_id();    /* select sensor id */
			icn85xx_trace( "invalid_id= %d\n", invalid_id );
		}
#else
		if ( R_OK == icn85xx_fw_update( firmware ) )
		{
			icn85xx_ts->code_loaded_flag = 1;
			icn85xx_trace( "ICN85XX_WITHOUT_FLASH, update default fw ok\n" );
		}else  {
			icn85xx_ts->code_loaded_flag = 0;
			icn85xx_trace( "ICN85XX_WITHOUT_FLASH, update error\n" );
		}
#endif
	}else if ( (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_85) || (icn85xx_ts->ictype == ICN85XX_WITH_FLASH_86) )
	{
#if (COMPILE_FW_WITH_DRIVER && !SUPPORT_SENSOR_ID)
		icn85xx_set_fw( sizeof(icn85xx_fw), &icn85xx_fw[0] );
#endif

#if SUPPORT_SENSOR_ID
		while ( 0 == invalid_id )       /* reselect sensor id */
		{
			compare_sensor_id();    /* select sensor id */
			if ( 1 == invalid_id )
			{
				icn85xx_trace( "select sensor id ok. begin compare fwVersion with curversion\n" );
			}
		}
#endif

		fwVersion	= icn85xx_read_fw_Ver( firmware );
		g_fwVersion	= icn85xx_read_fw_Ver( firmware ); // add  by  zhouhua 
		curVersion	= icn85xx_readVersion();
		icn85xx_trace( "fwVersion : 0x%x\n", fwVersion );
		icn85xx_trace( "current version: 0x%x\n", curVersion );

#if FORCE_UPDATA_FW
		retry = 5;
		while ( retry > 0 )
		{
			if ( icn85xx_goto_progmode() != 0 )
			{
				//printk( "icn85xx_goto_progmode() != 0 error\n" );
				return(-1);
			}
			icn85xx_read_flashid();
			if ( R_OK == icn85xx_fw_update( firmware ) )
			{
				break;
			}
			retry--;
			icn85xx_error( "icn85xx_fw_update failed.\n" );
		}

#else
		if ( fwVersion > curVersion )
		{
			retry = 5;
			while ( retry > 0 )
			{
				if ( R_OK == icn85xx_fw_update( firmware ) )
				{
					break;
				}
				retry--;
				icn85xx_error( "icn85xx_fw_update failed.\n" );
			}
		}
#endif
	}else if ( icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87 )
	{
		icn85xx_trace( "icn85xx_update  87 without flash\n" );


#if (COMPILE_FW_WITH_DRIVER && !SUPPORT_SENSOR_ID)
		icn85xx_set_fw( sizeof(icn85xx_fw), &icn85xx_fw[0] );
#endif

		fwVersion	= icn85xx_read_fw_Ver( firmware );
		curVersion	= icn85xx_readVersion();
		icn85xx_trace( "fwVersion : 0x%x\n", fwVersion );
		icn85xx_trace( "current version: 0x%x\n", curVersion );

		if ( R_OK == icn87xx_fw_update( firmware ) )
		{
			icn85xx_ts->code_loaded_flag = 1;
			//printk( "ICN87XX_WITHOUT_FLASH, update default fw ok\n" );
		}else  {
			icn85xx_ts->code_loaded_flag = 0;
			//printk( "ICN87XX_WITHOUT_FLASH, update error\n" );
		}
	}else if ( icn85xx_ts->ictype == ICN85XX_WITH_FLASH_87 )
	{
		//printk( "icn85xx_update 87 with flash\n" );

#if (COMPILE_FW_WITH_DRIVER && !SUPPORT_SENSOR_ID)
		icn85xx_set_fw( sizeof(icn85xx_fw), &icn85xx_fw[0] );
#endif

		fwVersion	= icn85xx_read_fw_Ver( firmware );
		curVersion	= icn85xx_readVersion();
		icn85xx_trace( "fwVersion : 0x%x\n", fwVersion );
		icn85xx_trace( "current version: 0x%x\n", curVersion );


#if FORCE_UPDATA_FW
		if ( R_OK == icn87xx_fw_update( firmware ) )
		{
			icn85xx_ts->code_loaded_flag = 1;
			icn85xx_trace( "ICN87XX_WITH_FLASH, update default fw ok\n" );
		}else  {
			icn85xx_ts->code_loaded_flag = 0;
			icn85xx_trace( "ICN87XX_WITH_FLASH, update error\n" );
		}

#else
		if ( fwVersion > curVersion )
		{
			retry = 5;
			while ( retry > 0 )
			{
				if ( R_OK == icn87xx_fw_update( firmware ) )
				{
					break;
				}
				retry--;
				icn85xx_error( "icn87xx_fw_update failed.\n" );
			}
		}
#endif
	}
}


#ifdef CONFIG_OF
static struct icn85xx_ts_platform_data *icn85xx_ts_parse_dt( struct device *dev )
{
	struct icn85xx_ts_platform_data *pdata;
	struct device_node		*np = dev->of_node;
	int				ret;

	pdata = kzalloc( sizeof(*pdata), GFP_KERNEL );
	if ( !pdata )
	{
		dev_err( dev, "Could not allocate struct icn85xx_ts_platform_data" );
		return(NULL);
	}
	pdata->reset_gpio_number = of_get_gpio( np, 0 );
	if ( pdata->reset_gpio_number < 0 )
	{
		dev_err( dev, "fail to get reset_gpio_number\n" );
		goto fail;
	}
	pdata->irq_gpio_number = of_get_gpio( np, 1 );
	if ( pdata->reset_gpio_number < 0 )
	{
		dev_err( dev, "fail to get reset_gpio_number\n" );
		goto fail;
	}
	ret = of_property_read_string( np, "vdd_name", &pdata->vdd_name );
	if ( ret )
	{
		dev_err( dev, "fail to get vdd_name\n" );
		goto fail;
	}
	ret = of_property_read_u32_array( np, "virtualkeys", &pdata->virtualkeys, 12 );
	if ( ret )
	{
		dev_err( dev, "fail to get virtualkeys\n" );
		goto fail;
	}
	ret = of_property_read_u32( np, "TP_MAX_X", &pdata->TP_MAX_X );
	if ( ret )
	{
		dev_err( dev, "fail to get TP_MAX_X\n" );
		goto fail;
	}
	ret = of_property_read_u32( np, "TP_MAX_Y", &pdata->TP_MAX_Y );
	if ( ret )
	{
		dev_err( dev, "fail to get TP_MAX_Y\n" );
		goto fail;
	}

	return(pdata);
fail:
	kfree( pdata );
	return(NULL);
}


#endif



#if SUPPORT_CHECK_ESD
static void icn85xx_esd_check_work(struct work_struct *work)
{
    int err,ret;
    u8 value;
    struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata(this_client);
    int retry = 3;
    int need_update_fw = false;
	
    icn85xx_info("icn85xx_esd_check_work enter \n");

	if ((icn85xx_ts->icn_is_suspend)||(icn85xx_ts->is_apk_update == 1)) 
	{
		icn85xx_trace("icn85xx Esd suspended or in APK update!");
		return;
	}
	
   if(icn85xx_ts->ictype == ICN85XX_WITHOUT_FLASH_87)
    {
        while (retry-- && !need_update_fw) 
        {
            msleep(1);
            ret = icn85xx_read_reg(0xa, &value);
             if ((ret > 0) && (value == 0x87)) 
            { 
                need_update_fw = false;
                break; 
            }
        }
        if (retry <= 0) need_update_fw = true;
        
		retry = 3;
        while (retry-- && need_update_fw) 
        {
			icn85xx_set_prog_addr();
			icn87xx_boot_sram();
			msleep(50);
            ret = icn85xx_read_reg(0xa, &value);
	        if ((ret > 0) && (value == 0x87)) 
	          { 
	                need_update_fw = true;
	                break; 
	          }
        }  
		if (retry <= 0) 
		{
		   if(R_OK == icn87xx_fw_update(firmware))
            {
                icn85xx_ts->code_loaded_flag = 1;
                icn85xx_trace("ICN87XX_WITHOUT_FLASH, reload code ok\n"); 
            }
            else
            {
                icn85xx_ts->code_loaded_flag = 0;
                icn85xx_trace("ICN87XX_WITHOUT_FLASH, reload code error\n"); 
            }
		}         
    }
    if(!icn85xx_ts->icn_is_suspend) 
	{
        queue_delayed_work(icn85xx_esd_workqueue, &icn85xx_esd_event_work, icn85xx_ts->clk_tick_cnt);
    } 
	else
    {
    	icn85xx_trace("Esd suspended!");
    }
        return;

}
#endif


static int icn85xx_ts_probe( struct i2c_client *client, const struct i2c_device_id *id )
{
	struct icn85xx_ts_platform_data *pdata = client->dev.platform_data;

	struct icn85xx_ts_data	*icn85xx_ts;
	struct i2c_dev		*i2c_dev;
	struct device		*dev;
/*	struct device_node *np; */
	enum of_gpio_flags	rst_flags;
	unsigned long		irq_flags;
	int			err	= 0;
	int			ret	= 0;
	u32			val;
	icn85xx_trace( "8051====%s begin=====.\n", __func__ );

#ifdef CONFIG_OF

	struct regulator	*reg_vdd;
	struct device_node	*np = client->dev.of_node;
#if 1
	if ( np && !pdata )
	{
		pdata = icn85xx_ts_parse_dt( &client->dev );
		if ( pdata )
		{
			client->dev.platform_data = pdata;
		}else  {
			err = -ENOMEM;
			goto exit_alloc_platform_data_failed;
		}
	}
#endif

#endif

	icn85xx_trace( "====%s begin=====001.\n", __func__ );

	if ( !i2c_check_functionality( client->adapter, I2C_FUNC_I2C ) )
	{
		icn85xx_error( "I2C check functionality failed.\n" );
		return(-ENODEV);
	}

	icn85xx_ts = kzalloc( sizeof(*icn85xx_ts), GFP_KERNEL );
	if ( !icn85xx_ts )
	{
		icn85xx_error( "Alloc icn85xx_ts memory failed.\n" );
		return(-ENOMEM);
	}
	memset( icn85xx_ts, 0, sizeof(*icn85xx_ts) );
/* =================================================================== */

	icn85xx_ts->irq_gpio	= pdata->irq_gpio_number;
	icn85xx_ts->rst_gpio	= pdata->reset_gpio_number;
	icn85xx_ts->vdd_name	= pdata->vdd_name;
/* icn85xx_ts->virtualkeys	=	pdata->virtualkeys; */
	icn85xx_ts->screen_max_x	= SCREEN_MAX_X;  /* pdata->TP_MAX_X; */
	icn85xx_ts->screen_max_y	= SCREEN_MAX_Y;  /* pdata->TP_MAX_Y; */

/* =================================================================== */
	this_client		= client;
	this_client->addr	= client->addr;
	icn85xx_trace( "====%s begin=====002====0x%x\n", __func__, client->addr );

	i2c_set_clientdata( client, icn85xx_ts );
	icn85xx_ts->client = client;
/*
 * dev = &icn85xx_ts->client->dev;
 * np = dev->of_node;
 */
	icn85xx_trace( "====%s begin=====002.\n", __func__ );

/*
 * if (!np) {
 *      dev_err(&client->dev, "no device tree\n");
 *      return -EINVAL;
 * }
 */

	icn85xx_ts->work_mode = 0;
	spin_lock_init( &icn85xx_ts->irq_lock );
	/* icn85xx_ts->irq_lock = SPIN_LOCK_UNLOCKED; */

/*
 *   icn85xx_ts->irq_gpio = of_get_named_gpio_flags(np, "touch-gpio", 0, (enum of_gpio_flags *)&irq_flags);
 *  icn85xx_ts->rst_gpio = of_get_named_gpio_flags(np, "rst-gpio", 0, &rst_flags);
 */

	if ( gpio_is_valid( icn85xx_ts->rst_gpio ) )
	{
		ret = gpio_request( icn85xx_ts->rst_gpio, "icn87xx_ts_rst" );
		if ( ret < 0 )
			printk( "icn87xx_ts_rst request fail!\n" );
		else
			gpio_direction_output( icn85xx_ts->rst_gpio, 1 );
	}

	if ( gpio_is_valid( icn85xx_ts->irq_gpio ) )
	{
		ret = gpio_request( icn85xx_ts->irq_gpio, "icn87xx_ts_irq" );
		if ( ret < 0 )
			printk( "icn87xx_ts_irq request fail!\n" );
		/*
		 * else
		 * gpio_direction_output(icn85xx_ts->rst_gpio, 1);
		 */
	}
	icn85xx_trace( "icn85xx irq_gpio=%d -- rst_gpio=%d\n", icn85xx_ts->irq_gpio, icn85xx_ts->rst_gpio );

#ifdef CONFIG_I2C_SPRD
	sprd_i2c_ctl_chg_clk( client->adapter->nr, 400000 );
#endif


#ifdef CONFIG_OF
	reg_vdd = regulator_get( &client->dev, icn85xx_ts->vdd_name );
	if ( !WARN( IS_ERR( reg_vdd ), "[FST] icn85xx_ts_hw_init regulator: failed to get %s.\n", icn85xx_ts->vdd_name ) )
	{
		regulator_set_voltage( reg_vdd, 2800000, 2800000 );
		regulator_enable( reg_vdd );
	}
	msleep( 200 );
#endif

	icn85xx_trace( "====%s begin=====003.\n", __func__ );

	icn85xx_ts_reset();

	err = icn85xx_iic_test();
	 g_chip_type = icn85xx_ts->ictype;
	if ( err <= 0 )
	{
		icn85xx_error( "icn85xx_iic_test  failed.\n" );
		return(-1);
	}else  {
		icn85xx_trace( "iic communication ok: 0x%x\n", icn85xx_ts->ictype );
	}
	icn85xx_trace( "====%s begin=====004.\n", __func__ );

	icn85xx_update( icn85xx_ts );
	
#if TOUCH_VIRTUAL_KEYS
	icn85xx_ts_virtual_keys_init();
	icn85xx_trace( "====%s begin=====XXXXXXXXXXXXXXXXXXXXXXXXX.\n", __func__ );
#endif

#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //houdaijun 160707 add
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
#endif // Endif 	

	err = icn85xx_request_input_dev( icn85xx_ts );
	if ( err < 0 )
	{
		icn85xx_error( "request input dev failed\n" );
		kfree( icn85xx_ts );
		return(err);
	}
	icn85xx_trace( "====%s begin=====005.\n", __func__ );

	err = icn85xx_request_irq( icn85xx_ts );
	if ( err != 0 )
	{
		icn85xx_error( "request irq error, use timer\n" );
		icn85xx_ts->use_irq = 0;
		hrtimer_init( &icn85xx_ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
		icn85xx_ts->timer.function = chipone_timer_func;
		hrtimer_start( &icn85xx_ts->timer, ktime_set( CTP_START_TIMER / 1000, (CTP_START_TIMER % 1000) * 1000000 ), HRTIMER_MODE_REL );
	}
#if SUPPORT_SYSFS
	/* icn85xx_create_sysfs(client); */
#endif

#if SUPPORT_PROC_FS
	sema_init( &icn85xx_ts->sem, 1 );
	init_proc_node();
#endif
/* zjw    fb_register_client(&icn85xx_ts_fb_notifier); */
	INIT_WORK( &icn85xx_ts->pen_event_work, icn85xx_ts_pen_irq_work );
	icn85xx_ts->ts_workqueue = create_singlethread_workqueue( dev_name( &client->dev ) );
	if ( !icn85xx_ts->ts_workqueue )
	{
		icn85xx_error( "create_singlethread_workqueue failed.\n" );
		kfree( icn85xx_ts );
		return(-ESRCH);
	}

#if CFG_ICN_DETECT_UP_EVENT
	icn85xx_trace( "%s: add up event timer \n", __func__ );
	init_timer( &_st_up_evnet_timer );
	_st_up_evnet_timer.function	= icn85xx_force_read_up_event; /* ft5x06_force_read_up_event; */
	_st_up_evnet_timer.data		= 1;
	_sui_last_point_cnt		= 0;
	mdelay( 10 );
	/* #endi */
#endif

#ifdef TPD_PROXIMITY

	ps_input_dev = input_allocate_device();
	if ( !ps_input_dev )
	{
		err = -ENOMEM;
	//	printk( "%s() Line: %d, input_allocate_device failed, err = %d.\n", __FUNCTION__, __LINE__, err );

		goto exit_input_register_device_failed;
	}

	ps_input_dev->name = "ctp_dev";
	set_bit( EV_ABS, ps_input_dev->evbit );
	input_set_abs_params( ps_input_dev, ABS_DISTANCE, 0, 1, 0, 0 );
	err = input_register_device( ps_input_dev );
	if ( err )
	{
		goto exit_input_register_device_failed;
	}
	err = misc_register( &icn85xx_ps_device );
	if ( err )
	{
	//	printk( "%s: icn85xx_ps_device register failed\n", __func__ );
		goto exit_input_register_device_failed;
	}
#endif

#if CTP_CHARGER_DETECT

	batt_psy = power_supply_get_by_name( "usb" );
	if ( !batt_psy )
	{
		icn85xx_info( "battery supply is not found  probe" );
	}
#endif

	if ( icn85xx_ts->use_irq )
	{
		icn85xx_irq_enable();
		icn85xx_trace( "====%s begin=====006.\n", __func__ );

#if SUPPORT_CHECK_ESD
	INIT_DELAYED_WORK(&icn85xx_esd_event_work, icn85xx_esd_check_work);
	icn85xx_esd_workqueue = create_workqueue("icn85xx_esd");

	icn85xx_ts->clk_tick_cnt = 2 * HZ;      // HZ: clock ticks in 1 second generated by system
	icn85xx_trace("Clock ticks for an esd cycle: %d", icn85xx_ts->clk_tick_cnt);
	spin_lock_init(&icn85xx_ts->esd_lock);

	spin_lock(&icn85xx_ts->esd_lock);
	if (!icn85xx_ts->esd_running)
    {
		icn85xx_ts->esd_running = 1;
		spin_unlock(&icn85xx_ts->esd_lock);
		icn85xx_trace("Esd started");
		queue_delayed_work(icn85xx_esd_workqueue, &icn85xx_esd_event_work, icn85xx_ts->clk_tick_cnt);
	}
#endif

	}
	icn85xx_trace( "==%s over =\n", __func__ );
	return(0);
#ifdef CONFIG_GST_TP_GESTURE_SUPPORT  // houdaijun 060701 add
exit_fail_create_gesture_device_file:
    device_remove_file(tp_gesture_dev, &dev_attr_enable);

exit_fail_create_gesture_device:
    device_destroy(tp_gesture_class, tp_gesture_dev);
    tp_gesture_dev = NULL;

exit_fail_create_gesture_class:
    class_destroy(tp_gesture_class);
    tp_gesture_class = NULL;
#endif // Endif CONFIG_GST_TP_GESTURE_SUPPORT
#ifdef TPD_PROXIMITY  /* luojy */
exit_input_register_device_failed:
	input_free_device( ps_input_dev );
#endif
exit_alloc_platform_data_failed:
	return(-ESRCH);
}


static int icn85xx_ts_remove( struct i2c_client *client )
{
	struct icn85xx_ts_data *icn85xx_ts = i2c_get_clientdata( client );
	icn85xx_trace( "==icn85xx_ts_remove=\n" );
	if ( icn85xx_ts->use_irq )
		icn85xx_irq_disable();
	gpio_free( icn85xx_ts->rst_gpio );
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend( &icn85xx_ts->early_suspend );
#endif

#if SUPPORT_PROC_FS
	uninit_proc_node();
#endif

#if SUPPORT_SYSFS
	icn85xx_remove_sysfs( client );
#endif

#if SUPPORT_CHECK_ESD
    destroy_workqueue(icn85xx_esd_workqueue);
#endif

	input_unregister_device( icn85xx_ts->input_dev );
	input_free_device( icn85xx_ts->input_dev );
/* zjw	fb_unregister_client(&icn85xx_ts_fb_notifier); */
	cancel_work_sync( &icn85xx_ts->pen_event_work );
	destroy_workqueue( icn85xx_ts->ts_workqueue );
	icn85xx_free_irq( icn85xx_ts );
	icn85xx_free_io_port( icn85xx_ts );
	kfree( icn85xx_ts );
	i2c_set_clientdata( client, NULL );


	return(0);
}


static const struct i2c_device_id icn85xx_ts_id[] = {
	{ CTP_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE( i2c, icn85xx_ts_id );


static const struct of_device_id icn85xx_of_match[] = {
	{ .compatible = "icn85xx,ctp_dev", },
	{ }
};
MODULE_DEVICE_TABLE( of, icn85xx_of_match );
static struct i2c_driver icn85xx_ts_driver = {
	.class	= I2C_CLASS_HWMON,
	.probe	= icn85xx_ts_probe,
	.remove = icn85xx_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= icn85xx_ts_suspend,
	.resume		= icn85xx_ts_resume,
#endif
	.id_table	= icn85xx_ts_id,
	.driver		= {
		.name		= CTP_NAME,
		.owner		= THIS_MODULE,
		.of_match_table = icn85xx_of_match,
#if SUPPORT_SYSFS
		.groups		= icn_drv_grp,
#endif
	},
};


static int __init icn85xx_ts_init( void )
{
	int ret = -1;
	icn85xx_trace( "===========================%s=====================\n", __func__ );


	ret = i2c_add_driver( &icn85xx_ts_driver );
	return(ret);
}


static void __exit icn85xx_ts_exit( void )
{
	icn85xx_trace( "==icn85xx_ts_exit==\n" );
	i2c_del_driver( &icn85xx_ts_driver );
}


late_initcall( icn85xx_ts_init );
/* module_init(icn85xx_ts_init); */
module_exit( icn85xx_ts_exit );

MODULE_AUTHOR( "<zmtian@chiponeic.com>" );
MODULE_DESCRIPTION( "Chipone icn85xx TouchScreen driver" );
MODULE_LICENSE( "GPL" );
