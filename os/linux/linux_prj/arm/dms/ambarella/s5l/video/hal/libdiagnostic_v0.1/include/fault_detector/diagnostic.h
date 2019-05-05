#ifndef _DIAGNOSTIC_H
#define _DIAGNOSTIC_H

typedef union {
	u8  state;
	struct {
		u8  cmr_pwr_volt_ft :		1; //voltage
		u8  cmr_pwr_curr_ft :		1; //current out of range
		u8  cmr_pwr_curr_zero_ft :	1;
		u8  cmr_brg_pass_ft :		1;
		u8  cmr_brg_lock_ft :		1;
		u8  cmr_remote_brg_ft :		1;
		u8  snr_access_ft :			1;
		u8  single_only_ft :		1;
	} bits;
} dev_diagnostic_state;
#endif
