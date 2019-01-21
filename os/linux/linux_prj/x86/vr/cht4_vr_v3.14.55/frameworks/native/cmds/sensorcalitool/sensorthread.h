#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <utils/threads.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <gui/SensorEventQueue.h>
#include <vector>
#include <string>

#include <log/log.h>
#undef LOG_TAG
#define LOG_TAG "calitool"

/*
 * used for adb debug
 * Android.mk use "include $(BUILD_EXECUTABLE)" instead of
 * "include $(BUILD_SHARED_LIBRARY)"
 *
 */

#ifdef DEBUG_USE_ADB
#undef ALOGD
#define ALOGD printf
#endif

#define SENSOR_TYPE_ACC_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+16)
#define SENSOR_TYPE_GYRO_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+17)
#define SENSOR_TYPE_COMPS_RAW (SENSOR_TYPE_DEVICE_PRIVATE_BASE+18)

char const* getSensorName(int type);

using namespace android;

struct SensorAttribute
{
    SensorAttribute(Sensor const* lp_new_sensor):
        m_lp_sensor(lp_new_sensor), m_IsEnable(false)
    {;}

    Sensor const* m_lp_sensor;
    bool m_IsEnable;
    bool m_IsRequestWakeUp;
};

typedef std::vector<SensorAttribute> SensorCollection;

class SensorThread : public Thread
{
	public:
		SensorThread(sp<SensorEventQueue>& queue, int sample, int incalibrate)
			: mSensorEventQueue(queue), mSamples(sample), mIncalibrate(incalibrate), mCount(0) { }
		~SensorThread() { }

	private:
		virtual bool threadLoop();
		sp<SensorEventQueue> mSensorEventQueue;
		int mSamples;
		int mIncalibrate;
		int mCount;
};
