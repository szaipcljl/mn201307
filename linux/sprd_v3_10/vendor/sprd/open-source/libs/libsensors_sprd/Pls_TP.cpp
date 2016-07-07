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

#define LOG_NDEBUG 0

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "TP-P"
#endif

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>

#include <cutils/log.h>

#include "PlsSensor.h"
#include "sensors.h"


#define SYSFS_PATH_CLASS_CONTROL  "/sys/board_properties/facemode"


#define DEVICE_HWINFO_PATH "/sys/board_properties/tp_information"

typedef struct {
	const char* vendor;
	const char* dev_name;
}tp_info;

const tp_info tp_dev_info[] = {
	{"FT6336GU", "focaltech_ts"},
	{"FT6x36", "focaltech_ts"},
	{"FT6x06", "focaltech_ts"},
	{"FT5206", "focaltech_ts"},
	{"MSG2133A", "msg2133a_ts"},
	{"MSG22XX", "msg2xxx_ts"},
	{"MSG28XX", "msg2xxx_ts"},
	{"GT9147", "goodix_ts"},
	{"GT9157", "goodix_ts"},
	{"himax 852Xes", "himax-touchscreen"},
	{NULL,NULL},	//dont remove!
};

const char* GetTpInputName(void)
{
	int fd = -1;
	char tp[256];
	int ret=-1;
	int idx=0;

	memset((void*)tp,0,256);
	fd = open(DEVICE_HWINFO_PATH,O_RDONLY);
	if(fd < 0){
		ALOGE("open tp hw_info failed!!");
		return 0;
	}
	ret = read(fd,tp,sizeof(tp));
	close(fd);
	if(ret < 0){
		ALOGE("Get TP HWinfo failed!!");
		return 0;
	}

	while(tp_dev_info[idx].vendor != NULL){
		if(strstr(tp,tp_dev_info[idx].vendor)!=NULL)	{
			ALOGE("TP vendor is %s\n",tp_dev_info[idx].vendor);
			return tp_dev_info[idx].dev_name;
		}
		idx++;
	}
	return tp_dev_info[idx].dev_name;
}

/*****************************************************************************/

static struct sensor_t sSensorList[] = {
    {
        "TP Proximity sensor",
        "LITEON",

		1,
		SENSORS_LIGHT_HANDLE,
		SENSOR_TYPE_LIGHT,
		1.0f,
		102400.0f,
		0.5f,
		0,
		0,
		0,
		SENSOR_STRING_TYPE_LIGHT,
		"",
		0,
		SENSOR_FLAG_ON_CHANGE_MODE,
		{}
	},
	{
		"TP Proximity sensor",
        "LITEON",
        1,
        SENSORS_PROXIMITY_HANDLE,
        SENSOR_TYPE_PROXIMITY,
        1.0f,
        1.0f,
        0.005f,
	 	0,
		0,
	 	0, 
	 	SENSOR_STRING_TYPE_PROXIMITY,
	 	"",
	 	0,
	 	SENSOR_FLAG_ON_CHANGE_MODE,
	 	{ } 
   },
};

PlsSensor::PlsSensor()
    : SensorBase(NULL, (const char*)GetTpInputName()/*"msg2xxx_ts"*/),
    mEnabled(0),
    mPendingMask(0),
    mInputReader(32),
    mHasPendingEvent(false)
{
    memset(mPendingEvents, 0, sizeof(mPendingEvents));

    mPendingEvents[Proximity].version = sizeof(sensors_event_t);
    mPendingEvents[Proximity].sensor = ID_P;
    mPendingEvents[Proximity].type = SENSOR_TYPE_PROXIMITY;

    for (int i = 0; i < numSensors; i++)
    {
        mDelays[i] = 200000000; // 200 ms by default
    }

    sprintf(mSysFsPath_Enable, "%s", SYSFS_PATH_CLASS_CONTROL);
}

PlsSensor::~PlsSensor()
{
}

bool PlsSensor::hasPendingEvents() const
{
    return mHasPendingEvent;
}

int PlsSensor::setDelay(int32_t handle, int64_t ns)
{
    return 0;
}

int PlsSensor::setEnable(int32_t handle, int enabled)
{
    int bytes;
    char buffer[8];

    int err = 0;

    if (handle != ID_P)
    {
        //ALOGE("PlsSensor: Invalid handle (%d)", handle);
        return -EINVAL;
    }

    //ALOGD("PlsSensor::enabled =%d; mEnabled=%d\n", enabled, mEnabled);

    if (mEnabled <= 0)
    {
        if (enabled)
        {
            bytes = sprintf(buffer, "%d\n", 1) + 1;
            err = write_sys_attribute(mSysFsPath_Enable, buffer, bytes);
        }
    }
    else if (mEnabled == 1)
    {
        if (!enabled)
        {
            bytes = sprintf(buffer, "%d\n", 0) + 1;
            err = write_sys_attribute(mSysFsPath_Enable, buffer, bytes);
        }
    }
    if (err != 0)
    {
        //ALOGE("PlsSensor: write sysfs failed (%s)", strerror(errno));
        return err;
    }

    if (enabled)
    {
        mEnabled++;
        if (mEnabled > 32767)
        {
            mEnabled = 32767;
        }
    }
    else
    {
        mEnabled--;
        if (mEnabled < 0)
        {
            mEnabled = 0;
        }
    }
    //ALOGD("PlsSensor: mEnabled = %d", mEnabled);

    return err;
}

int PlsSensor::getEnable(int32_t handle)
{
    int enable=0;
    int what = -1;
    switch (handle) {
        case ID_P: what = Proximity; break;
    }

    if (uint32_t(what) >= numSensors)
        return -EINVAL;

    enable = mEnabled & (1 << what);

    if(enable > 0)
        enable = 1;

    //ALOGD("PlsSensor::mEnabled=0x%x; enable=%d\n", mEnabled, enable);

    return enable;
}

int PlsSensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            processEvent(event->code, event->value);
            mInputReader.next();
        } else if (type == EV_SYN) {
            int64_t time = timevalToNano(event->time);
            for (int j=0 ; count && mPendingMask && j<numSensors ; j++) {
                if (mPendingMask & (1<<j)) {
                    mPendingMask &= ~(1<<j);
                    mPendingEvents[j].timestamp = time;
                    if (mEnabled & (1<<j)) {
                        *data++ = mPendingEvents[j];
                        count--;
                        numEventReceived++;
                        //ALOGD("PlsSensor: report light value = %f",mPendingEvents[j].light);  //tao.feng
                    }
                }
            }
            if (!mPendingMask) {
                mInputReader.next();
            }
        } else {
            //ALOGE("Apds9900Sensor: unknown event (type=%d, code=%d)", type, event->code);
            mInputReader.next();
        }
    }

    return numEventReceived;
}
void PlsSensor::getChipInfo(char *buf){}
void PlsSensor::processEvent(int code, int value)
{
    switch (code) {
        case EVENT_TYPE_PROXIMITY:
            mPendingMask |= 1<<Proximity;
            mPendingEvents[Proximity].distance = value;
            //ALOGD("PlsSensor: mPendingEvents[Proximity].distance = %f",mPendingEvents[Proximity].distance);
            break;
        default:
            //ALOGD("PlsSensor: default value = %d",value);
            break;
    }
}

int PlsSensor::populateSensorList(struct sensor_t *list)
{
    memcpy(list, sSensorList, sizeof(struct sensor_t) * numSensors);

    return numSensors;
}
/*****************************************************************************/
