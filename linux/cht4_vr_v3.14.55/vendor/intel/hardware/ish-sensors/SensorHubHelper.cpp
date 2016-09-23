#include "ISHSensor.hpp"
#include "SensorConfig.h"
static const char *PROP_GESTURE_STR =     "{\"classMask\":63,\"level\":0,\"sensitivity\":1}";
static const char *PROP_LIFT_STR    =     "{\"classMask\":3,\"sensitivity\":1}";
static const char *PROP_INSTANTACT_STR =  "{\"classMask\":15,\"sensitivity\":0}";

void SensorHubHelper::getStartStreamingParameters(int sensorType, int &dataRate, int &bufferDelay, streaming_flag &flag)
{
        switch (sensorType) {
        case SENSOR_TYPE_PROXIMITY:
        case SENSOR_TYPE_SAR:
                flag = NO_STOP_WHEN_SCREEN_OFF;
        case SENSOR_TYPE_GESTURE_FLICK:
                dataRate = GF_SAMPLE_RATE;
                bufferDelay = GF_BUF_DELAY;
                break;
        case SENSOR_TYPE_TERMINAL:
                dataRate = TERM_RATE;
                bufferDelay = TERM_DELAY;
                break;
        case SENSOR_TYPE_SHAKE:
                dataRate = SHAKING_SAMPLE_RATE;
                bufferDelay = SHAKING_BUF_DELAY;
                break;
        case SENSOR_TYPE_SIMPLE_TAPPING:
                dataRate = STAP_SAMPLE_RATE;
                bufferDelay = STAP_BUF_DELAY;
                break;
        default:
                break;
        }
}

bool SensorHubHelper::setISHPropertyIfNeeded(int sensorType, struct sensor_hub_methods methods, handle_t handler)
{
        int sensitivity = 0;

        switch (sensorType) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_ORIENTATION:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_GRAVITY:
        case SENSOR_TYPE_LINEAR_ACCELERATION:
        case SENSOR_TYPE_ROTATION_VECTOR:
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
        case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
        case SENSOR_TYPE_PAN_ZOOM:
                if (methods.ish_set_property(handler, PROP_SENSITIVITY, (void *)&sensitivity) == ERROR_NONE)
                        return true;
                else
                        return false;
        case SENSOR_TYPE_LIGHT:
                sensitivity = 5;
                if (methods.ish_set_property(handler, PROP_SENSITIVITY, (void *)&sensitivity) == ERROR_NONE)
                        return true;
                else
                        return false;
        case SENSOR_TYPE_GESTURE_FLICK:
                if (methods.ish_set_property_with_size(handler, PROP_GENERIC_START, strlen(PROP_GESTURE_STR) + 1, (void*)PROP_GESTURE_STR) == ERROR_NONE)
                        return true;
                else
                        return false;
        case SENSOR_TYPE_LIFT:
        case SENSOR_TYPE_WAKE_GESTURE:
                if (methods.ish_set_property_with_size(handler, PROP_GENERIC_START, strlen(PROP_LIFT_STR) + 1, (void*)PROP_LIFT_STR) == ERROR_NONE)
                        return true;
                else
                        return false;
        case SENSOR_TYPE_INSTANT_ACTIVITY:
                if (methods.ish_set_property_with_size(handler, PROP_GENERIC_START, strlen(PROP_INSTANTACT_STR) + 1, (void*)PROP_INSTANTACT_STR) == ERROR_NONE)
                        return true;
                else
                        return false;

        default:
                return true;
        }
}

int SensorHubHelper::getGestureFlickEvent(struct gesture_flick_data data)
{
        return data.flick;
}

int SensorHubHelper::getTerminalEvent(struct tc_data data)
{
        if (data.state == 0)
                return SENSOR_EVENT_TYPE_TERMINAL_PORTRAIT_UP;
        else if (data.state == 1)
                return SENSOR_EVENT_TYPE_TERMINAL_HORIZONTAL_UP;
        else if (data.state == 2)
                return SENSOR_EVENT_TYPE_TERMINAL_PORTRAIT_DOWN;
        else if (data.state == 3)
                return SENSOR_EVENT_TYPE_TERMINAL_HORIZONTAL_DOWN;
        else if (data.state == 4)
                return SENSOR_EVENT_TYPE_TERMINAL_FACE_UP;
        else if (data.state == 5)
                return SENSOR_EVENT_TYPE_TERMINAL_FACE_DOWN;
        else
                return SENSOR_EVENT_TYPE_TERMINAL_UNKNOWN;
}

int SensorHubHelper::getShakeEvent(struct shaking_data data)
{
        return data.shaking;
}

int SensorHubHelper::getSimpleTappingEvent(struct stap_data data)
{
        return data.stap;
}

int SensorHubHelper::getMoveDetectEvent(struct md_data data)
{
        return data.state;
}

size_t SensorHubHelper::getUnitSize(int sensorType)
{
        switch (sensorType) {
        case SENSOR_TYPE_ACCELEROMETER:
                return sizeof(struct accel_data);
        case SENSOR_TYPE_ACC_SHAKE:
                return sizeof(struct accel_shake_data);
        case SENSOR_TYPE_MAGNETIC_FIELD:
                return sizeof(struct compass_raw_data);
        case SENSOR_TYPE_ORIENTATION:
                return sizeof(struct orientation_data);
        case SENSOR_TYPE_GYROSCOPE:
                return sizeof(struct gyro_raw_data);
        case SENSOR_TYPE_LIGHT:
                return sizeof(struct als_raw_data);
        case SENSOR_TYPE_PRESSURE:
                return sizeof(struct baro_raw_data);
        case SENSOR_TYPE_PROXIMITY:
        case SENSOR_TYPE_SAR:
                return sizeof(struct ps_phy_data);
        case SENSOR_TYPE_GRAVITY:
                return sizeof(struct gravity_data);
        case SENSOR_TYPE_LINEAR_ACCELERATION:
                return sizeof(struct linear_accel_data);
        case SENSOR_TYPE_ROTATION_VECTOR:
                return sizeof(struct rotation_vector_data);
        case SENSOR_TYPE_GESTURE_FLICK:
                return sizeof(struct gesture_flick_data);
        case SENSOR_TYPE_GESTURE:
                return sizeof(struct gs_data);
        case SENSOR_TYPE_TERMINAL:
                return sizeof(struct tc_data);
        case SENSOR_TYPE_PEDOMETER:
                return sizeof(struct pedometer_data);
        case SENSOR_TYPE_SHAKE:
                return sizeof(struct shaking_data);
        case SENSOR_TYPE_SIMPLE_TAPPING:
        case SENSOR_TYPE_GLANCE_GESTURE:
                return sizeof(struct stap_data);
        case SENSOR_TYPE_MOVE_DETECT:
                return sizeof(struct md_data);
        case SENSOR_TYPE_STEP_DETECTOR:
                return sizeof(struct stepdetector_data);
        case SENSOR_TYPE_STEP_COUNTER:
                return sizeof(struct stepcounter_data);
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
                return sizeof(struct sm_data);
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
                return sizeof(struct game_rotation_vector_data);
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
                return sizeof(struct geomagnetic_rotation_vector_data);
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                return sizeof(struct uncalib_compass_data);
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
                return sizeof(struct uncalib_gyro_data);
        case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
                return sizeof(struct uncalib_acc_data);
        case SENSOR_TYPE_MOTION_DETECT:
                return sizeof(struct motion_detect_data);
        case SENSOR_TYPE_PAN_ZOOM:
                return sizeof(struct pz_data);
        case SENSOR_TYPE_LIFT:
        case SENSOR_TYPE_WAKE_GESTURE:
                return sizeof(struct lift_data);
        case SENSOR_TYPE_INSTANT_ACTIVITY:
                return sizeof(struct instant_activity_data);
        case SENSOR_TYPE_TILT_DETECTOR:
                return sizeof(struct tilt_data);
        case SENSOR_TYPE_PICKUP:
                return sizeof(struct pickup_data);
        case SENSOR_TYPE_HINGE:
                return sizeof(struct hinge_data);
        case SENSOR_TYPE_TEMPERATURE:
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        case SENSOR_TYPE_AUDIO_CLASSIFICATION:
                log_message(CRITICAL,"%s: Unsupported Sensor Type: %d\n", __FUNCTION__, sensorType);
                break;
        default:
                log_message(DEBUG,"%s: Unknow Sensor, suppose it's oem sensor : %d\n", __FUNCTION__, sensorType);
                return sizeof(struct oem_data);
        }
        return -1;
}

ssize_t SensorHubHelper::readSensorhubEvents(int fd, struct sensorhub_event_t* events, size_t count, int sensorType)
{
        if (fd < 0)
                return fd;

        if (count <= 0)
                return 0;

        size_t unitSize = getUnitSize(sensorType);
        size_t streamSize = unitSize * count;
        byte* stream = new byte[streamSize];
        streamSize = read(fd, reinterpret_cast<void *>(stream), streamSize);
        if (streamSize % unitSize != 0) {
                log_message(CRITICAL,"%s line: %d: invalid stream size: type: %d size: %d unit_size: %d\n",
                     __FUNCTION__, __LINE__, sensorType, streamSize, unitSize);
                delete[] stream;
                return -1;
        }

        count = streamSize / unitSize;

        switch (sensorType) {
        case SENSOR_TYPE_ACCELEROMETER:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct accel_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct accel_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct accel_data*>(stream))[i].z;
                        events[i].accuracy = SENSOR_STATUS_ACCURACY_HIGH;
                        events[i].timestamp = (reinterpret_cast<struct accel_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_ACC_SHAKE:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct accel_shake_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct accel_shake_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct accel_shake_data*>(stream))[i].z;
                        events[i].data[3] = (reinterpret_cast<struct accel_shake_data*>(stream))[i].motion;
                        events[i].timestamp = (reinterpret_cast<struct accel_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct compass_raw_data *>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct compass_raw_data *>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct compass_raw_data *>(stream))[i].z;
                        events[i].accuracy = (reinterpret_cast<struct compass_raw_data *>(stream))[i].mag_accuracy;
                        events[i].timestamp = (reinterpret_cast<struct compass_raw_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_ORIENTATION:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = 36000000 - ((reinterpret_cast<struct orientation_data*>(stream))[i].tiltz);
                        events[i].data[1] = - (reinterpret_cast<struct orientation_data*>(stream))[i].tiltx;
                        events[i].data[2] = (reinterpret_cast<struct orientation_data*>(stream))[i].tilty;
                        events[i].accuracy = (reinterpret_cast<struct orientation_data*>(stream))[i].mag_accuracy;
                        events[i].timestamp = (reinterpret_cast<struct orientation_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GYROSCOPE:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct gyro_raw_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct gyro_raw_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct gyro_raw_data*>(stream))[i].z;
                        events[i].accuracy = SENSOR_STATUS_ACCURACY_HIGH;
                        events[i].timestamp = (reinterpret_cast<struct gyro_raw_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_LIGHT:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct als_raw_data *>(stream))[i].lux;
                        events[i].timestamp = (reinterpret_cast<struct als_raw_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_PRESSURE:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct baro_raw_data*>(stream))[i].p;
                        events[i].timestamp = (reinterpret_cast<struct baro_raw_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_PROXIMITY:
        case SENSOR_TYPE_SAR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct ps_phy_data*>(stream))[i].near == 0 ? 1 : 0;
                        events[i].timestamp = (reinterpret_cast<struct ps_phy_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GRAVITY:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct gravity_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct gravity_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct gravity_data*>(stream))[i].z;
                        events[i].accuracy = SENSOR_STATUS_ACCURACY_MEDIUM;
                        events[i].timestamp = (reinterpret_cast<struct gravity_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_LINEAR_ACCELERATION:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct linear_accel_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct linear_accel_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct linear_accel_data*>(stream))[i].z;
                        events[i].accuracy = SENSOR_STATUS_ACCURACY_MEDIUM;
                        events[i].timestamp = (reinterpret_cast<struct linear_accel_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_ROTATION_VECTOR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].z;
                        events[i].data[3] = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].w;
                        events[i].data[4] = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].mag_accuracy;
                        events[i].timestamp = (reinterpret_cast<struct rotation_vector_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GESTURE_FLICK:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = getGestureFlickEvent((reinterpret_cast<struct gesture_flick_data*>(stream))[i]);
                        events[i].timestamp = (reinterpret_cast<struct gesture_flick_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_PICKUP:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct pickup_data*>(stream))[i].event;
                        events[i].timestamp = (reinterpret_cast<struct gesture_flick_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_TERMINAL:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = getTerminalEvent((reinterpret_cast<struct tc_data*>(stream))[i]);
                        events[i].timestamp = (reinterpret_cast<struct tc_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_SHAKE:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = getShakeEvent((reinterpret_cast<struct shaking_data*>(stream))[i]);
                        events[i].timestamp = (reinterpret_cast<struct shaking_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_SIMPLE_TAPPING:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = getSimpleTappingEvent((reinterpret_cast<struct stap_data*>(stream))[i]);
                        events[i].timestamp = (reinterpret_cast<struct stap_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GLANCE_GESTURE:
                {
                        int j = 0;
                        for (unsigned int i = 0; i < count; i++) {
                                int status = getSimpleTappingEvent((reinterpret_cast<struct stap_data*>(stream))[i]);

                                if (status) {
                                        events[j].timestamp = 1;
                                        events[j].timestamp = (reinterpret_cast<struct stap_data*>(stream))[i].ts;
                                        j++;
                                }
                        }
                        count = j;
                }
                break;
        case SENSOR_TYPE_MOVE_DETECT:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = getMoveDetectEvent((reinterpret_cast<struct md_data*>(stream))[i]);
                        events[i].timestamp = (reinterpret_cast<struct md_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_STEP_DETECTOR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct stepdetector_data*>(stream))[i]).step_event_counter;
                        events[i].timestamp = (reinterpret_cast<struct stepdetector_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_STEP_COUNTER:
                for (unsigned int i = 0; i < count; i++) {
                        int run_counter = ((reinterpret_cast<struct stepcounter_data*>(stream))[i]).run_step_count;
                        int walk_counter = ((reinterpret_cast<struct stepcounter_data*>(stream))[i]).walk_step_count;
                        events[i].step_counter = run_counter + walk_counter;
                        events[i].timestamp = (reinterpret_cast<struct stepcounter_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct sm_data*>(stream))[i]).state;
                        events[i].timestamp = (reinterpret_cast<struct sm_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_LIFT:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct lift_data*>(stream))[i]).look;
                        events[i].data[1] = ((reinterpret_cast<struct lift_data*>(stream))[i]).vertical;
                        events[i].timestamp = (reinterpret_cast<struct lift_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_WAKE_GESTURE:
                {
                        int j = 0;
                        for (unsigned int i = 0; i < count; i++) {
                                int look = ((reinterpret_cast<struct lift_data*>(stream))[i]).look;
                                int vertical = ((reinterpret_cast<struct lift_data*>(stream))[i]).vertical;

                                if (look || vertical) {
                                        events[j].data[0] = 1;
                                        events[j].timestamp = (reinterpret_cast<struct lift_data*>(stream))[i].ts;
                                        j++;
                                }
                        }
                        count = j;
                }
                break;
        case SENSOR_TYPE_PAN_ZOOM:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct pz_data*>(stream))[i]).deltX;
                        events[i].data[1] = ((reinterpret_cast<struct pz_data*>(stream))[i]).deltY;
                        events[i].timestamp = (reinterpret_cast<struct pz_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_INSTANT_ACTIVITY:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct instant_activity_data*>(stream))[i]).typeclass;
                        events[i].timestamp = (reinterpret_cast<struct instant_activity_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_TILT_DETECTOR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct tilt_data*>(stream))[i]).tilt_event;
                        events[i].timestamp = (reinterpret_cast<struct tilt_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct game_rotation_vector_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct game_rotation_vector_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct game_rotation_vector_data*>(stream))[i].z;
                        events[i].data[3] = (reinterpret_cast<struct game_rotation_vector_data*>(stream))[i].w;
                        events[i].timestamp = (reinterpret_cast<struct game_rotation_vector_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct geomagnetic_rotation_vector_data*>(stream))[i].x;
                        events[i].data[1] = (reinterpret_cast<struct geomagnetic_rotation_vector_data*>(stream))[i].y;
                        events[i].data[2] = (reinterpret_cast<struct geomagnetic_rotation_vector_data*>(stream))[i].z;
                        events[i].data[3] = (reinterpret_cast<struct geomagnetic_rotation_vector_data*>(stream))[i].w;
                        events[i].timestamp = (reinterpret_cast<struct geomagnetic_rotation_vector_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].x_uncalib;
                        events[i].data[1] = (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].y_uncalib;
                        events[i].data[2] = (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].z_uncalib;
                        events[i].data[3] = events[i].data[0] - (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].x_calib;
                        events[i].data[4] = events[i].data[1] - (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].y_calib;
                        events[i].data[5] = events[i].data[2] - (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].z_calib;
                        events[i].timestamp = (reinterpret_cast<struct uncalib_compass_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].x_uncalib;
                        events[i].data[1] = (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].y_uncalib;
                        events[i].data[2] = (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].z_uncalib;
                        events[i].data[3] = events[i].data[0] - (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].x_calib;
                        events[i].data[4] = events[i].data[1] - (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].y_calib;
                        events[i].data[5] = events[i].data[2] - (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].z_calib;
                        events[i].timestamp = (reinterpret_cast<struct uncalib_gyro_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct uncalib_acc_data*>(stream))[i].x_uncalib;
                        events[i].data[1] = (reinterpret_cast<struct uncalib_acc_data*>(stream))[i].y_uncalib;
                        events[i].data[2] = (reinterpret_cast<struct uncalib_acc_data*>(stream))[i].z_uncalib;
                        events[i].timestamp = (reinterpret_cast<struct uncalib_acc_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_MOTION_DETECT:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = (reinterpret_cast<struct motion_detect_data*>(stream))[i].eventData1;
                        events[i].data[1] = (reinterpret_cast<struct motion_detect_data*>(stream))[i].eventData2;
                        events[i].data[2] = (reinterpret_cast<struct motion_detect_data*>(stream))[i].eventData3;
                        events[i].data[3] = (reinterpret_cast<struct motion_detect_data*>(stream))[i].eventData4;
                        events[i].data[4] = (reinterpret_cast<struct motion_detect_data*>(stream))[i].eventData5;
                        events[i].timestamp = (reinterpret_cast<struct motion_detect_data*>(stream))[i].ts;
                }
                break;
        case SENSOR_TYPE_HINGE:
                for (unsigned int i = 0; i < count; i++) {
                        events[i].data[0] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).hinge_angle;
                        events[i].data[1] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).screen_angle;
                        events[i].data[2] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).keyboard_angle;
                        events[i].data[3] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).accel_data[0];
                        events[i].data[4] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).accel_data[1];
                        events[i].data[5] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).accel_data[2];
                        events[i].data[6] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).second_accel_data[0];
                        events[i].data[7] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).second_accel_data[1];
                        events[i].data[8] = ((reinterpret_cast<struct hinge_data*>(stream))[i]).second_accel_data[2];
                        events[i].timestamp = (reinterpret_cast<struct hinge_data*>(stream))[i].ts;
                }
                break;

        case SENSOR_TYPE_TEMPERATURE:
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        case SENSOR_TYPE_AUDIO_CLASSIFICATION:
        case SENSOR_TYPE_GESTURE:
        case SENSOR_TYPE_PEDOMETER:
                log_message(CRITICAL,"%s: Unsupported Sensor Type: %d\n", __FUNCTION__, sensorType);
                break;
        default:
                log_message(DEBUG, "%s: Unkown sensor, suppose it's oem sensor: %d\n", __FUNCTION__, sensorType);
                for (unsigned int i = 0; i < count; i++) {
                       events[i].timestamp = (reinterpret_cast<struct oem_data*>(stream))[i].ts;

                       for (unsigned int j = 0; j < 16; j++)
                               events[i].data[j] = (reinterpret_cast<struct oem_data*>(stream))[i].data[j];
                }
                break;
        }

        delete[] stream;

        return count;
}
