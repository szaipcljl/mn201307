#include <stdio.h>

#include "cmr_fault_detector.h"

/*
 * camera fault detector API
 */
int get_cmr_current_state(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev is NULL\n");
		return -1;
	}

	return ld_sw_get_current_state(&cmr_dev->ld_sw_dev);
}

int get_cmr_voltage_state(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev is NULL\n");
		return -1;
	}

	return ld_sw_get_voltage_state(&cmr_dev->ld_sw_dev);
}

int get_cmr_current_val(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev is NULL\n");
		return -1;
	}
	return ld_sw_get_current_val(&cmr_dev->ld_sw_dev);
}

bool is_cmr_current_normal(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

bool is_cmr_voltage_normal(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

bool get_cmr_vin_capture_state(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

bool is_cmr_vin_capture_normal(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

int get_cmr_temp(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

bool is_cmr_temp_normal(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

int cmr_dev_restart(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

int cmr_vins_resume(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

int cmr_ft_detect_init(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("[%s] error: cmr_dev is NULL\n", __func__);
		return -1;
	}

	cmr_dev->iav_fd = video_dev_open("/dev/iav");
	//snr_open(addr); //i2c device open
	if (ld_sw_gpio_init(&cmr_dev->ld_sw_dev) < 0) {
		printf("[%s] error: ld_sw_gpio_init failed\n", __func__);
		return -1;
	}

	ld_sw_adc_chan_init(&cmr_dev->ld_sw_dev);

	cmr_dev->initialized = 1;

	return 0;
}

int cmr_ft_detect_exit(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("[%s] error: cmr_dev is NULL\n", __func__);
		return -1;
	}

	video_dev_close(cmr_dev->iav_fd);
	//video_dev_close(cmr_dev->snr_fd);
	return 0;
}

