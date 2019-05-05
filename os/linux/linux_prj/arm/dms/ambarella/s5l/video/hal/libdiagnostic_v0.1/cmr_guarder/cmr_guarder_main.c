#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "fault_detector/cmr_fault_detector.h"
#include "fault_detector/vin_cap.h"
#include "cmr_guarder.h"


extern int fd_iav;

struct cmr_fault_detector cmr_ft_dtect;
struct cmr_dev_info *cmr_devp;

#ifdef CMR_GUARD_DEBUG
int test_get_cur_vol_state(struct cmr_dev_info *cmr_dev)
{
	int state;
	while (1) {
		state = get_cmr_current_state(cmr_dev);
		if (state < 0) {
			printf("[%s] get cmr current state failed\n", __func__);
			return -1;
		} else {
			printf("[%s] current state is: %d\n", __func__, state);
		}

		state = get_cmr_voltage_state(cmr_dev);
		if (state < 0) {
			printf("[%s] get cmr voltage state failed\n", __func__);
			return -1;
		} else {
			printf("[%s] current voltage is: %d\n", __func__, state);
		}
		get_cmr_current_val(cmr_dev);

		cmr_get_brg_pass_status(cmr_dev);

		sleep(1);
	}

	return 0;
}
#endif

int main(int argc, const char *argv[])
{
	int ret;
	pthread_t tid[2];

	ret = cmr_ft_detector_init(&cmr_ft_dtect);
	if (ret < 0) {
		printf("cmr_ft_detector_init failed \n");
		goto fail1;
	}
	fd_iav = cmr_ft_dtect.cmr_dev.iav_fd;


	ret = cmr_guarder_run(tid[0]);
	if (ret !=0) {
		printf("[%s]: cmr_guarder_run failed\n", __func__);
		goto fail1;

	}

	//ret = pthread_create(&tid[1], NULL, cur_volt_loop, (void *)NULL);

#ifdef CMR_GUARD_DEBUG
	test_get_cur_vol_state(&cmr_ft_dtect.cmr_dev);
#endif

	pthread_join(tid[0],NULL);

	return 0;

fail1:
	cmr_ft_detector_exit(&cmr_ft_dtect);

	return -1;
}
