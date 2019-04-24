#ifndef _CMR_INTERFACE_H
#define _CMR_INTERFACE_H

enum ir_led_status {
	IR_LED_OFF,
	IR_LED_ON,
};



struct i2c_dev {
	int bus_id;
	char *adapt_dev[10];
	int i2c_addr;
	int fd;
	int is_open;
	const struct cmr_usr_ops *ops;
	int ir_led_ctl_reg;
	int ir_led_status;
	int ir_led_ctl_val;
	int ir_led_ctl_ori_val;
	int ir_led_ctl_ori_val_saved;
	int temp_ctl_reg;
	int temp_reg;
};

struct cmr_usr_ops {
	int (*set_sensor_temp_on)(struct i2c_dev *cmr_dev);
	int (*get_sensor_temp)(struct i2c_dev *cmr_dev);
	int (*get_sensor_ir_led_status)(struct i2c_dev *cmr_dev);
	int (*set_sensor_ir_led_on)(struct i2c_dev *cmr_dev);
	int (*set_sensor_ir_led_off)(struct i2c_dev *cmr_dev);
};

int i2c_device_open(struct i2c_dev *cmr_dev);
int i2c_device_close(struct i2c_dev *cmr_dev);

struct i2c_dev *i2c_dev_init();
int i2c_dev_release(struct i2c_dev *cmr_dev);


#endif
