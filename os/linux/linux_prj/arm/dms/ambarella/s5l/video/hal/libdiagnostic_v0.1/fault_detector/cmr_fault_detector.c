#include <stdio.h>

#include "fault_detector/cmr_fault_detector.h"
#include "fault_detector/cmr_ft_recorder.h"
#include "fault_detector/brg_dev_info.h"

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

/* current adc val*/
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

int cmr_ft_detector_init(struct cmr_fault_detector *cmr_ft_detector)
{
	struct cmr_dev_info *cmr_dev = &cmr_ft_detector->cmr_dev;

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

	cmr_ft_detector->ft_rcd = cmr_ft_record_init();
	if (!cmr_ft_detector->ft_rcd) {
		printf("cmr_ft_record_init failed\n");
		return -1;
	}

	// brg info init
	cmr_dev->brg_info = brg_device_init();

	cmr_dev->initialized = 1;

	return 0;
}

int cmr_ft_detector_exit(struct cmr_fault_detector *cmr_ft_detector)
{
	struct cmr_dev_info *cmr_dev = &cmr_ft_detector->cmr_dev;

	if (!cmr_dev) {
		printf("[%s] error: cmr_dev is NULL\n", __func__);
		return -1;
	}

	video_dev_close(cmr_dev->iav_fd);
	//video_dev_close(cmr_dev->snr_fd);
	return 0;
}

struct cmr_ft_recorder *cmr_ft_record_init(void)
{
	return record_init();
}

void cmr_ft_record_exit(struct cmr_ft_recorder *cmr_ft_rcd)
{
	record_exit(cmr_ft_rcd);
}

int cmr_ft_diagnostic_work(struct cmr_fault_detector *cmr_ft_detector)
{
	int cam_pwr_flag, cam_pwr_uvov, cam_cur_adc_val;
	int brg_pass, brg_lock;

	struct cmr_dev_info *cmr_dev = &cmr_ft_detector->cmr_dev;
	struct cmr_ft_recorder *ft_rcd = cmr_ft_detector->ft_rcd;

	cam_pwr_flag = get_cmr_current_state(cmr_dev);
	cam_pwr_uvov = get_cmr_voltage_state(cmr_dev);
	cam_cur_adc_val = get_cmr_current_val(cmr_dev);
	brg_pass = cmr_get_brg_pass_status(cmr_dev);
	brg_lock = cmr_get_brg_lock_status(cmr_dev);

	printf("cam_pwr_flag: %d, cam_pwr_uvov: %d, cam_cur_adc_val:%d\n",\
			cam_pwr_flag, cam_pwr_uvov, cam_cur_adc_val);

// TODO: get mutex
	if (cam_pwr_flag < 0 || cam_pwr_uvov <0 || cam_cur_adc_val < 0) {
		printf("get camera state failed\n");
		return -1;
	} else if (cam_pwr_uvov == STAT_ERR) {
		printf("voltage error: cam_pwr_uvov:%d\n", STAT_ERR);
		if (ft_rcd->ops->record_volt_ft) {
			ft_rcd->ops->record_volt_ft(ft_rcd);
		}
	} else if (cam_pwr_flag == STAT_ERR) {
		printf("current error: cam_pwr_flag:%d\n", STAT_ERR);
		if (ft_rcd->ops->record_curr_ft) {
			ft_rcd->ops->record_curr_ft(ft_rcd);
		}
	} else if (cam_cur_adc_val < 16) { //adc raw < 16, when disconneting the power cord
		printf("disconneting the power cord error: cam_cur_adc_val:%d\n", cam_cur_adc_val);
		if (ft_rcd->ops->record_pwr_zero_ft) {
			ft_rcd->ops->record_pwr_zero_ft(ft_rcd);
		}
	} else if (brg_pass < 0 || brg_pass < 0) {
		printf("get brg state failed\n");
	} else if (brg_pass == 0) {
		printf("brg error: brg_pass:%d\n", STAT_ERR);
		if (ft_rcd->ops->record_brg_pass_ft) {
			ft_rcd->ops->record_brg_pass_ft(ft_rcd);
		}
	} else if (brg_lock == 0) {
		printf("brg error: brg_lock:%d\n", STAT_ERR);
		if (ft_rcd->ops->record_brg_lock_ft) {
			ft_rcd->ops->record_brg_lock_ft(ft_rcd);
		}
	} else {
		if (cmr_ft_detector->vsync_lost) {
			printf("signal_corrupt error\n");
			if (ft_rcd->ops->record_signal_ft) {
				ft_rcd->ops->record_signal_ft(ft_rcd);
			}
		} else {
			// do nothing
			printf("[%s] timer expires\n", __func__);
		}
	}

	cmr_ft_detector->vsync_lost = 0;

	//TODO: release mutex

	if (ft_rcd->ops->record_file_update && ft_rcd->need_update_file == 1) {
		ft_rcd->ops->record_file_update(ft_rcd);
	}

	return 0;
}

// camera brg interfaces
int cmr_get_brg_lock_status(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info) {
		printf("error: cmr_dev->brg_info is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info->ops) {
		printf("error: cmr_dev->brg_info->ops is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info->ops->brg_get_lock_pin_status) {
		printf("error: cmr_dev->brg_info->ops->brg_get_lock_pin_status is NULL\n");
		return -1;
	}

	return cmr_dev->brg_info->ops->brg_get_lock_pin_status(cmr_dev->brg_info);
}

int cmr_get_brg_pass_status(struct cmr_dev_info *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info) {
		printf("error: cmr_dev->brg_info is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info->ops) {
		printf("error: cmr_dev->brg_info->ops is NULL\n");
		return -1;
	}

	if (!cmr_dev->brg_info->ops->brg_get_pass_pin_status) {
		printf("error: cmr_dev->brg_info->ops->brg_get_pass_pin_status is NULL\n");
		return -1;
	}

	return cmr_dev->brg_info->ops->brg_get_pass_pin_status(cmr_dev->brg_info);
}
