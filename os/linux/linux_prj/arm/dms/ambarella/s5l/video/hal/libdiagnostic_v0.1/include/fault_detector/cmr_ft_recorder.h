#ifndef _CMR_FT_RECORDER_H
#define _CMR_FT_RECORDER_H

#define CMR_FAULT_RECORD_PATH "/home/default/cmr_ft_recorder"
#define FILE_NAME_MAX_LEN 32

/* camera fault recorder*/
struct cmr_ft_recorder {
	char record_file[FILE_NAME_MAX_LEN];
	int rcd_fd;
	long total_ft;
	long signal_ft;
	long volt_ft;
	long curr_ft;
	long pwr_zero_ft;
	long brg_pass;
	long brg_lock;
	struct timeval last_record_tm;
	const struct cmr_ft_rcd_operations *ops;
	int need_update_file;
};

struct cmr_ft_rcd_operations {
	void (*record_total_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_signal_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_volt_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_curr_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_pwr_zero_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_brg_pass_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_brg_lock_ft)(struct cmr_ft_recorder *cmr_ft_rcd);
	void (*record_file_update)(struct cmr_ft_recorder *cmr_ft_rcd);
};

struct cmr_ft_recorder *record_init(void);
void record_exit(struct cmr_ft_recorder *cmr_ft_rcd);

#endif
