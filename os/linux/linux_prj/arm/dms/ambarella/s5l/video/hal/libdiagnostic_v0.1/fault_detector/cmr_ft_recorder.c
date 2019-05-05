#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fault_detector/cmr_ft_recorder.h"

void record_total_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->total_ft++;
	cmr_ft_rcd->need_update_file = 1;

	printf("cmr_ft_rcd->total_ft:%ld\n", cmr_ft_rcd->total_ft);
}

void record_signal_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->signal_ft++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
}

void record_volt_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->volt_ft++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
}

void record_curr_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->curr_ft++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
}

void record_pwr_zero_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->pwr_zero_ft++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
	printf("cmr_ft_rcd->pwr_zero_ft:%ld\n", cmr_ft_rcd->pwr_zero_ft);
}

void record_brg_pass_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->brg_pass++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
	printf("cmr_ft_rcd->brg_pass:%ld\n", cmr_ft_rcd->brg_pass);
}

void record_brg_lock_ft(struct cmr_ft_recorder *cmr_ft_rcd)
{
	cmr_ft_rcd->brg_lock++;
	cmr_ft_rcd->need_update_file = 1;

	if (cmr_ft_rcd->ops->record_total_ft) {
		cmr_ft_rcd->ops->record_total_ft(cmr_ft_rcd);
	}
	printf("cmr_ft_rcd->brg_lock:%ld\n", cmr_ft_rcd->brg_lock);
}

void record_file_update(struct cmr_ft_recorder *cmr_ft_rcd)
{
#define BUF_LEN 160
	struct timeval current_tv;
	char buf[BUF_LEN] = {0};
	int buf_len;

	gettimeofday(&current_tv, NULL);

	if (current_tv.tv_sec - cmr_ft_rcd->last_record_tm.tv_sec > 3) {
		memcpy(&cmr_ft_rcd->last_record_tm, &current_tv, sizeof(struct timeval));

		snprintf(buf, BUF_LEN, "last_update: %ld, total_ft:%ld, signal_ft: %ld,"\
				" volt_ft: %ld, curr_ft: %ld, pwr_zero_ft: %ld"\
				" brg_pass: %ld, brg_lock: %ld\n",\

				current_tv.tv_sec, \
				cmr_ft_rcd->total_ft,\
				cmr_ft_rcd->signal_ft,\
				cmr_ft_rcd->volt_ft,\
				cmr_ft_rcd->curr_ft,\
				cmr_ft_rcd->pwr_zero_ft,\
				cmr_ft_rcd->brg_pass,\
				cmr_ft_rcd->brg_lock);

		buf_len = strlen(buf);
		printf("buf_len:%d\n", buf_len);

		if (buf_len > BUF_LEN) {
			buf_len = BUF_LEN;
			buf[BUF_LEN-2] = '\n';
			buf[BUF_LEN-1] = '\0';
		}

		write(cmr_ft_rcd->rcd_fd, buf, buf_len);
		cmr_ft_rcd->need_update_file = 0;
	}

}

struct cmr_ft_rcd_operations rcd_ops = {
	.record_total_ft = record_total_ft,
	.record_signal_ft = record_signal_ft,
	.record_volt_ft = record_volt_ft,
	.record_curr_ft = record_curr_ft,
	.record_pwr_zero_ft = record_pwr_zero_ft,
	.record_brg_pass_ft = record_brg_pass_ft,
	.record_brg_lock_ft = record_brg_lock_ft,
	.record_file_update = record_file_update,
};

struct cmr_ft_recorder *record_init(void)
{
	struct cmr_ft_recorder *pcmr_ft_rcd;
	int rcd_fd;

	pcmr_ft_rcd = malloc(sizeof(struct cmr_ft_recorder));
	if (!pcmr_ft_rcd) {
		printf("malloc struct cmr_ft_recorder object failed \n");
		return NULL;
	}

	memset(pcmr_ft_rcd, 0, sizeof(struct cmr_ft_recorder));

	pcmr_ft_rcd->ops = &rcd_ops;
	//pcmr_ft_rcd->record_file = CMR_FAULT_RECORD_PATH;
	memcpy(pcmr_ft_rcd->record_file, CMR_FAULT_RECORD_PATH, FILE_NAME_MAX_LEN);

	// open the file
	if ((rcd_fd = open(CMR_FAULT_RECORD_PATH, O_RDWR | O_CREAT | O_APPEND, 0)) < 0) {
		perror(CMR_FAULT_RECORD_PATH);
		return NULL;
	}

	pcmr_ft_rcd->rcd_fd = rcd_fd;

	return pcmr_ft_rcd;
}

void record_exit(struct cmr_ft_recorder *cmr_ft_rcd)
{
	free(cmr_ft_rcd);
	close(cmr_ft_rcd->rcd_fd);
}

