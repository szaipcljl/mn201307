#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <utils/threads.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <gui/SensorEventQueue.h>

#include"sensorcali.h"
#include "my_sensor.h"

using namespace android;
//int readSensorData(SENSOR_DATA_T& data,int type);
char const* getSensorName(int type);


class SensorThread : public Thread
{
	public:
		SensorThread(sp<SensorEventQueue>& queue, int type, int incalibrate)
			: mSensorEventQueue(queue), mTypes(type), mIncalibrate(incalibrate), mCount(0) { }
		~SensorThread() { }

		SENSOR_DATA_T* data;

	private:
		virtual bool threadLoop();
		sp<SensorEventQueue> mSensorEventQueue;
		int mTypes;
		int mIncalibrate;
		int mCount;
};

