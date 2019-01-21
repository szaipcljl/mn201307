/*
 * kernel/private/drivers/vout/ds90ub925/ds90ub925.c
 *
 * History:
 *	2013/01/08 - [Johnson Diao]
 *
 * Copyright (C) 2016  Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ambpriv_device.h>
#include <linux/of_gpio.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <plat/clk.h>
#include "ds90ub925.h"

#define DRV_PRINT printk
struct ds90ub925_t {
    struct i2c_client *client;
	int pdb_gpio;
	int pdb_gpio_active;
    int rst_gpio;
    u8  rst_gpio_active;
};



static int addr = 0x0c;
module_param(addr, int, S_IRUGO | S_IWUSR);


/*******************************************
 ** I2C slave device read and write function
 *******************************************/
/* i2c_write_reg8 for 926*/
static s32 i2c_write_reg8(struct ds90ub925_t *ds90ub925, u8 slave_addr, u8 reg, u8 val)
{
	s32 rc = 0;
	u8 buf[2] = {0};
	u16 slave_addr_temp = 0;

	buf[0] = reg;
	buf[1] = val;

	slave_addr_temp = ds90ub925->client->addr;
	ds90ub925->client->addr = slave_addr;

	if (2 != i2c_master_send(ds90ub925->client, buf, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, val);
		rc = -1;
	}

	ds90ub925->client->addr = slave_addr_temp;
	return rc;
}

/* i2c_read_reg for 926*/
static s32 i2c_read_reg(struct ds90ub925_t *ds90ub925, u8 slave_addr, u8 reg, u8 *val)
{
	s32 rc = 0;
	u8 buf[1] = {0};
	u8 u8RdVal = 0;
	u16 slave_addr_temp = 0;

	buf[0] = reg;

	slave_addr_temp = ds90ub925->client->addr;
	ds90ub925->client->addr = slave_addr;

	if (1 != i2c_master_send(ds90ub925->client, buf, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x\n", __func__, slave_addr, reg);
		rc = -1;
	}

	if (1 != i2c_master_recv(ds90ub925->client, &u8RdVal, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, u8RdVal);
		rc = -1;
	}

	ds90ub925->client->addr = slave_addr_temp;
	*val = u8RdVal;
	return rc;
}


static u8 ds90ub925_read_reg(struct ds90ub925_t *ds90ub925, u32 addr)
{
    u8 temp;
    temp = i2c_smbus_read_byte_data(ds90ub925->client, addr);
    //DRV_PRINT("read [%x]=%x\r\n",addr,temp);
    return temp;
}

static int ds90ub925_write_reg(struct ds90ub925_t *ds90ub925, u32 addr, u8 mask, u8 data)
{
    struct i2c_client *client;
    u8 temp;
    client = ds90ub925->client;
    if (mask == 0xff) {
        //DRV_PRINT("write [%x]=%x\r\n",addr,data);
        i2c_smbus_write_byte_data(client, addr, data);
    } else {
        temp = (u8)i2c_smbus_read_byte_data(client, addr);
        temp &= (~mask);
        temp |= (data & mask);
        //DRV_PRINT("write [%x]=%x\r\n",addr,temp);
        i2c_smbus_write_byte_data(client, addr, temp);
    }
    return 0;
}

static int ds90ub925_write_reg_direct(struct ds90ub925_t *ds90ub925, u32 addr,  u8 data)
{
    ds90ub925_write_reg(ds90ub925, addr, 0xff, data);
    return 0;
}


static int ds90ub925_init_fun(struct ds90ub925_t *ds90ub925)
{
	// enable communication with all the remote devices
	ds90ub925_write_reg_direct(ds90ub925, UB925_I2C_PASS_THROUGH_REG, 0xda);

	// set Slave ID(0x07), Slave Alias(0x08)
	ds90ub925_write_reg_direct(ds90ub925, UB925_SLAVE_ID_REG, SLAVE_ADDR);
	ds90ub925_write_reg_direct(ds90ub925, UB925_SLAVE_ALIAS_REG, SLAVE_ADDR);

	// reset IT66121FN
	// 926.GPO_REG6(0x20) -> IT66121FN.SYSRSTN: 0->1
	//ds90ub925_write_reg_direct();
	i2c_write_reg8(ds90ub925, UB926_ADDR_7BIT, UB926_GPO_REG6, 0x10);
	mdelay(5);
	i2c_write_reg8(ds90ub925, UB926_ADDR_7BIT, UB926_GPO_REG6, 0x90);


    return 0;
}

static int ds90ub925_poweron(struct ds90ub925_t *ds90ub925)
{
	int rval = 0;
	//set pdb_gpio
	if (gpio_is_valid(ds90ub925->pdb_gpio)) {
		rval = devm_gpio_request(&ds90ub925->client->dev, ds90ub925->pdb_gpio, "ds90ub925 pdb gpio");
		if (rval < 0) {
			dev_err(&ds90ub925->client->dev, "Failed to request pdb gpio for ds90ub925: %d\n", rval);
			printk("#mnA:error\n");
			return rval;
		} else {
			gpio_direction_output(ds90ub925->pdb_gpio, ds90ub925->pdb_gpio_active);
		}
	}
	mdelay(10);
	printk("%s: ds90ub954 poweron!\n", __func__);
	return 0;
}


static int ds90ub925_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct ds90ub925_t *ds90ub925;
    u8 ds90ub925_id = {0};
    struct device_node *np = client->dev.of_node;
    enum of_gpio_flags flags;
    int rval;

    ds90ub925 = kzalloc(sizeof(struct ds90ub925_t), GFP_KERNEL);
    if (ds90ub925 == NULL) {
        return -ENOMEM;
    }
    ds90ub925->client = client;
    i2c_set_clientdata(client, ds90ub925);

    //get pdb_gpio
    ds90ub925->pdb_gpio = of_get_named_gpio_flags(np, "pdb_gpio", 0, &flags);
    ds90ub925->pdb_gpio_active = !!(flags && OF_GPIO_ACTIVE_LOW);
	ds90ub925_poweron(ds90ub925);

#if 0
    //get rst_gpio
    ds90ub925->rst_gpio = of_get_named_gpio_flags(np, "rst_gpios", 0, &flags);
    ds90ub925->rst_gpio_active = !!(flags && OF_GPIO_ACTIVE_LOW);
    if (gpio_is_valid(ds90ub925->rst_gpio)) {
        rval = devm_gpio_request(&client->dev, ds90ub925->rst_gpio, "ds90ub925 rst gpio");
        if(rval < 0) {
            dev_err(&client->dev, "Failed to request rst gpio for ds90ub925: %d\n", rval);
            return rval;
        } else {
            gpio_direction_output(ds90ub925->rst_gpio, !ds90ub925->rst_gpio_active);
        }
    }
#endif

    DRV_PRINT("DS90UB925 id= ");
	ds90ub925_id = ds90ub925_read_reg(ds90ub925, 0);
	DRV_PRINT("%x ", ds90ub925_id);
    DRV_PRINT("\r\n");

    ds90ub925_init_fun(ds90ub925);
    DRV_PRINT("DS90UB925 insmod success\r\n");
    return 0;
}

static int ds90ub925_remove(struct i2c_client *client)
{
    struct ds90ub925_t *ds90ub925;
    ds90ub925 = i2c_get_clientdata(client);
    kfree(ds90ub925);
    return 0;
}

static struct of_device_id ds90ub925_of_match[] = {
    { .compatible = "ambarella,ds90ub925",},
    {},
};
MODULE_DEVICE_TABLE(of, ds90ub925_of_match);


static const struct i2c_device_id ds90ub925_idtable[] = {
    { "ds90ub925", 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, ds90ub925_idtable);
static struct i2c_driver i2c_driver_ds90ub925 = {
    .driver = {
        .name = "ds90ub925",
        .owner = THIS_MODULE,
        .of_match_table = ds90ub925_of_match,
    },
    .id_table	=	ds90ub925_idtable,
    .probe	=	ds90ub925_probe,
    .remove	=	ds90ub925_remove,
};

static int __init ds90ub925_init(void)
{
    int rval;

    rval = i2c_add_driver(&i2c_driver_ds90ub925);
    if (rval < 0) {
        DRV_PRINT("DS90UB925 i2c driver register failed\r\n");
        return rval;
    }
    DRV_PRINT("DS90UB925 inited\r\n");
    return rval;
}

static void __exit ds90ub925_exit(void)
{
    i2c_del_driver(&i2c_driver_ds90ub925);
    DRV_PRINT("DS90UB925 removed\r\n");
}

module_init(ds90ub925_init);
module_exit(ds90ub925_exit);

MODULE_DESCRIPTION("DS90UB925 digital to HDMI converter");
MODULE_AUTHOR("Johnson Diao,<cddiao@ambarella.com>");
MODULE_LICENSE("GPL v2");
