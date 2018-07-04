#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include "ds90ub914.h"

/***********************************
 ** Architecture:
 ** I2C bus: I2C1
 ** 7'bit ADDR:   0x61   0x5d     0x10
 **        s32v -> 914 -> 913  -> ar0144
 ***********************************/
static struct ds90ub914_i2c_dev ds90ub914a_dev;

/*******************************************
 ** I2C slave device read and write function
 *******************************************/
/* i2c_write_reg8 for 914 913*/
static s32 i2c_write_reg8(struct i2c_client *client, u8 reg, u8 val)
{
	s32 rc = 0;
	u8 au8Buf[2] = {0};

	au8Buf[0] = reg;
	au8Buf[1] = val;

	if (2 != i2c_master_send(client, au8Buf, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, client->addr, reg, val);
		rc =  -1;
	}

	return rc;
}

/* i2c_read_reg8 for 914 913*/
static s32 i2c_read_reg8(struct i2c_client *client, u8 reg, u8 *val)
{
	s32 rc = 0;
	u8 au8RegBuf[1] = {0};
	u8 u8RdVal = 0;

	au8RegBuf[0] = reg;

	if (1 != i2c_master_send(client, au8RegBuf, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x\n", __func__, client->addr, reg);
		rc = -1;
	}

	if (1 != i2c_master_recv(client, &u8RdVal, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, client->addr, reg, u8RdVal);
		rc = -1;
	}

	*val = u8RdVal;
	return rc;
}


/* config 914 regs */
static int config_914_regs(void)
{
	i2c_write_reg8(ds90ub914a_dev.i2c_client, 0x07,  ds90ub914a_dev.ds914_pdata->ds90ub913_i2c_addr << 1); //913 slave addr
	i2c_write_reg8(ds90ub914a_dev.i2c_client, 0x09,  ds90ub914a_dev.ds914_pdata->camera_i2c_addr << 1); //ar0144 slave addr
	i2c_write_reg8(ds90ub914a_dev.i2c_client, 0x11, ds90ub914a_dev.ds914_pdata->camera_i2c_addr << 1); //ar0144 slave addr
	mdelay(1);

	return 0;
}

static int power_on(void)
{
	//enable ds90ub914
	gpio_set_value(ds90ub914a_dev.ds914_pdata->ds90ub914_pwn_gpio, 1);

	//enable ds90ub913 xc7027 ar0144 power
	gpio_set_value(ds90ub914a_dev.ds914_pdata->cmr_pwr_pin, 1);

	//the delay must be 20ms, or read/write 913 reg failed
	mdelay(2);
	return 0;
}

static int power_off(void)
{
	//disable ds90ub913 ar0144 power
	gpio_set_value(ds90ub914a_dev.ds914_pdata->cmr_pwr_pin, 0);

	//disable ds90ub914
	gpio_set_value(ds90ub914a_dev.ds914_pdata->ds90ub914_pwn_gpio, 0);

	return 0;
}

static struct ds90ub914a_platform_data * ds90ub914a_parse_dt(struct device *dev)
{
	int retval;
	struct ds90ub914a_platform_data *pdata;

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		dev_err(dev, "Could not allocate struct ds90ub914a_platform_data");
		return NULL;
	}

	/* request ds90ub914 power down pin */
	pdata->ds90ub914_pwn_gpio = of_get_named_gpio(dev->of_node, "ds90ub914-pwn-gpios", 0);
	if (!gpio_is_valid(pdata->ds90ub914_pwn_gpio)) {
		dev_err(dev, "no ds90ub914 pwdn pin available\n");
		goto fail;
	}

	/* request ar0144 power enable pin */
	pdata->cmr_pwr_pin = of_get_named_gpio(dev->of_node, "cmr-pwr-pin", 0);
	if (!gpio_is_valid(pdata->cmr_pwr_pin)) {
		dev_err(dev, "no camera pwr-on pin available\n");
		goto fail;
	}

	retval = of_property_read_u32(dev->of_node, "DS90UB913_I2C_ADDR", &pdata->ds90ub913_i2c_addr);
	if (retval) {
		dev_err(dev, "fail to get DS90UB913_I2C_ADDR\n");
		goto fail;
	}

	retval = of_property_read_u32(dev->of_node, "CAMERA_I2C_ADDR", &pdata->camera_i2c_addr);
	if (retval) {
		dev_err(dev, "fail to get CAMERA_I2C_ADDR\n");
		goto fail;
	}

	return pdata;
fail:
	kfree(pdata);
	return NULL;
}

static s32 ds90ub914a_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct ds90ub914a_platform_data *pdata = client->dev.platform_data;
	s32 retval;

	memset(&ds90ub914a_dev, 0, sizeof(ds90ub914a_dev));
	ds90ub914a_dev.i2c_client = client;

	pdata = ds90ub914a_parse_dt(dev);
	if (pdata) {
		client->dev.platform_data = pdata;
		ds90ub914a_dev.ds914_pdata = pdata;
	} else {
		dev_err(dev, PFLAG "parse device tree failed\n");
		return -ENOMEM;
	}

	//request gpio for module power control
	retval = devm_gpio_request_one(dev, pdata->ds90ub914_pwn_gpio, GPIOF_OUT_INIT_LOW, "ds90ub914_pwdn");
	if (retval < 0) {
		dev_err(dev, PFLAG "gpio request ds90ub914_pwdn failed\n");
		return retval;
	}

	retval = devm_gpio_request_one(dev, pdata->cmr_pwr_pin, GPIOF_OUT_INIT_LOW, "cmr-pwr-on");
	if (retval < 0) {
		dev_err(dev, PFLAG "gpio request isp_pwdn failed\n");
		return retval;
	}

	power_on();
	config_914_regs();

	printk("[%s] ds90ub914a is found and configured!\n", __func__);

	return retval;
}

static int ds90ub914a_remove(struct i2c_client *client)
{
	power_off();
	return 0;
}

static const struct i2c_device_id ds90ub914a_id[] = {
	{ "ds90ub914a", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ds90ub914a_id);

static const struct of_device_id ds90ub914a_dt_ids[] = {
	{ .compatible = "ti,ds90ub914a" },
	{ }
};
MODULE_DEVICE_TABLE(of, ds90ub914a_dt_ids);

static struct i2c_driver ds90ub914a_driver = {
	.driver = {
		.name = "ds90ub914a",
		.of_match_table	= ds90ub914a_dt_ids,
	},
	.id_table = ds90ub914a_id,
	.probe	  = ds90ub914a_probe,
	.remove   = ds90ub914a_remove,
};

module_i2c_driver(ds90ub914a_driver);

MODULE_AUTHOR("Roadefend, Inc.");
MODULE_DESCRIPTION("ds90ub914a driver");
MODULE_LICENSE("GPL");
