#include"calibrate.h"
#include "sensorthread.h"

SENSOR_DATA_T data_loop_acc;
SENSOR_DATA_T data_loop_gyr;
SENSOR_DATA_T data_loop_mag;

extern int apk_exit;

bool SensorThread::threadLoop()
{
	ASensorEvent event[16];
	ssize_t n;

	ALOGD("enter threadLoop(); mTypes:%d\n", mTypes);

	if((mTypes != SENSOR_TYPE_ACC_RAW && mTypes != SENSOR_TYPE_GYRO_RAW && mTypes != SENSOR_TYPE_COMPS_RAW) || 1 == apk_exit) {
		requestExit();
		return false;
	}

	mSensorEventQueue->waitForEvent();
	n = mSensorEventQueue->read(event, 16);

	if (n > 0) {
		for (int i = 0; i < n; i++) {

			/* 
			 * For Android KK, there is a META_DATA type to indicate flush
			 * operation 
			 */
			if (event[i].type == SENSOR_TYPE_META_DATA) {
				continue;
			}

			if (event[i].type == mTypes) {

				switch (event[i].type) {

				case SENSOR_TYPE_ACC_RAW:

					/*
					 * ensure readAccSensor() can read acc data, just use
					 * data_loop_acc to save acc data
					 */
					data_loop_acc.data.accelMilliG.accelX = (int)(event[i].data[0] * 1000); //float * 1000 => int
					data_loop_acc.data.accelMilliG.accelY = (int)(event[i].data[1] * 1000);
					data_loop_acc.data.accelMilliG.accelZ = (int)(event[i].data[2] * 1000);

					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, accuracy=%d, sensor=%s\n",\
							data_loop_acc.data.accelMilliG.accelX,\
							data_loop_acc.data.accelMilliG.accelY,\
							data_loop_acc.data.accelMilliG.accelZ,\
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					break;

				case SENSOR_TYPE_GYRO_RAW:

					data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX = (int)(event[i].data[0] * 1000);
					data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY = (int)(event[i].data[1] * 1000);
					data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ = (int)(event[i].data[2] * 1000);

					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, accuracy=%d, sensor=%s\n",\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX,\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY,\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ,\
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					break;

				case SENSOR_TYPE_COMPS_RAW:

					data_loop_mag.data.magFieldMilliGauss.magFieldX = (int)(event[i].data[0] * 1000);
					data_loop_mag.data.magFieldMilliGauss.magFieldY = (int)(event[i].data[1] * 1000);
					data_loop_mag.data.magFieldMilliGauss.magFieldZ = (int)(event[i].data[2] * 1000);

					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, accuracy=%d, sensor=%s\n",\
							data_loop_mag.data.magFieldMilliGauss.magFieldX,\
							data_loop_mag.data.magFieldMilliGauss.magFieldY,\
							data_loop_mag.data.magFieldMilliGauss.magFieldZ,\
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					break;
				}
			}

			if(1 == apk_exit) {
				requestExit();
				return true;
			}
		}
	}

	return true;
}

char const* getSensorName(int type) {
	switch(type) {
	case SENSOR_TYPE_ACC_RAW:
		return "Acc_raw";
	case SENSOR_TYPE_GYRO_RAW:
		return "Gyr_raw";
	case SENSOR_TYPE_COMPS_RAW:
		return "Mag_raw";
	}

	static char buf[64];
	sprintf(buf, "%d", type);
	return buf;
}
