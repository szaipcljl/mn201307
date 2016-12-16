#include"calibrate.h"
#include "sensorthread.h"

SENSOR_DATA_T data_loop_acc;
SENSOR_DATA_T data_loop_gyr;
SENSOR_DATA_T data_loop_mag;

extern struct my_fifo acc_fifo;
extern struct my_fifo gyr_fifo;
extern struct my_fifo mag_fifo;
#define FIFO_SIZE (1 << 4)

extern int apk_exit;
extern int enable_finish;
#ifdef DEBUG_USE_ADB
extern int print_acc_log;
extern int print_gyr_log;
extern int print_mag_log;
extern int calc_gyr_integral;
extern SENSOR_CALC_TOOL calc_tool[3];
extern char* reqSnr_g[3];
#define NS_TO_S (float)(0.000000001)
#define NS_TO_MS (float)(0.000001)
#endif

bool SensorThread::threadLoop()
{
	ASensorEvent event[16];
	ssize_t n;

	//ALOGD("enter threadLoop(); mTypes:%d\n", mTypes);
    if(mSamples == 0) {
        requestExit();
        return false;
    }


	mSensorEventQueue->waitForEvent();
	n = mSensorEventQueue->read(event, 16);

    if (mSamples != 0 && n > 0) {
		for (int i = 0; i < n; i++) {

			/* 
			 * For Android KK, there is a META_DATA type to indicate flush
			 * operation 
			 */
			if (event[i].type == SENSOR_TYPE_META_DATA) {
				continue;
			}

			switch (event[i].type) {
			case SENSOR_TYPE_ACC_RAW:

				/*
				 * ensure readAccSensor() can read acc data, just use
				 * data_loop_acc to save acc data
				 */

				if (event[i].timestamp == data_loop_acc.snr_time.acc_time)
					break;
				
				data_loop_acc.data.accelMilliG.accelX = (int)(event[i].data[0]); //float * 1000 => int
				data_loop_acc.data.accelMilliG.accelY = (int)(event[i].data[1]);
				data_loop_acc.data.accelMilliG.accelZ = (int)(event[i].data[2]);
				data_loop_acc.snr_time.acc_time = event[i].timestamp;
				myfifo_in(&acc_fifo, (void*)&data_loop_acc, 1);


#ifdef DEBUG_USE_ADB
				if (print_acc_log)
#endif
					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, sensor=%s\n",\
							data_loop_acc.data.accelMilliG.accelX,\
							data_loop_acc.data.accelMilliG.accelY,\
							data_loop_acc.data.accelMilliG.accelZ,\
							event[i].timestamp, getSensorName(event[i].type));



				break;

			case SENSOR_TYPE_GYRO_RAW:

				if (event[i].timestamp == data_loop_gyr.snr_time.gyr_time)
					break;

				data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX = (int)(event[i].data[0]);
				data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY = (int)(event[i].data[1]);
				data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ = (int)(event[i].data[2]);
				data_loop_gyr.snr_time.gyr_time = event[i].timestamp;
				myfifo_in(&gyr_fifo, (void*)&data_loop_gyr, 1);


#ifdef DEBUG_USE_ADB
				if (!calc_gyr_integral)
					break;
				
				if (calc_tool[1].gyro_angle.stoptime == 0) {
					calc_tool[1].gyro_angle.starttime = event[i].timestamp;
					calc_tool[1].gyro_angle.stoptime = event[i].timestamp;
				}

				calc_tool[1].gyro_angle.delta_T = event[i].timestamp - calc_tool[1].gyro_angle.stoptime;

				if (0 != calc_tool[1].gyro_angle.delta_T) {
					calc_tool[1].gyro_angle.agl_x += ((float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)) * 0.001;
					calc_tool[1].gyro_angle.agl_y += ((float)event[i].data[1] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)) * 0.001;
					calc_tool[1].gyro_angle.agl_z += ((float)event[i].data[2] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)) * 0.001;

					calc_tool[1].gyro_angle.stoptime = event[i].timestamp;
					calc_tool[1].gyro_angle.time_step = calc_tool[1].gyro_angle.stoptime - calc_tool[1].gyro_angle.starttime;

					ALOGD("\n");
#if 0
					ALOGD("event[i].data[0]        = %lf (lf)\n", event[i].data[0]); //type 'double' -> '%lf'
					ALOGD("(int)event[i].data[0]   = %9d (9d)\n", (int)event[i].data[0]);
					ALOGD("(float)event[i].data[0] = %0.9f\n", (float)event[i].data[0]);

					ALOGD("(float)calc_tool[1].gyro_angle.delta_T * NS_TO_S = %0.9f\n)",\
							(float)calc_tool[1].gyro_angle.delta_T * NS_TO_S);

					ALOGD("(float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)           = %0.9f\n",
							(float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S));
					ALOGD("((float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)) * 0.001 = %0.9f\n",
							((float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S)) * 0.001);

					ALOGD("calc_tool[1].gyro_angle.agl_x = %0.9f\n", calc_tool[1].gyro_angle.agl_x);
#endif
					ALOGD("[gyro_angle]:<agl_x = %0.9f,\tagl_y = %0.9f,\tagl_z = %0.9f>\n\t"\
							"<starttime = %lld,\tstoptime = %lld>\n\t<delta-T = %lld,\ttime_step = %lld>\n"\
							, calc_tool[1].gyro_angle.agl_x
							, calc_tool[1].gyro_angle.agl_y
							, calc_tool[1].gyro_angle.agl_z
							, calc_tool[1].gyro_angle.starttime
							, calc_tool[1].gyro_angle.stoptime
							, calc_tool[1].gyro_angle.delta_T
							, calc_tool[1].gyro_angle.time_step);
				}

				if (print_gyr_log)
#endif
					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, sensor=%s\n",\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX,\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY,\
							data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ,\
							event[i].timestamp, getSensorName(event[i].type));

				break;

			case SENSOR_TYPE_COMPS_RAW:

				if (event[i].timestamp == data_loop_mag.snr_time.mag_time)
					break;

				data_loop_mag.data.magFieldMilliGauss.magFieldX = (int)(event[i].data[0]);
				data_loop_mag.data.magFieldMilliGauss.magFieldY = (int)(event[i].data[1]);
				data_loop_mag.data.magFieldMilliGauss.magFieldZ = (int)(event[i].data[2]);
				data_loop_mag.snr_time.mag_time = event[i].timestamp;
				myfifo_in(&mag_fifo, (void *)&data_loop_mag, 1);


#ifdef DEBUG_USE_ADB
				if (print_mag_log)
#endif
					ALOGD("[sensorthread]:value=<%9d,%9d,%9d>, time=%lld, accuracy=%d, sensor=%s\n",\
							data_loop_mag.data.magFieldMilliGauss.magFieldX,\
							data_loop_mag.data.magFieldMilliGauss.magFieldY,\
							data_loop_mag.data.magFieldMilliGauss.magFieldZ,\
							event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));

				break;

#ifdef DEBUG_USE_ADB
			case SENSOR_TYPE_GYROSCOPE:
				if (calc_tool[1].gyro_angle.stoptime == 0) {
					calc_tool[1].gyro_angle.starttime = event[i].timestamp;
					calc_tool[1].gyro_angle.stoptime = event[i].timestamp;
				}

				calc_tool[1].gyro_angle.delta_T = event[i].timestamp - calc_tool[1].gyro_angle.stoptime;

				if (0 != calc_tool[1].gyro_angle.delta_T) {
					calc_tool[1].gyro_angle.agl_x += ((float)event[i].data[0] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S));
					calc_tool[1].gyro_angle.agl_y += ((float)event[i].data[1] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S));
					calc_tool[1].gyro_angle.agl_z += ((float)event[i].data[2] * ((float)calc_tool[1].gyro_angle.delta_T * NS_TO_S));

					calc_tool[1].gyro_angle.stoptime = event[i].timestamp;
					calc_tool[1].gyro_angle.time_step = calc_tool[1].gyro_angle.stoptime - calc_tool[1].gyro_angle.starttime;

					ALOGD("\n");
					ALOGD("[gyro_angle]:<agl_x = %0.9f,\tagl_y = %0.9f,\tagl_z = %0.9f>\n\t"\
							"<starttime = %lld,\tstoptime = %lld>\n\t<delta-T = %lld,\ttime_step = %lld>\n"\
							, calc_tool[1].gyro_angle.agl_x
							, calc_tool[1].gyro_angle.agl_y
							, calc_tool[1].gyro_angle.agl_z
							, calc_tool[1].gyro_angle.starttime
							, calc_tool[1].gyro_angle.stoptime
							, calc_tool[1].gyro_angle.delta_T
							, calc_tool[1].gyro_angle.time_step);
				}

				if (print_gyr_log)
					ALOGD("[sensorthread]:value=<%0.9f,%0.9f,%0.9f>, time=%lld, sensor=%s\n",\
							event[i].data[0], event[i].data[1], event[i].data[2],
							event[i].timestamp, getSensorName(event[i].type));


				break;
#endif

			default:
				ALOGD("value=<%9.4f,%9.4f,%9.4f>, time=%lld, sensor=%s\n",
						event[i].data[0], event[i].data[1], event[i].data[2],
						event[i].timestamp, getSensorName(event[i].type));
				break;
			}

			mSamples--;
			if (mSamples == 0)
				break;

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

int getSensorType(char const* type) {
    if (strcmp(type, "Acc") == 0) {
        return SENSOR_TYPE_ACCELEROMETER;
    }
    else if (strcmp(type, "Mag") == 0) {
        return SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
    }
    else if (strcmp(type, "MagCal") == 0) {
        return SENSOR_TYPE_MAGNETIC_FIELD;
    }
    else if (strcmp(type, "Ori") == 0) {
        return SENSOR_TYPE_ORIENTATION;
    }
    else if (strcmp(type, "Gyr") == 0) {
        return SENSOR_TYPE_GYROSCOPE;
    }
    else if (strcmp(type, "Lux") == 0) {
        return SENSOR_TYPE_LIGHT;
    }
    else if (strcmp(type, "Bar") == 0) {
        return SENSOR_TYPE_PRESSURE;
    }
    else if (strcmp(type, "Tmp") == 0) {
        return SENSOR_TYPE_TEMPERATURE;
    }
    else if (strcmp(type, "Prx") == 0) {
        return SENSOR_TYPE_PROXIMITY;
    }
    else if (strcmp(type, "Grv") == 0) {
        return SENSOR_TYPE_GRAVITY;
    }
    else if (strcmp(type, "Lac") == 0) {
        return SENSOR_TYPE_LINEAR_ACCELERATION;
    }
    else if (strcmp(type, "Rot") == 0) {
        return SENSOR_TYPE_ROTATION_VECTOR;
    }
    else if (strcmp(type, "Hum") == 0) {
        return SENSOR_TYPE_RELATIVE_HUMIDITY;
    }
    else if (strcmp(type, "Tam") == 0) {
        return SENSOR_TYPE_AMBIENT_TEMPERATURE;
    }
    else if (strcmp(type, "Rmx") == 0) {
        return SENSOR_TYPE_ROTATION_MATRIX;
    }
	else if (strcmp(type, "Acc_raw") == 0) {
		return SENSOR_TYPE_ACC_RAW;
	}
	else if(strcmp(type, "Gyro_raw") == 0) {
		return SENSOR_TYPE_GYRO_RAW;
	}
	else if(strcmp(type, "Comps_raw") == 0) {
		return SENSOR_TYPE_COMPS_RAW;
	}
    else if (isdigit(type[0])) {
        return atoi(type);
    }

    return -1;
}

static void SensorList2SensorCollection(SensorCollection& AllSensor,
                                        Sensor const* const* lpList, int Count)
{
    int Loop_i;

    if (!lpList || 0 == Count) {
        return;
    }

    for (Loop_i = 0; Loop_i < Count; ++Loop_i) {
        AllSensor.push_back(SensorAttribute(lpList[Loop_i]));
    }

    return;
}

static void DumpExistSensor(SensorCollection& AllSensor)
{
    SensorCollection::iterator IterBegin = AllSensor.begin();
    SensorCollection::iterator IterEnd = AllSensor.end();

    if (AllSensor.empty()) {
        ALOGD("Not found any exist sensors.\n");
    }

    IterEnd = AllSensor.end();
    for (; IterBegin != IterEnd; ++IterBegin) {
        ALOGD("Name: %-32s| Vendor: %-28s | Handle: %10d | type: %5d | parameter name: %s\n",
               (*IterBegin).m_lp_sensor->getName().string(),
               (*IterBegin).m_lp_sensor->getVendor().string(),
               (*IterBegin).m_lp_sensor->getHandle(),
               (*IterBegin).m_lp_sensor->getType(),
               getSensorName((*IterBegin).m_lp_sensor->getType())
               );
    }

    return;
}
static bool IsNeedWakup(int Type)
{
    if (Type == SENSOR_TYPE_PROXIMITY
        || Type == SENSOR_TYPE_SIGNIFICANT_MOTION
        || Type == SENSOR_TYPE_TILT_DETECTOR
        || Type == SENSOR_TYPE_WAKE_GESTURE
        || Type == SENSOR_TYPE_GLANCE_GESTURE
        || Type == SENSOR_TYPE_PICK_UP_GESTURE) {
        return true;
    }

    return false;
}

static bool SelectRequestSensor(SensorCollection& AllSensor,
                                std::vector<std::string>& RequestSensor)
{
    unsigned int Loop_i = 0;
    int SensorType;
    int IsWakeup;
    bool RetValue;

    SensorCollection::iterator Begin, End;

    RetValue = false;
    for(; Loop_i < RequestSensor.size(); ++Loop_i) {
        /*
          get type by parameter name
          check wake up state by type

         */
        SensorType = getSensorType(RequestSensor[Loop_i].c_str());
        if (-1 == SensorType) {
            ALOGD("Unknow sensor name: %s, Ignored.\n",
                   RequestSensor[Loop_i].c_str());
            continue;
        }

        IsWakeup = IsNeedWakup(SensorType);

        /*
          get special sensor by both type and wake up
         */
        Begin = AllSensor.begin();
        End = AllSensor.end();
        for (; Begin != End; ++Begin) {
            ALOGD("gettype() = %d, iswakup = %d | Sensortype = %d, wakup = %d \n",
                   (*Begin).m_lp_sensor->getType(),
                   (*Begin).m_lp_sensor->isWakeUpSensor(),
                   SensorType, IsWakeup);

            if ((*Begin).m_lp_sensor->getType() == SensorType
                && (*Begin).m_lp_sensor->isWakeUpSensor() == IsWakeup) {
                (*Begin).m_IsEnable = true;
                (*Begin).m_IsRequestWakeUp = IsWakeup;
                RetValue = true;
            }
        }
    }

    ALOGD("RetValue = %d\n", RetValue);
    return RetValue;
}

static void EnableSelectedSensor(SensorCollection& AllSensor,
                                 sp<SensorEventQueue> SensorDataQueue,
                                 int DelayTime, int BatchTime)
{
    SensorCollection::iterator Begin, End;

    int Delay = ns2us(ms2ns(DelayTime));
    int Batch = ns2us(ms2ns(BatchTime));
    int IsFiFo;

    End = AllSensor.end();
    for(Begin = AllSensor.begin(); Begin != End; ++Begin) {
		if ((*Begin).m_IsEnable) {
            IsFiFo = (*Begin).m_IsRequestWakeUp ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0;
            SensorDataQueue->enableSensor((*Begin).m_lp_sensor->getHandle(),
                                         Delay, Batch, IsFiFo);
            ALOGD("Enabled Sensor: %s \n",
                   (*Begin).m_lp_sensor->getName().string());
        }
    }
}


static void DisableSelectedSensor(SensorCollection& AllSensor,
                                  sp<SensorEventQueue> SensorDataQueue)
{
    SensorCollection::iterator Begin, End;

    End = AllSensor.end();
    for(Begin = AllSensor.begin(); Begin != End; ++Begin) {
        if ((*Begin).m_IsEnable) {
            SensorDataQueue->disableSensor((*Begin).m_lp_sensor);
            ALOGD("Disable Sensor: %s \n",
                   (*Begin).m_lp_sensor->getName().string());
        }
    }
}

void *sensorAGM_read_data_loop(void *arg)
{
	int ret;
    char const* action = "measure";
    int delay = 20, sample = -1, verbose = 0, show_list = 0;
    int batch_time_ms = 0, wakeup=0;

	ret = myfifo_alloc(&acc_fifo, FIFO_SIZE, sizeof(SENSOR_DATA_T));
	if (ret) {
		ALOGD("failed to alloc acc_fifo.ret = %d\n", ret);
		return NULL;
	}

	ret = myfifo_alloc(&gyr_fifo, FIFO_SIZE, sizeof(SENSOR_DATA_T));
	if (ret) {
		ALOGD("failed to alloc gyr_fifo.ret = %d\n", ret);
		return NULL;
	}

	ret = myfifo_alloc(&mag_fifo, FIFO_SIZE, sizeof(SENSOR_DATA_T));
	if (ret) {
		ALOGD("failed to alloc mag_fifo.ret = %d\n", ret);
		return NULL;
	}

    sp<SensorThread> sensor_thread;
    SensorManager& mgr(SensorManager::getInstance());
    sp<SensorEventQueue> queue = mgr.createEventQueue();

    SensorCollection AllSensor;
    std::vector<std::string> sensor_name_list;

    Sensor const* const* list;
    int count;

    int incalibrate = 0;

    if (queue == NULL) {
        ALOGD("createEventQueue returned NULL\n");
        return 0;
    } else {
        count = mgr.getSensorList(&list);
        SensorList2SensorCollection(AllSensor, list, count);
    }

	/* just open 1 sensor for easy test */
	int type;
	int numSkipSensors = 0;
	Sensor const *sensor = NULL;


	/*
	   Save all request sensor name to vector
	   */
#if DEBUG_USE_ADB
	int i;
	for (i = 0; i < 3 && reqSnr_g[i] != NULL; i++) {
		ALOGD("request Sensor type string name: (%s)\n", reqSnr_g[i]);
		sensor_name_list.push_back(reqSnr_g[i]);
	}
#else
	char* reqSnr[3] = {"Acc_raw", "Gyro_raw", "Comps_raw"};
	int i;
	for (i = 0; i < 3; i++) {
		ALOGD("request Sensor type string name: (%s)\n", reqSnr[i]);
		sensor_name_list.push_back(reqSnr[i]);
	}
#endif

	if (!SelectRequestSensor(AllSensor,
				sensor_name_list)) {
		ALOGD("No Sensor want to been enable.\n");
		return NULL;
	}

	if (strcmp(action, "calibrate") == 0) {
		sample = -1;
		//incalibrate =1;
		incalibrate =0;
	}

	sensor_thread = new SensorThread(queue, sample, incalibrate);
	if (sensor_thread == NULL) {
		ALOGD("failed to create sensor thread\n");
		return 0;
	}

	if (incalibrate == 0) {
		sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
	}




	EnableSelectedSensor(AllSensor, queue,
			delay, batch_time_ms);


	enable_finish = 1;
	sensor_thread->join();
	if (verbose) {
		ALOGD("sensor thread terminated\n");
	}

	DisableSelectedSensor(AllSensor, queue);

	myfifo_free(&acc_fifo);
	myfifo_free(&gyr_fifo);
	myfifo_free(&mag_fifo);

	return NULL;
}
