#ifndef __SENSOR_H__
#define __SENSOR_H__

struct sensor_data {
	//const struct ov5642_platform_data *platform_data;
	//struct v4l2_int_device *v4l2_int_device;
	struct i2c_client *i2c_client;
	//struct v4l2_pix_format pix;
	//struct v4l2_captureparm streamcap;
	bool on;

	/* control settings */
	int brightness;
	int hue;
	int contrast;
	int saturation;
	int red;
	int green;
	int blue;
	int ae_mode;

	u32 mclk;
	u8 mclk_source;
	struct clk *sensor_clk;
	int csi;

	void (*io_init)(void);
};

#endif
