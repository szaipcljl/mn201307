#include "mySensorManager.h"


extern int flag_finish;
extern int flag_acc_finish;
extern int flag_gyr_finish;
extern int flag_mag_finish;
extern SENSOR_DATA_T data;
extern int type;


bool SensorThread::threadLoop()
{
	ASensorEvent event[16];
	ssize_t n;


	if(mTypes != SENSOR_TYPE_ACCELEROMETER && mTypes != SENSOR_TYPE_GYROSCOPE && mTypes != SENSOR_TYPE_MAGNETIC_FIELD) {
		requestExit();
		return false;
	}

	mSensorEventQueue->waitForEvent();
	n = mSensorEventQueue->read(event, 16);


	if (n > 0) {
		for (int i = 0; i < n; i++) {
			/* For Android KK, there is a META_DATA type to indicate flush
			   operation */
			if (event[i].type == SENSOR_TYPE_META_DATA) {
				continue;
			}

			if (event[i].type == mTypes) {
				switch (event[i].type) {

				case SENSOR_TYPE_ACCELEROMETER:
					printf("value=<%9.4f,%9.4f,%9.4f>, time=%lld, accuracy=%d, sensor=%s\n",
							event[i].data[0], event[i].data[1], event[i].data[2],
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					data->data.accelMilliG.accelX = event[i].data[0];
					data->data.accelMilliG.accelY = event[i].data[1];
					data->data.accelMilliG.accelZ = event[i].data[2];
					flag_acc_finish = 1;

					break;
				case SENSOR_TYPE_GYROSCOPE:
					printf("value=<%9.4f,%9.4f,%9.4f>, time=%lld, accuracy=%d, sensor=%s\n",
							event[i].data[0], event[i].data[1], event[i].data[2],
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					data->data.gyroMilliDegreesPerSecond.gyroX = event[i].data[0];
					data->data.gyroMilliDegreesPerSecond.gyroY = event[i].data[1];
					data->data.gyroMilliDegreesPerSecond.gyroZ = event[i].data[2];
					flag_gyr_finish = 1;

					break;

				case SENSOR_TYPE_MAGNETIC_FIELD:
					printf("value=<%9.4f,%9.4f,%9.4f>, time=%lld, accuracy=%d, sensor=%s\n",
							event[i].data[0], event[i].data[1], event[i].data[2],
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

					data->data.magFieldMilliGauss.magFieldX = event[i].data[0];
					data->data.magFieldMilliGauss.magFieldY = event[i].data[1];
					data->data.magFieldMilliGauss.magFieldZ = event[i].data[2];

					flag_mag_finish = 1;
					break;
				}

				flag_finish = 1;
				requestExit();
				return true;

			}
		}

	}

	return true;

}

char const* getSensorName(int type) {
	switch(type) {
	case SENSOR_TYPE_ACCELEROMETER:
		return "Acc";
	case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
		return "Mag";
	case SENSOR_TYPE_MAGNETIC_FIELD:
		return "MagCal";
	case SENSOR_TYPE_ORIENTATION:
		return "Ori";
	case SENSOR_TYPE_GYROSCOPE:
		return "Gyr";
	case SENSOR_TYPE_LIGHT:
		return "Lux";
	case SENSOR_TYPE_PRESSURE:
		return "Bar";
	case SENSOR_TYPE_TEMPERATURE:
		return "Tmp";
	case SENSOR_TYPE_PROXIMITY:
		return "Prx";
	case SENSOR_TYPE_GRAVITY:
		return "Grv";
	case SENSOR_TYPE_LINEAR_ACCELERATION:
		return "Lac";
	case SENSOR_TYPE_ROTATION_VECTOR:
		return "Rot";
	case SENSOR_TYPE_RELATIVE_HUMIDITY:
		return "Hum";
	case SENSOR_TYPE_AMBIENT_TEMPERATURE:
		return "Tam";
	case SENSOR_TYPE_ROTATION_MATRIX:
		return "Rmx";
	}

	static char buf[64];
	sprintf(buf, "%d", type);
	return buf;
}
