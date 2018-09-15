/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <cutils/log.h>

#include <hardware/sensors.h>
#include <utils/Timers.h>

#define S_ON	1
#define S_OFF	0

struct sensors_poll_device_t *device;
struct sensors_module_t *module;
struct sensor_t const *list;
int count = 0;

static int activate_sensors(int id, int delay, int opt)
{
	int err;
	err = device->activate(device, list[id].handle, 0);
	if (err != 0) {
		printf("mmitest deactivate() for '%s'failed (%s)\n",list[id].name, strerror(-err));
		return 0;
	}
	if (!opt) {
		return 0;
	}
	err = device->activate(device, list[id].handle, 1);
	if (err != 0) {
		printf("mmitest activate() for '%s'failed (%s)\n",list[id].name, strerror(-err));
		return 0;
	}
	device->setDelay(device, list[id].handle, ms2ns(delay));
	return err;
}

int sensor_enable()
{
	int i,err;
	static const size_t numEvents = 16;
	sensors_event_t buffer[numEvents];

	for ( i = 0; i < count; i++) {
		err = activate_sensors(i, NULL, S_ON);
		if (err != 0) {
			printf("activate_sensors(OFF) for '%s'failed (%s)\n", list[i].name, strerror(-err));
			return 0;
		}
	}

	do {
		int n = device->poll(device, buffer, numEvents);
		if (n < 0) {
			printf("poll() failed (%s)\n", strerror(-err));
			break;
		}

		for (int i = 0; i < n; i++) {
			const sensors_event_t & data = buffer[i];

			if (data.version != sizeof(sensors_event_t)) {
				printf("incorrect event version (version=%d, expected=%d",
				     data.version, sizeof(sensors_event_t));
				break;
			}
		}
	} while (1);

	return err;
}

int sensor_stop()
{
	int i,err;

	/*********activate_sensors(OFF)***************/
	for (i = 0; i < count; i++) {
		err = activate_sensors(i, NULL, S_OFF);
		if (err != 0) {
			printf("activate_sensors(OFF) for '%s'failed (%s)\n", list[i].name, strerror(-err));
			return 0;
		}
	}
	/*********close sensor***************/
	err = sensors_close(device);
	if (err != 0) {
		printf("mmitest sensors_close() failed (%s)\n", strerror(-err));
	}
	return err;
}

int sensor_start()
{
	int err;

	/*********load sensor.so***************/
	err = hw_get_module(SENSORS_HARDWARE_MODULE_ID,
			    (hw_module_t const **)&module);
	if (err != 0) {
		printf("mmitest hw_get_module() failed (%s)\n", strerror(-err));
		return 0;
	}
	/*********open sensor***************/
	err = sensors_open(&module->common, &device);
	if (err != 0) {
		printf("mmitest sensors_open() failed (%s)\n", strerror(-err));
		return 0;
	}
	/*********read cmd***************/
	count = module->get_sensors_list(module, &list);

	return err;
}

int main(int argc, char **argv)
{
	int err;

	err = sensor_start();
	if(err < 0) {
		printf("sensor_start() failed\n");
		return -1;
	}
	err = sensor_enable();
	if(err < 0) {
		printf("sensor_enable() failed\n");
		return -1;
	}
	err = sensor_stop();
	if(err < 0) {
		printf("sensor_stop() failed\n");
		return -1;
	}
	return err;
}
