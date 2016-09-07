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

#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <utils/threads.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <gui/SensorEventQueue.h>

char const* getSensorName(int type);

using namespace android;

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


bool SensorThread::threadLoop()
{
    ASensorEvent event[16];
    ssize_t n;

    if(mSamples == 0) {
        requestExit();
        return false;
    }

    mSensorEventQueue->waitForEvent();
    n = mSensorEventQueue->read(event, 16);

    if (mSamples != 0 && n > 0) {
        for (int i = 0; i < n; i++) {
            /* For Android KK, there is a META_DATA type to indicate flush
               operation */
            if (event[i].type == SENSOR_TYPE_META_DATA) {
                continue;
            }
            switch (event[i].type) {
                case SENSOR_TYPE_MAGNETIC_FIELD:
                case SENSOR_TYPE_ORIENTATION:
                    if (mIncalibrate == 1) {
                        if ((event[i].magnetic.status == 3) && (mCount > 30)) {
                            printf("calibrate successful type %d\n", event[i].type);
                            requestExit();
                            return false;
                        }
                        else {
                            mCount++;
                            printf("calibrating, accuracy=%d, type %d\n", event[i].magnetic.status,event[i].type);
                        }
                    }
                    else {
                        printf("value=<%9.4f,%9.4f,%9.4f>, time=%lld, accuracy=%d, sensor=%s\n",
                               event[i].data[0], event[i].data[1], event[i].data[2],
                               event[i].timestamp, event[i].magnetic.status, getSensorName(event[i].type));
                    }
                    break;
                case SENSOR_TYPE_STEP_COUNTER:
                    printf("steps=%lld, time=%lld, sensor=%s\n",
                           event[i].u64.step_counter,
                           event[i].timestamp, getSensorName(event[i].type));
                    break;
                case SENSOR_TYPE_ROTATION_MATRIX:
                    printf("value=<%9.4f,%9.4f,%9.4f,\n       %9.4f,%9.4f,%9.4f,\n       %9.4f,%9.4f,%9.4f>\n",
                    //printf("value=<%d,%d,%d,\n       %d,%d,%d,\n       %d,%d,%d>\n",
                           event[i].data[0], event[i].data[1], event[i].data[2],
                           event[i].data[3], event[i].data[4], event[i].data[5],
                           event[i].data[6], event[i].data[7], event[i].data[8]);
                    printf("time=%lld, sensor=%s\n",
                           event[i].timestamp, getSensorName(event[i].type));
                    break;
                default:
                    printf("value=<%9.4f,%9.4f,%9.4f>, time=%lld, sensor=%s\n",
                           event[i].data[0], event[i].data[1], event[i].data[2],
                           event[i].timestamp, getSensorName(event[i].type));
                    break;
            }
            mSamples--;
            if (mSamples == 0)
                break;
        }
    }

    return true;
}


char const* short_options = "a:d:s:b:vlw";

struct option long_options[] = {
        {"action" ,  required_argument, 0,  'a'},
        {"delay"  ,  required_argument, 0,  'd'},
        {"sample" ,  required_argument, 0,  's'},
        {"verbose",  no_argument      , 0,  'v'},
        {"list"   ,  no_argument      , 0,  'l'},
        {"batch" ,   required_argument, 0,  'b'},
        {"wake"   ,  no_argument      , 0,  'w'},
        {0        ,  0                , 0,   0 }
};

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
    else if (isdigit(type[0])) {
        return atoi(type);
    }

    return -1;
}

int skipThisSensor(int type, int* types, int numTypes) {
    for (int i=0; i<numTypes; i++) {
        if (type == types[i]) {
            return 0;
        }
    }

    return numTypes != 0;
}

int main(int argc, char** argv)
{
    int err;

    char const* action = "measure";
    int delay = 20, sample = -1, verbose = 0, show_list = 0;
    int batch_time_ms = 0, wakeup=0;

    sp<SensorThread> sensor_thread;
    SensorManager &mgr(SensorManager::getInstance());
    sp<SensorEventQueue> queue = mgr.createEventQueue();
    Sensor const* const* list;
    int count;
    int incalibrate = 0;

    if (queue == NULL)
    {
        printf("createEventQueue returned NULL\n");
        return 0;
    }
    else
    {
        count = mgr.getSensorList(&list);
    }

    while (1) {
        int c = getopt_long(argc, argv,
                            short_options, long_options, NULL);

        if (c == -1){
            break;
        }

        switch (c) {
            case 'a':
                action = optarg;
                break;
            case 'd':
                delay = atoi(optarg);
                break;
            case 's':
                sample = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'l':
                show_list = 1;
                break;
            case 'b':
                batch_time_ms = atoi(optarg);
                break;
            case 'w':
                wakeup = 1;
                break;
            default:
                return 0;
        }
    }

    /* if list is set, just put the sensor list info */
    if (show_list) {
        printf("%d sensors found:\n", count);
        for (int i=0 ; i<count ; i++) {
            printf("%-32s| vendor: %-28s | handle: %10d | type: %5d | name: %s\n",
                   list[i]->getName().string(),
                   list[i]->getVendor().string(),
                   list[i]->getHandle(),
                   list[i]->getType(),
                   getSensorName(list[i]->getType()));
        }
        return 0;
    }

    if (strcmp(action, "set") == 0) {
#if 0
        // TODO: add set funtion
#else
        printf("'set' is not supported\n");
#endif
        return 0;
    }
    else if (strcmp(action, "get") == 0) {
#if 0
        // TODO: add get funtion
#else
        printf("'get' is not supported\n");
#endif
        return 0;
    }
    else if (strcmp(action, "selftest") == 0) {
#if 0
        // TODO: add selftest function
#else
        printf("'selftest' is not supported\n");
#endif
        return 0;
    }
        /* just open 1 sensor for easy test */
    else if ((strcmp(action, "measure") == 0) || (strcmp(action, "calibrate") == 0)) {
        int type;
        int numSkipSensors = 0;
        Sensor const *sensor = NULL;

        if(optind >= argc){
            printf("invalid cmd parameters\n");
            return 0;
        }

        if ((type = getSensorType(argv[optind])) == -1) {
            printf("invalid sensor type (%s)\n", argv[optind]);
            return 0;
        }
        if (strcmp(action, "calibrate") == 0) {
            sample = -1;
            //incalibrate =1;
            incalibrate =0;
        }
        sensor_thread = new SensorThread(queue, sample, incalibrate);
        if (sensor_thread == NULL){
            printf("failed to create sensor thread\n");
            return 0;
        }
        if (incalibrate == 0)
            sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);

        // For the following sensor types, return a wake-up sensor. These types are by default
        // defined as wake-up sensors. For the rest of the sensor types defined in sensors.h return
        // a non_wake-up version.
        if (type == SENSOR_TYPE_PROXIMITY || type == SENSOR_TYPE_SIGNIFICANT_MOTION ||
            type == SENSOR_TYPE_TILT_DETECTOR || type == SENSOR_TYPE_WAKE_GESTURE ||
            type == SENSOR_TYPE_GLANCE_GESTURE || type == SENSOR_TYPE_PICK_UP_GESTURE) {
            wakeup = true;
        }

        for (int i=0 ; i<count ; i++) {
            if (list[i]->getType() == type &&
                list[i]->isWakeUpSensor() == wakeup) {
                sensor = list[i];
                break;
            }
        }
        if (sensor == NULL){
            printf("get sensor of type:%d error\n", type);
            return 0;
        }
        if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
                                ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
            printf("enable sensor of type %d error\n", type);
            return 0;
        }

#if 0
        if (incalibrate == 1) {
            err = queue->calibrateSensor(sensor->getHandle());
            if (err){
                printf("force calibrate sensor of type %d error %d\n", type, err);
            }

            if (type == SENSOR_TYPE_MAGNETIC_FIELD || type == SENSOR_TYPE_ORIENTATION)
                sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
            else {
                if (!err)
                    printf("calibrate successful type %d\n", type);
                return 0;
            }
        }
#endif
        sensor_thread->join();
        if (verbose)
            printf("sensor thread terminated\n");

        err = queue->disableSensor(sensor);
        if (err != NO_ERROR) {
            printf("disableSensor() for '%s'failed (%d)\n",
                   getSensorName(type), err);
            return 0;
        }
    } else {
        printf("unknown action %s\n", action);
        return 0;
    }

    return 0;
}
