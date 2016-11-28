/*
 ============================================================================
 Name        : fusion.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Algo/AlgorithmManager.h"

#define ER_SUCCESS                     0
#define ER_FAILED                     -1
#define ER_IO_ERROR					  -2
#define ER_IO_END					  -3

typedef struct {
	int64_t delta_t;
	int64_t integ_valx;
	int64_t integ_valy;
	int64_t integ_valz;
	int64_t integ_st_tm;
} integral_t;

typedef struct {
	int32_t x; /**< accel x data 10,14 and 12 resolution*/
	int32_t y; /**< accel y data 10,14 and 12 resolution*/
	int32_t z; /**< accel z data 10,14 and 12 resolution*/
}xyz_32_t;

static integral_t gyro_itgr;
static xyz_32_t accel, gyro;
int64_t current_ts = 0;
FILE *fp = NULL;
char* line = NULL;

static void HandleAlgoNotifation(ULONG SensorType, void* data)
{
	return;
}

int read_file()
{
	char *pch = NULL;
	size_t len = 0;
	ssize_t read;
	float value[7];
	int index = 0;

	int scaler = 1;
	double pi_scaler = 180.0/3.1415;

	if((read = getline(&line, &len, fp)) != -1) {

		//printf("Retrieved %s", line);
		pch = strtok (line,",\r\n");
		while(pch != NULL) 	{
			value[index] = atof(pch);
			pch = strtok (NULL, ",\r\n");
			index++;
		}

		accel.x = (int32_t) scaler*value[0];
		accel.y = (int32_t) scaler*value[1];
		accel.z = (int32_t) scaler*value[2];
		gyro.x = (int32_t) scaler*pi_scaler*value[3];
		gyro.y = (int32_t) scaler*pi_scaler*value[4];
		gyro.z = (int32_t) scaler*pi_scaler*value[5];

		gyro_itgr.delta_t = (int64_t)value[6] - current_ts;
		current_ts = (int64_t)value[6]; //ms
	} else {
		return ER_IO_END;
	}
	return ER_SUCCESS;
}

int fusion(pIvhAlgo Algo_Handle)
{
	int accel_fusion[3], gyro_fusion[3], euler_fusion[3];
	int rot[9];

	//m m/s2; current_ts: ms
	Alg_UpdateAccelerometerData(Algo_Handle,
			accel.x,accel.y,accel.z,current_ts);
	Alg_GetAccelerometer(Algo_Handle,accel_fusion);

	//mdps
	Alg_UpdateGyrometerData(Algo_Handle,
			gyro.x,gyro.y,gyro.z,current_ts);
	Alg_GetGyrometer(Algo_Handle,gyro_fusion);

	Alg_GetEuler(Algo_Handle,euler_fusion);
	Alg_GetOrientationMatrix(Algo_Handle, rot);


	if (!gyro_itgr.integ_st_tm)
		gyro_itgr.integ_st_tm = current_ts; //ms

	//mdps * ms
	gyro_itgr.integ_valx += gyro_fusion[0] * gyro_itgr.delta_t;
	gyro_itgr.integ_valy += gyro_fusion[1] * gyro_itgr.delta_t;
	gyro_itgr.integ_valz += gyro_fusion[2] * gyro_itgr.delta_t;

#if 0
	printf("%9d\t%9d\t%9d\t", accel_fusion[0], accel_fusion[1], accel_fusion[2]);//ACC
	printf("%9d\t%9d\t%9d\t", gyro_fusion[0], gyro_fusion[1], gyro_fusion[2]);//GYRO
#endif
	printf("%9.9f\t%9.9f\t%9.9f\t", (float)gyro_itgr.integ_valx/1000000, (float)gyro_itgr.integ_valy/1000000, (float)gyro_itgr.integ_valz/1000000);//G-ITGR
	printf("%f\t%f\t%f\t", euler_fusion[0]/100.0, euler_fusion[1]/100.0, euler_fusion[2]/100.0);//EULER
	printf("%9lld\n", current_ts);//time

	return ER_SUCCESS;
}

int main(int argc, char* argv[]) {

	int ret_code = ER_SUCCESS;
	unsigned long sensor_input = IVH_PHYSICAL_SENSOR_ACCELEROMETER3D|IVH_PHYSICAL_SENSOR_GYROSCOPE3D|IVH_PHYSICAL_SENSOR_MAGNETOMETER3D;
	unsigned long sensor_output = 0;


	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	pIvhAlgo Algo_Handle = NULL;
	Algo_Handle = Alg_Init(HandleAlgoNotifation, sensor_input, &sensor_output);

	fp = fopen(argv[1], "r");
	if(fp == NULL)
		return ER_IO_ERROR;

	if(ER_SUCCESS != ret_code)
		printf("ERR while read file %d\n", ret_code);

	while(read_file() != ER_IO_END)
	{
		fusion(Algo_Handle);
	}

	free(line);
	return ER_SUCCESS;
}
