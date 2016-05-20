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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "AccSensor.h"

//#define MC3XXX_HAL_DRIVER_USING_IOCTL

#define MC3XXX_ACC_DEV_PATH_NAME    "/dev/mc3xxx"
#define MC3XXX_ACC_INPUT_NAME       "accelerometer" 
#define MC3XXX_SYSFS_PATH		"/sys/class/input"

#define ACC_UNIT_CONVERSION(value) ((value) * GRAVITY_EARTH / (1024.0f))

#ifdef MC3XXX_HAL_DRIVER_USING_IOCTL
#define	MC3XXX_ACC_IOCTL_BASE			77
#define	MC3XXX_ACC_IOCTL_SET_DELAY		_IOW(MC3XXX_ACC_IOCTL_BASE, 0, int)
#define	MC3XXX_ACC_IOCTL_GET_DELAY		_IOR(MC3XXX_ACC_IOCTL_BASE, 1, int)
#define	MC3XXX_ACC_IOCTL_SET_ENABLE		_IOW(MC3XXX_ACC_IOCTL_BASE, 2, int)
#define	MC3XXX_ACC_IOCTL_GET_ENABLE		_IOR(MC3XXX_ACC_IOCTL_BASE, 3, int)
#endif

/*****************************************************************************/
static struct sensor_t sSensorList[] = {
	{"mCube MC3XXX 3-axis Accelerometer",
	 "mCube",
	 1, 
	 SENSORS_ACCELERATION_HANDLE,
	 SENSOR_TYPE_ACCELEROMETER, 
	 (GRAVITY_EARTH * 2.0f),
	 (GRAVITY_EARTH) / 1024.0f, 
	 0.145f, 
	 10000, 
	 0,
	 0,
	 SENSOR_STRING_TYPE_ACCELEROMETER,
	 "",
	 1000000,
	 SENSOR_FLAG_CONTINUOUS_MODE,
	 {}
	 },
};

#ifndef MC3XXX_HAL_DRIVER_USING_IOCTL
//static char input_sysfs_path[PATH_MAX];
//static int input_sysfs_path_len;

//#define MC3XXX_DATA_NAME    "mc3xxx" 

static int mc3xxx_get_class_path(char *class_path)
{
	char dirname[] = MC3XXX_SYSFS_PATH;
	char buf[256];
	int res;
	DIR *dir;
	struct dirent *de;
	int fd = -1;
	int found = 0;

	dir = opendir(dirname);
	if (dir == NULL)
		return -1;

	while((de = readdir(dir))) {
		if (strncmp(de->d_name, "input", strlen("input")) != 0) {
		    continue;
        	}

		sprintf(class_path, "%s/%s", dirname, de->d_name);
		snprintf(buf, sizeof(buf), "%s/name", class_path);

		fd = open(buf, O_RDONLY);
		if (fd < 0) {
		    continue;
		}
		if ((res = read(fd, buf, sizeof(buf))) < 0) {
		    close(fd);
		    continue;
		}
		buf[res - 1] = '\0';
		if (strcmp(buf, MC3XXX_ACC_INPUT_NAME) == 0) {
		    found = 1;
		    close(fd);
		    break;
		}

		close(fd);
		fd = -1;
	}
	closedir(dir);
	//ALOGE("the G sensor dir is %s",class_path);

	if (found) {
		return 0;
	}else {
		*class_path = '\0';
		return -1;
	}
}
#endif


AccSensor::AccSensor() :
	SensorBase(MC3XXX_ACC_DEV_PATH_NAME, MC3XXX_ACC_INPUT_NAME),
		mEnabled(0), mDelay(-1), mInputReader(32), mHasPendingEvent(false),
		mSensorCoordinate()
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = ID_A;
	mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

#ifndef MC3XXX_HAL_DRIVER_USING_IOCTL
	memset(input_sysfs_path, '\0', sizeof(input_sysfs_path));
	if (mc3xxx_get_class_path(input_sysfs_path))
	{
		ALOGE("AccSensor::AccSensor: Can`t find MC3XXX sensor!");
		input_sysfs_path[0] = '\0';
	}
	
	input_sysfs_path_len = strlen(input_sysfs_path);
	input_sysfs_path[input_sysfs_path_len ++] = '/';
	ALOGD("AccSensor::AccSensor: %s", input_sysfs_path);	
#else
	open_device();
#endif

#if 0 //ndef MC3XXX_HAL_DRIVER_USING_IOCTL
	if (data_fd) {
		strcpy(input_sysfs_path, "/sys/class/input/input2/");
		//strcpy(input_sysfs_path, "/sys/class/input/input1/");
		input_sysfs_path_len = strlen(input_sysfs_path);
	} else {
		input_sysfs_path[0] = '\0';
		input_sysfs_path_len = 0;
	}
#endif

    ALOGD("AccSensor::AccSensor(Acc_Mc3xxx.cpp): construct called");
}

AccSensor::~AccSensor()
{
	if (mEnabled) {
		setEnable(0, 0);
	}

	close_device();

    ALOGD("AccSensor::~AccSensor(Acc_Mc3xxx.cpp): destroy called");
}

int AccSensor::setInitialState()
{
	struct input_absinfo absinfo;

	if (mEnabled) {
		if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_X), &absinfo)) {
			mPendingEvent.acceleration.x = ACC_UNIT_CONVERSION(absinfo.value);
		}
		if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Y), &absinfo)) {
			mPendingEvent.acceleration.y = ACC_UNIT_CONVERSION(absinfo.value);
		}
		if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Z), &absinfo)) {
			mPendingEvent.acceleration.z = ACC_UNIT_CONVERSION(absinfo.value);
		}
	}
	return 0;
}

bool AccSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int AccSensor::setEnable(int32_t handle, int enabled)
{
	int err = 0;
	int opDone = 0;

#ifndef MC3XXX_HAL_DRIVER_USING_IOCTL
    char buffer[2];
	buffer[0] = '\0';
	buffer[1] = '\0';
#endif

	/* handle check */
	if (handle != ID_A) {
		ALOGE("AccSensor: Invalid handle (%d)", handle);
		return -EINVAL;
	}

#ifdef MC3XXX_HAL_DRIVER_USING_IOCTL
	if (mEnabled <= 0) {
		if (enabled) {
			ALOGD("AccSensor:enable ioctl");
			err = ioctl(dev_fd, MC3XXX_ACC_IOCTL_SET_ENABLE,
				  &enabled);
			opDone = 1;
			ALOGD("AccSensor:enable ioctl done");
		}
	} else if (mEnabled == 1) {
		if (!enabled) {
			ALOGD("AccSensor:enable ioctl false");
			err = ioctl(dev_fd, MC3XXX_ACC_IOCTL_SET_ENABLE,
				  &enabled);
			opDone = 1;
		}
    }
#else
	if (mEnabled<= 0) {
		if(enabled) buffer[0] = '1';
	} else if (mEnabled == 1) {
		if(!enabled) buffer[0] = '0';
	}

    if (buffer[0] != '\0') {
		strcpy(&input_sysfs_path[input_sysfs_path_len], "enable");
		err = write_sys_attribute(input_sysfs_path, buffer, 1);
		//err = write_sys_attribute(sysfs_enable, buffer, 1);
        opDone = 1;
    }
#endif
	if (err != 0) {
		ALOGE("AccSensor: IOCTL failed (%s)", strerror(errno));
		return err;
	}
	if (opDone) {
		ALOGD("AccSensor: Control set %d", enabled);
		setInitialState();
	}

	if (enabled) {
		mEnabled++;
		if (mEnabled > 32767)
			mEnabled = 32767;
	} else {
		mEnabled--;
		if (mEnabled < 0)
			mEnabled = 0;
	}
	ALOGD("AccSensor(Acc_Mc3xxx.cpp): mEnabled = %d", mEnabled);

	return err;
}

int AccSensor::setDelay(int32_t handle, int64_t delay_ns)
{
	int err = 0;
	int ms;

#ifndef MC3XXX_HAL_DRIVER_USING_IOCTL
	char buffer[32];
	int bytes;
#endif

	/* handle check */
	if (handle != ID_A) {
		ALOGE("AccSensor: Invalid handle (%d)", handle);
		return -EINVAL;
	}

	if (mDelay != delay_ns) {
		ms = delay_ns / 1000000;
#ifdef MC3XXX_HAL_DRIVER_USING_IOCTL
		if (ioctl(dev_fd, MC3XXX_ACC_IOCTL_SET_DELAY, &ms)) {
#else
		bytes = sprintf(buffer, "%d", ms);
		strcpy(&input_sysfs_path[input_sysfs_path_len], "delay");
		if (write_sys_attribute(input_sysfs_path, buffer, bytes)){
		//if (write_sys_attribute(sysfs_poll, buffer, bytes)){
#endif
			return -errno;
		}
		mDelay = delay_ns;
	}

	ALOGD("AccSensor(Acc_Mc3xxx.cpp): delay = %d", delay_ns);
	
	return err;
}

int64_t AccSensor::getDelay(int32_t handle)
{
	return (handle == ID_A) ? mDelay : 0;
}

int AccSensor::getEnable(int32_t handle)
{
	return (handle == ID_A) ? mEnabled : 0;
}

int AccSensor::readEvents(sensors_event_t * data, int count)
{
	if (count < 1)
		return -EINVAL;

	if (mHasPendingEvent) {
		mHasPendingEvent = false;
		mPendingEvent.timestamp = getTimestamp();
		*data = mPendingEvent;
		return mEnabled ? 1 : 0;
	}

	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;

	int numEventReceived = 0;
	input_event const *event;
	static float acc_latest_x;
	static float acc_latest_y;
	static float acc_latest_z;

	while (count && mInputReader.readEvent(&event)) {
		int type = event->type;
		if (type == EV_ABS) {
			float value = event->value;
			if (event->code == EVENT_TYPE_ACCEL_X) {
				//mPendingEvent.acceleration.x = ACC_UNIT_CONVERSION(value);
				acc_latest_x = ACC_UNIT_CONVERSION(value);
			} else if (event->code == EVENT_TYPE_ACCEL_Y) {
				//mPendingEvent.acceleration.y = ACC_UNIT_CONVERSION(value);
				acc_latest_y = ACC_UNIT_CONVERSION(value);
			} else if (event->code == EVENT_TYPE_ACCEL_Z) {
				//mPendingEvent.acceleration.z = ACC_UNIT_CONVERSION(value);
				acc_latest_z = ACC_UNIT_CONVERSION(value);
			}
		} else if (type == EV_SYN) {
			mPendingEvent.timestamp = timevalToNano(event->time);
			if (mEnabled) {
				mPendingEvent.acceleration.x = acc_latest_x;
				mPendingEvent.acceleration.y = acc_latest_y;
				mPendingEvent.acceleration.z = acc_latest_z;
				mSensorCoordinate.coordinate_data_convert(
						mPendingEvent.acceleration.v, INSTALL_DIR);
				*data++ = mPendingEvent;
				count--;
				numEventReceived++;
			}
		} else {
			ALOGE("AccSensor: unknown event (type=%d, code=%d)",
			      type, event->code);
		}
		mInputReader.next();
	}

	return numEventReceived;
}

int AccSensor::populateSensorList(struct sensor_t *list)
{
	memcpy(list, sSensorList, sizeof(struct sensor_t) * numSensors);
	return numSensors;
}
