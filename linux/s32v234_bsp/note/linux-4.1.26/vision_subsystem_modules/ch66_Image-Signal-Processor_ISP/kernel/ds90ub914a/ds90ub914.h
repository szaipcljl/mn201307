#ifndef __DS90UB914_H__
#define __DS90UB914_H__

struct ds90ub914a_platform_data {
	int ds90ub914_pwn_gpio;
	//we see ds90ub914a and remote camera power on pin as ds90ub914a
	int cmr_pwr_pin; //managed by ds90ub914a driver for convenience
	int ds90ub913_i2c_addr;
	int camera_i2c_addr;
};


struct ds90ub914_i2c_dev {
	struct i2c_client *i2c_client;
	struct ds90ub914a_platform_data *ds914_pdata;
};

#if 0
#define PFLAG "#mn: "
#else
#define PFLAG
#endif

#undef dev_err
#define dev_err(dev, format, arg...)		\
	dev_printk(KERN_DEBUG, dev, PFLAG format, ##arg)

#endif
