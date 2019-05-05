#ifndef _BRG_DEV_INFO_H
#define _BRG_DEV_INFO_H

#define DS90UB934_BRG
// bridge info
#ifdef DS90UB934_BRG
#define DS934_BUS_ID		1
#define DS934_I2C_ADDR		0X30
#define DS934_PASS_GPIO		10
#define DS934_LOCK_GPIO		11

#define BRG_BUS_ID			DS934_BUS_ID
#define BRG_I2C_ADDR		DS934_I2C_ADDR
#define BRG_PASS_GPIO		DS934_PASS_GPIO
#define BRG_LOCK_GPIO		DS934_LOCK_GPIO
#endif

// DEVICE_STS
#define BRG_PASS		1
#define BRG_LOCK		1

#define DEVICE_STS		0x04

// used to open/close a i2c device
struct i2c_dev_base_info {
	char chip_name[16];
	int bus_id;
	int i2c_addr;
	int fd;
	int is_open;
};

enum brg_reg_name {
	BRG_STS,
	REG_MAX,
};

enum brg_pin_name {
	BRG_LOCK_PIN,
	BRG_PASS_PIN,
	BRG_INTB_PIN,
	BRG_PIN_MAX,
};

struct brg_dev_info {
	struct i2c_dev_base_info i2c_info;
	const struct brg_usr_ops *ops;
	int brg_regs[REG_MAX];
	//int brg_sts_reg;
	int brg_pins[BRG_PIN_MAX];
	//int lock_pin;
	//int pass_pin;
	//int intb_pin;
	int status;
};

// interfaces to access i2c dev after the dev opened
struct brg_usr_ops {
	int	(*brg_get_lock_pin_status)(struct brg_dev_info *brg_info);
	int (*brg_get_pass_pin_status)(struct brg_dev_info *brg_info);
	//int (*brg_get_dev_status)(struct brg_dev_info brg_info);
};

// open/close the i2c dev
int i2c_device_open(struct i2c_dev_base_info *dev);
int i2c_device_close(struct i2c_dev_base_info *dev);

// init the brg device
struct brg_dev_info *brg_device_init();
int brg_device_release(struct brg_dev_info *dev);

#endif
