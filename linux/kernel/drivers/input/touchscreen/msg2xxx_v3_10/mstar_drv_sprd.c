////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2014 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (??MStar Confidential Information??) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/**
 *
 * @file    mstar_drv_sprd.c
 *
 * @brief   This file defines the interface of touch screen
 *
 *
 */

/*=============================================================*/
// INCLUDE FILE
/*=============================================================*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif //CONFIG_HAS_EARLYSUSPEND
#include <linux/i2c.h>
#include <linux/kobject.h>
#include <asm/irq.h>
#include <asm/io.h>

#include "mstar_drv_platform_interface.h"

#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
//#include <mach/regulator.h>
#include <soc/sprd/regulator.h>
#include <linux/regulator/consumer.h>
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON

#if 1
#include "mstar_drv_platform_porting_layer.h" 
int TOUCH_SCREEN_X_MAX = 0;
int TOUCH_SCREEN_Y_MAX = 0;
int MS_TS_MSG_IC_GPIO_RST = 0;
int MS_TS_MSG_IC_GPIO_INT = 0;
#endif

#ifdef TP_PROXIMITY_SENSOR 
extern u32 SLAVE_I2C_ID_DWI2C;

int PROXIMITY_SWITCH;
int PROXIMITY_STATE;

struct spinlock proximity_switch_lock;
struct spinlock proximity_state_lock;
#include "mstar_drv_utility_adaption.h"
#endif
/*=============================================================*/
// CONSTANT VALUE DEFINITION
/*=============================================================*/

#define MSG_TP_IC_NAME "msg2xxx_ts" //"msg21xxA" or "msg22xx" or "msg26xxM" or "msg28xx"
/* Please define the mstar touch ic name based on the mutual-capacitive ic or self capacitive 
 * ic that you are using */

/*=============================================================*/
// VARIABLE DEFINITION
/*=============================================================*/

struct i2c_client *g_I2cClient = NULL;

#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
struct regulator *g_ReguVdd = NULL;
struct regulator *g_ReguVcc_i2c = NULL;
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON

/*=============================================================*/
// FUNCTION DEFINITION
/*=============================================================*/

struct msg_ts_platform_data{
	int irq_gpio_number;
	int reset_gpio_number;
	const char *vdd_name;
	int virtualkeys[12];
	int TP_MAX_X;
	int TP_MAX_Y;
};
#ifdef CONFIG_OF
static struct msg_ts_platform_data *msg2133_ts_parse_dt(struct device *dev)
{
	struct msg_ts_platform_data *pdata;
	struct device_node *np = dev->of_node;
	int ret;

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
	if(pdata->reset_gpio_number < 0){
		dev_err(dev, "fail to get reset_gpio_number\n");
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
	if(ret){
		dev_err(dev, "fail to get TP_MAX_X\n");
		goto fail;
	}
	ret = of_property_read_u32(np, "TP_MAX_Y", &pdata->TP_MAX_Y);
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
#if 1 
extern u8 g_ChipType;
extern u8 *_gFwVersion;
extern void DrvIcFwLyrGetCustomerFirmwareVersion(u16 *pMajor, u16 *pMinor, u8 **ppVersion);

static ssize_t tp_information_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	char *p_chip_type=NULL;
	u16 nMajor = 0, nMinor = 0;

	DrvIcFwLyrGetCustomerFirmwareVersion(&nMajor, &nMinor, &_gFwVersion);	
	if(g_ChipType==CHIP_TYPE_MSG21XX)
	{
		p_chip_type="MSG21XX";
	}
	else if(g_ChipType==CHIP_TYPE_MSG21XXA)
	{
		p_chip_type="MSG21XXA";
	}
	else if(g_ChipType==CHIP_TYPE_MSG26XXM)
	{
		p_chip_type="MSG26XXM";
	}
	else if(g_ChipType==CHIP_TYPE_MSG22XX)
	{
		p_chip_type="MSG22XX";
	}
	else
	{
		p_chip_type="NOT SUPPORT";
	}
        return sprintf(buf,"IC: %s\nFirmware version:%s\n",p_chip_type,_gFwVersion);
}


static struct kobj_attribute tp_information = {
    .attr = {
        .name = "tp_information",
        .mode = S_IRUGO,
    },
    .show  = &tp_information_show,
    .store = NULL,
};
#endif
#ifdef TP_PROXIMITY_SENSOR
static int TP_face_get_mode(void)
{
    DBG(&g_I2cClient->dev,">>>> PROXIMITY_SWITCH is %d <<<<\n", PROXIMITY_SWITCH);

    return PROXIMITY_SWITCH;
}

static int TP_face_mode_state(void)
{
    DBG(&g_I2cClient->dev,">>>> PROXIMITY_STATE  is %d <<<<\n", PROXIMITY_STATE);

    return PROXIMITY_STATE;
}

/*static*/ int Mstar_TP_face_mode_switch(int on)
{
	u8 data[4];


	data[0] = 0x52;
	data[1] = 0x00; // 0x01
	data[2] = 0x4A; // 0x24
	data[3] = 0xA0; // 0x52, 0x01, 0x24, 0xA0


	DBG(&g_I2cClient->dev,"***%s(), begin, on is 0x%.2X, PROXIMITY_SWITCH is 0x%.2X, PROXIMITY_STATE is %d <<<<\n", __func__, on, PROXIMITY_SWITCH, PROXIMITY_STATE);

	spin_lock(&proximity_switch_lock);

	if ((1 == on) && (0 == PROXIMITY_SWITCH))
	{
		PROXIMITY_SWITCH = 1;


		data[3] = 0xA0;


		goto OUT;
	}
	else if ((0 == on) && (1 == PROXIMITY_SWITCH))
	{
		PROXIMITY_SWITCH = 0;
		PROXIMITY_STATE  = 0;


		data[3] = 0xA1; // 0x01?

		goto OUT;
	}
	else
	{
		spin_unlock(&proximity_switch_lock);

		DBG(&g_I2cClient->dev,"***%s(), do nothing, on is %d, PROXIMITY_SWITCH is %d, PROXIMITY_STATE is %d <<<<\n", __func__, on, PROXIMITY_SWITCH, PROXIMITY_STATE);

		return -EINVAL;
	}

OUT:
	spin_unlock(&proximity_switch_lock);

	DBG(&g_I2cClient->dev,"*** %s(), end, on is %d, PROXIMITY_SWITCH is %d, PROXIMITY_STATE is %d <<<<\n", __func__, on, PROXIMITY_SWITCH, PROXIMITY_STATE); 
	//return ((this_client) ? (i2c_master_send(this_client, &data[0], 4)) : (-ENODEV));
	return ((g_I2cClient) ? (IicWriteData(SLAVE_I2C_ID_DWI2C, &data[0], 4)) : (-ENODEV));
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
            Mstar_TP_face_mode_switch(0); // Disable proximity;

            break;
        }
        case 1:
        {
            Mstar_TP_face_mode_switch(1); // Enable proximity;

            break;
        }

        default:
            return -EINVAL;
    }

	return count;
}

static struct kobj_attribute tp_face_mode_attr = {
    .attr = {
        .name = "facemode",     // Path: /sys/board_properties/facemode;
        .mode = S_IRUGO | S_IWUGO,
    },
    .show  = &tp_face_mode_show, // For read the proximity state by upper level;
    .store = &tp_face_mode_store, // For open/close the proximity_switch switch by upper level;
};
#endif
#if VIRTUAL_KEYS
static ssize_t virtual_keys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
#if defined(CONFIG_DJT_PRJ_V131_DJT_001)
	   return sprintf(buf,
			__stringify(EV_KEY) ":" __stringify(KEY_HOMEPAGE) ":390:850:15:20"  ///90
			":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":330:850:15:20"  ///150
			":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":455:850:15:5" ///25
			//":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":220:%d:15:5"
			"\n");
#elif defined(CONFIG_DJT_PRJ_V110_JTV_011)
	   return sprintf(buf,
			__stringify(EV_KEY) ":" __stringify(KEY_HOMEPAGE) ":90:850:15:20"  ///90
			":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":25:850:15:20"  ///150
			":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":150:850:15:5" ///25
			//":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":220:%d:15:5"
			"\n");
#else
		return sprintf(buf, 				   
				   __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":25:904:50:50"
			   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOMEPAGE) ":90:904:50:50"
			   ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":150:904:60:50"
			   "\n");	
    #endif
}

static struct kobj_attribute virtual_keys_attr = {
    .attr = {
        .name = "virtualkeys.msg2133_ts", //virtualkeys.msg2133
        .mode = S_IRUGO,
    },
    .show = &virtual_keys_show
};

static struct attribute *properties_attrs[] = {
    &tp_information.attr,
    &virtual_keys_attr.attr,
#ifdef TP_PROXIMITY_SENSOR
    &tp_face_mode_attr.attr,
#endif    
    NULL
};

static struct attribute_group properties_attr_group = {
    .attrs = properties_attrs,
};

static struct kobject *properties_kobj = NULL;

/*static*/ void virtual_keys_init(void) //huafeizhou160316 mod
{
    int ret;

    DBG(&g_I2cClient->dev,"%s()\n", __func__);

    properties_kobj = kobject_create_and_add("board_properties", NULL);
    if (properties_kobj)
    {
        ret = sysfs_create_group(properties_kobj, &properties_attr_group);
    }

    if (!properties_kobj || ret)
    {
        pr_err("failed to create board_properties\n");
    }
}
#endif
/* probe function is used for matching and initializing input device */
static int /*__devinit*/ touch_driver_probe(struct i2c_client *client,
        const struct i2c_device_id *id)
{
	struct msg_ts_platform_data *pdata = client->dev.platform_data;
    int ret = 0;
#ifdef CONFIG_OF
	struct device_node *np = client->dev.of_node;
	if (np && !pdata){
		pdata = msg2133_ts_parse_dt(&client->dev);
		if(pdata){
			client->dev.platform_data = pdata;
		}
		else{
			ret = -ENOMEM;
			return ret;
		}

	#if 1 // add for CONFIG_OF
	MS_TS_MSG_IC_GPIO_RST = pdata->reset_gpio_number;
	MS_TS_MSG_IC_GPIO_INT = pdata->irq_gpio_number;
	TOUCH_SCREEN_X_MAX = pdata->TP_MAX_X;
	TOUCH_SCREEN_Y_MAX = pdata->TP_MAX_Y;
	#endif
	}
#endif

#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
//    const char *vdd_name = "vdd";
//    const char *vcc_i2c_name = "vcc_i2c";
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON

    printk("*** %s ***\n", __FUNCTION__);
    
    if (client == NULL)
    {
        printk("i2c client is NULL\n");
        return -1;
    }
    g_I2cClient = client;


#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
    g_ReguVdd = regulator_get(&g_I2cClient->dev, pdata->vdd_name);

    ret = regulator_set_voltage(g_ReguVdd, 2800000, 2800000); 
    if (ret)
    {
        printk("Could not set to 2800mv.\n");
    }
/*
    g_ReguVcc_i2c = regulator_get(&g_I2cClient->dev, g_I2cClient->dev.platform_data->vcc_i2c_name);

    ret = regulator_set_voltage(g_ReguVcc_i2c, 1800000, 1800000);  
    if (ret)
    {
        printk("Could not set to 1800mv.\n");
    }
	*/
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON


#ifdef CONFIG_I2C_SPRD
	sprd_i2c_ctl_chg_clk(client->adapter->nr, 100000);
#endif

    return MsDrvInterfaceTouchDeviceProbe(g_I2cClient, id);
}

/* remove function is triggered when the input device is removed from input sub-system */
static int /*__devexit*/ touch_driver_remove(struct i2c_client *client)
{
    printk("*** %s ***\n", __FUNCTION__);

    return MsDrvInterfaceTouchDeviceRemove(client);
}

/* The I2C device list is used for matching I2C device and I2C device driver. */
static const struct i2c_device_id touch_device_id[] =
{
    {MSG_TP_IC_NAME, 0},
    {}, /* should not omitted */ 
};

MODULE_DEVICE_TABLE(i2c, touch_device_id);

static const struct of_device_id touch_dt_match_table[] = {
       { .compatible = "mstar,msg2xxx_ts", },
    {},
};

MODULE_DEVICE_TABLE(of, touch_dt_match_table);

static struct i2c_driver touch_device_driver =
{
    .driver = {
        .name = MSG_TP_IC_NAME,
        .owner = THIS_MODULE,
        .of_match_table = touch_dt_match_table,
    },
    .probe = touch_driver_probe,
    .remove = touch_driver_remove,
    .id_table = touch_device_id,
};

static int /*__init*/ touch_driver_init(void)
{
    int ret;

    /* register driver */
    ret = i2c_add_driver(&touch_device_driver);
    if (ret < 0)
    {
        printk("add MStar touch device driver i2c driver failed.\n");
        return -ENODEV;
    }
    printk("add MStar touch device driver i2c driver.\n");

    return ret;
}

static void /*__exit*/ touch_driver_exit(void)
{
    printk("remove MStar touch device driver i2c driver.\n");

    i2c_del_driver(&touch_device_driver);
}

module_init(touch_driver_init);
module_exit(touch_driver_exit);
MODULE_LICENSE("GPL");
