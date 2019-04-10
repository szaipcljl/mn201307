#ifndef _CMR_FAULT_DETECTOR_H
#define _CMR_FAULT_DETECTOR_H
#include "cmr_dev_info.h"


enum cmr_ft_detect_ver {
	CMR_FT_DTCT_VER0,
	CMR_FT_DTCT_VER1,
	CMR_FT_DTCT_VER2,
	CMR_FT_DTCT_VER3,
};

struct cmr_fault_detector {
	int version;
	struct cmr_dev_info cmr_dev;
	int cmr_cur_state; /* recoder the state last read*/
	int cmr_vol_state;
	struct cmr_ft_recorder *ft_rcd;
};

int get_cmr_current_state(struct cmr_dev_info *cmr_dev);
int get_cmr_voltage_state(struct cmr_dev_info *cmr_dev);
int get_cmr_current_val(struct cmr_dev_info *cmr_dev);
bool is_cmr_current_normal(struct cmr_dev_info *cmr_dev);
bool is_cmr_voltage_normal(struct cmr_dev_info *cmr_dev);

bool get_cmr_vin_capture_state(struct cmr_dev_info *cmr_dev);
bool is_cmr_vin_capture_normal(struct cmr_dev_info *cmr_dev);

int get_cmr_temp(struct cmr_dev_info *cmr_dev);
bool is_cmr_temp_normal(struct cmr_dev_info *cmr_dev);

int cmr_dev_restart(struct cmr_dev_info *cmr_dev);
int cmr_vins_resume(struct cmr_dev_info *cmr_dev);

int cmr_ft_detect_init(struct cmr_dev_info *cmr_dev);
int cmr_ft_detect_exit(struct cmr_dev_info *cmr_dev);

struct cmr_ft_recorder *cmr_ft_record_init(void);
void cmr_ft_record_exit(struct cmr_ft_recorder *cmr_ft_rcd);

int cmr_ft_record_work(struct cmr_dev_info *cmr_dev, struct cmr_ft_recorder *ft_rcd);

#endif
