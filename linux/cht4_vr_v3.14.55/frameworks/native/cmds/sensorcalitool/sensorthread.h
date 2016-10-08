#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <utils/threads.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <gui/SensorEventQueue.h>

#include <log/log.h>
#undef LOG_TAG
#define LOG_TAG "calitool"

/*
 * used for adb debug
 * Android.mk use "include $(BUILD_EXECUTABLE)" instead of
 * "include $(BUILD_SHARED_LIBRARY)"
 *
 */

#define DEBUG_USE_ADB 0
#if DEBUG_USE_ADB
#define ALOGD printf
#endif

#define SENSOR_TYPE_ACC_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+16)
#define SENSOR_TYPE_GYRO_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+17)
#define SENSOR_TYPE_COMPS_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+18)

char const* getSensorName(int type);

using namespace android;

class SensorThread : public Thread
{
	public:
		SensorThread(sp<SensorEventQueue>& queue, int type, int incalibrate)
			: mSensorEventQueue(queue), mTypes(type), mIncalibrate(incalibrate), mCount(0) { }
		~SensorThread() { }

	private:
		virtual bool threadLoop();
		sp<SensorEventQueue> mSensorEventQueue;
		int mTypes;
		int mIncalibrate;
		int mCount;
};
