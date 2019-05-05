#include <stdio.h>
#include <pthread.h>

#include "fault_detector/vin_cap.h"

int cmr_guarder_run(pthread_t *tid, void *arg)
{
	if (!tid) {
		printf("[%s] tid == NULL\n", __func__);
		return -1;
	}

	return pthread_create(tid, NULL, vin_cap_auto, arg);
}

