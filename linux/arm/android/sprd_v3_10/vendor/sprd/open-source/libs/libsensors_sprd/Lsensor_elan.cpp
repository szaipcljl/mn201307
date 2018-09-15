/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2011 Sorin P. <sorin@hypermagik.com>
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

#include "LSensor.h"
#include "sensors.h"

#define ELAN_LS_DEVICE_NAME              "/dev/elan_als"
#if 0
#define ELAN_IOCTL_MAGIC                         0x1C
#define ELAN_EPL2812_IOCTL_GET_PFLAG _IOR(ELAN_IOCTL_MAGIC, 1, int *)
#define ELAN_EPL2812_IOCTL_GET_LFLAG _IOR(ELAN_IOCTL_MAGIC, 2, int *)
#define ELAN_EPL2812_IOCTL_ENABLE_PFLAG _IOW(ELAN_IOCTL_MAGIC, 3, int *)
#define ELAN_EPL2812_IOCTL_ENABLE_LFLAG _IOW(ELAN_IOCTL_MAGIC, 4, int *)
#define ELAN_EPL2812_IOCTL_GETDATA _IOR(ELAN_IOCTL_MAGIC, 5, int *)
#else
#define ELAN_IOCTL_MAGIC 'c'
#define ELAN_EPL8800_IOCTL_GET_PFLAG    _IOR(ELAN_IOCTL_MAGIC, 1, int)
#define ELAN_EPL8800_IOCTL_GET_LFLAG    _IOR(ELAN_IOCTL_MAGIC, 2, int)
#define ELAN_EPL8800_IOCTL_ENABLE_PFLAG _IOW(ELAN_IOCTL_MAGIC, 3, int)
#define ELAN_EPL8800_IOCTL_ENABLE_LFLAG _IOW(ELAN_IOCTL_MAGIC, 4, int)
#define ELAN_EPL8800_IOCTL_GETDATA      _IOW(ELAN_IOCTL_MAGIC, 5, int)
#endif

/*****************************************************************************/
static struct sensor_t sSensorList[] = {
	{ "ELAN Light sensor",
          "ELAN",
          1, SENSORS_LIGHT_HANDLE,
          SENSOR_TYPE_LIGHT, 1.0f,
          100000.0f, 0.005f, 0,0,0, 
		  		SENSOR_STRING_TYPE_LIGHT,
		"",
		0,
		SENSOR_FLAG_ON_CHANGE_MODE,{ } },
};

LSensor::LSensor() :
	SensorBase(ELAN_LS_DEVICE_NAME, "lightsensor-level"),
		mEnabled(0), mPendingMask(0), mInputReader(32), mHasPendingEvent(false)
{
	memset(mPendingEvents, 0, sizeof(mPendingEvents));

	mPendingEvents[Light].version = sizeof(sensors_event_t);
	mPendingEvents[Light].sensor = ID_L;
	mPendingEvents[Light].type = SENSOR_TYPE_LIGHT;

	for (int i = 0; i < numSensors; i++)
		mDelays[i] = 200000000;	// 200 ms by default
}

LSensor::~LSensor()
{
}

bool LSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int LSensor::setDelay(int32_t handle, int64_t ns)
{
	return 0;
}

int LSensor::setEnable(int32_t handle, int en)
{
	int what = -1;
	switch (handle) {
	case ID_L:
		what = Light;
		break;
	}
	if (uint32_t(what) >= numSensors)
		return -EINVAL;

	int newState = en ? 1 : 0;
	int err = 0;

	ALOGD
	    ("LSensor::enable en=%d; newState=%d; what=%d; mEnabled=%d\n",
	     en, newState, what, mEnabled);

	if ((uint32_t(newState) << what) != (mEnabled & (1 << what))) {
		if (!mEnabled)
			open_device();

		int cmd;

		switch (what) {
		case Light:
			#if 0
			cmd = ELAN_EPL2812_IOCTL_ENABLE_LFLAG;
			#else
			cmd = ELAN_EPL8800_IOCTL_ENABLE_LFLAG;
			#endif
			break;
		}

		int flags = newState;
		err = ioctl(dev_fd, cmd, &flags);
		err = err < 0 ? -errno : 0;
		ALOGD("LSensor::enable what=%d; flags=%d; err=%d\n",
		      what, flags, err);
		ALOGE_IF(err, "ECS_IOCTL_APP_SET_XXX failed (%s)",
			 strerror(-err));
		if (!err) {
			mEnabled &= ~(1 << what);
			mEnabled |= (uint32_t(flags) << what);
		}
		ALOGD("LSensor::mEnabled=0x%x\n", mEnabled);
		if (!mEnabled)
			close_device();
	}
	return err;
}

int LSensor::getEnable(int32_t handle)
{
	int enable = 0;
	int what = -1;
	switch (handle) {
	case ID_L:
		what = Light;
		break;
	}
	if (uint32_t(what) >= numSensors)
		return -EINVAL;
	enable = mEnabled & (1 << what);
	if (enable > 0)
		enable = 1;
	ALOGD("LSensor::mEnabled=0x%x; enable=%d\n", mEnabled, enable);
	return enable;
}

int LSensor::readEvents(sensors_event_t * data, int count)
{
	if (count < 1)
		return -EINVAL;
	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;
	int numEventReceived = 0;
	input_event const *event;
	while (count && mInputReader.readEvent(&event)) {
		int type = event->type;
		if (type == EV_ABS) {
			processEvent(event->code, event->value);
			mInputReader.next();
		} else if (type == EV_SYN) {
			int64_t time = timevalToNano(event->time);
			for (int j = 0;
			     count && mPendingMask && j < numSensors; j++) {
				if (mPendingMask & (1 << j)) {
					mPendingMask &= ~(1 << j);
					mPendingEvents[j].timestamp = time;
					if (mEnabled & (1 << j)) {
						*data++ = mPendingEvents[j];
						count--;
						numEventReceived++;
					}
				}
			}
			if (!mPendingMask) {
				mInputReader.next();
			}
		} else {
			ALOGE
			    ("LSensor: unknown event (type=%d, code=%d)",
			     type, event->code);
			mInputReader.next();
		}
	}
	return numEventReceived;
}

void LSensor::processEvent(int code, int value)
{
	switch (code) {
	case EVENT_TYPE_LIGHT:
		mPendingMask |= 1 << Light;
		mPendingEvents[Light].light = (float)value;
		ALOGD("LSensor: mPendingEvents[Light].light = %f",
		      mPendingEvents[Light].light);
		break;
	default:
		ALOGD("LSensor: default value = %d", value);
		break;
	}
}

int LSensor::populateSensorList(struct sensor_t *list)
{
	memcpy(list, sSensorList, sizeof(struct sensor_t) * numSensors);
	return numSensors;
}

/*****************************************************************************/
