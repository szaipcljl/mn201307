#include <stdio.h>
#include "fault_detector/cmr_dev_info.h"

/*
 * cmr dev info API
 */

int ld_sw_adc_chan_init(struct load_switch_dev *ld_sw_dev)
{
	if (!ld_sw_dev) {
		printf("[%s] ld_sw_dev is NULL\n", __func__);
		return -1;
	}

	ld_sw_dev->adc_chan = ADC_CHAN;

	return 0;
}

int ld_sw_get_current_state(struct load_switch_dev *ld_sw_dev)
{
	if (!ld_sw_dev) {
		printf("[%s] ld_sw_dev is NULL\n", __func__);
		return -1;
	}
	return get_gpio_state(ld_sw_dev, GPIO_CUR_IDX);
}

int ld_sw_get_voltage_state(struct load_switch_dev *ld_sw_dev)
{
	if (!ld_sw_dev) {
		printf("[%s] ld_sw_dev is NULL\n", __func__);
		return -1;
	}
	return get_gpio_state(ld_sw_dev, GPIO_VOL_IDX);
}

bool ld_sw_is_current_normal(struct load_switch_dev *ld_sw_dev)
{
	return 0;
}

bool ld_sw_is_voltage_normal(struct load_switch_dev *ld_sw_dev)
{
	return 0;
}



int ld_sw_gpio_init(struct load_switch_dev* ld_sw_dev)
{
	int ret;
	int i;
	int gpio_num = GPIO_NUM_MAX;
	ld_sw_dev->ft_ind_pins[GPIO_CUR_IDX] = CAM_PWR_NFLAG;
	ld_sw_dev->ft_ind_pins[GPIO_VOL_IDX] = CAM_PWR_NUVOV;

	if (!ld_sw_dev) {
		printf("[%s] error: ld_sw_dev is NULL\n", __func__);
		return -1;
	}

	for (i = 0; i < gpio_num; i++) {
		ret = gpio_init(ld_sw_dev->ft_ind_pins[i]);
		if (ret < 0) {
			perror("gpio init failed!\n");
			return -1;
		}

	}

	return 0;
}

/* return adc raw */
int ld_sw_get_current_val(struct load_switch_dev* ld_sw_dev)
{
	if (!ld_sw_dev) {
		printf("[%s] error: ld_sw_dev is NULL\n", __func__);
		return -1;
	}

	return get_current_adc_val(ld_sw_dev->adc_chan);
}

/* camera device info API */
int snr_open(int addr)
{
	return 0;
}


int snr_get_temp(struct cmr_dev_info *cmr_dev)
{
	return 0;
}

