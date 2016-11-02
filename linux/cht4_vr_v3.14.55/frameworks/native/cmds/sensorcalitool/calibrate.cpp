#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //memset()
#include <cstring>

#include "calibrate.h"
#include "sensorthread.h"

#define AGM_SENSOR_FILE_NAME	"/storage/emulated/0/sensor_calibration_AGM.bin"
#define TXT_BUF_SIZE 1024


void SwapData(SENSOR_DATA_T* a, SENSOR_DATA_T* b);
int Filter(SENSOR_DATA_T* data, int size);

int Calibrated = 0;

int enable_finish = 0;
int apk_exit = 0;
int point = 0;

extern SENSOR_DATA_T data_loop_acc;
extern SENSOR_DATA_T data_loop_gyr;
extern SENSOR_DATA_T data_loop_mag;

static const int SKIP_DATA = 10;
static const int DATA_SIZE = 11;
SENSOR_DATA_T data;
SENSOR_DATA_T temp[DATA_SIZE*3];

SENSOR_CALIBRATION file_content ={
	{
		/*.calibrated*/1, /*0: not calibrated; 1: per-system calibrate; 2: per-model calibrate*/
		/*.spen = */0,
		/*.shake_th = */1800,
		/*.shake_shock = */50,
		/*.shake_quiet = */300,
		/*.accl_tolarence = */20,
		/*.gyro_tolarence = */400,
		/*.magn_tolarence = */50,
		/*.stableMS = */2000
	},
	{
		/*int magnx,*/(int)(-5),
		/*int magny,*/305UL,
		/*int magnz,*/(int)(-118),
		/*int magnxnx;*/(int)(-15),
		/*int magnxny;*/(int)(-15),
		/*int magnxnz;*/(int)(-15),
		/*int magnyux;*/(int)(-15),
		/*int magnyuy;*/(int)(-15),
		/*int magnyuz;*/(int)(-15),
		/*int magnxsx;*/(int)(-15),
		/*int magnxsy;*/(int)(-15),
		/*int magnxsz;*/(int)(-15),
		/*int magnydx;*/(int)(-15),
		/*int magnydy;*/(int)(-15),
		/*int magnydz;*/(int)(-15),
		/*int magnnx,*/(int)(-15),
		/*int magnny,*/268UL,
		/*int magnnz,*/(int)(-225),
		/*int magnsx,*/(int)(-13),
		/*int magnsy,*/(int)(-233),
		/*int magnsz,*/241UL,
		/*int acclx,*/8UL,
		/*int accly,*/(int)(-17),
		/*int acclz,*/(int)(-30),
		/*int acclzx;//horizontal*/(int)(-15),
		/*int acclzy;*/(int)(-15),
		/*int acclzz;*/(int)(-15),
		/*int acclyx;//vertical*/(int)(-15),
		/*int acclyy;*/(int)(-15),
		/*int acclyz;*/(int)(-15),
		/*int gyrox,*/0UL,
		/*int gyroy,*/(int)(234),
		/*int gyroz,*/(int)(-242),
		/*int gyrozx;*/(int)(0),
		/*int gyrozy;*/(int)(-0),
		/*int gyrozz;*/(int)(-45),
		/*int gyroyx;*/(int)(-0),
		/*int gyroyy;*/(int)(-45),
		/*int gyroyz;*/0UL,
		/*int als_curve[20]*/12UL,0UL,24UL,0UL,35UL,1UL,47UL,4UL,59UL,10UL,71UL,28UL,82UL,75UL,94UL,206UL,106UL,565UL,118UL,1547UL,
		/*int als_multiplier*/125UL

	}
};

#ifdef DEBUG_USE_ADB
void child_handler(int signo)
{

	if(signo == SIGINT) {
		ALOGD("child_handler\n");
		apk_exit = 1;
	}

}
#endif

int WriteDataToFile(const char *pathname, /*char*/void *buf, size_t count)
{
	int fd;

	if((fd = open(pathname,O_RDWR | O_CREAT | O_APPEND,0664)) == -1){
		perror("open");
		return -1;
	}
	ALOGD("open .bin file.\n");

	write(fd,buf,count);

	close(fd);

	return 0;
}


int WriteDataToFileInTxt()
{
	char buf[TXT_BUF_SIZE];
	int len = 0;
	memset(buf,0xff,sizeof(buf));

	len = snprintf(buf, sizeof(buf),\
			//mag
			"magnx=%d, magny=%d, magnz=%d,\n"  \
			"magnxnx=%d, magnxny=%d, magnxnz=%d,\n" \
			"magnyux=%d, magnyuy=%d, magnyuz=%d,\n" \
			"magnxsx=%d, magnxsy=%d, magnxsz=%d,\n" \
			"magnydx=%d, magnydy=%d, magnydz=%d,\n" \
			"magnnx =%d, magnny =%d, magnnz =%d,\n" \
			"magnsx =%d, magnsy =%d, magnsz =%d,\n" \

			//acc
			"acclx =%d, accly =%d, acclz =%d,\n" \
			"acclzx=%d, acclzy=%d, acclzz=%d,\n" \
			"acclyx=%d, acclyy=%d, acclyz=%d,\n" \

			//gyr
			"gyrox =%d, gyroy =%d, gyroz =%d,\n" \
			"gyrozx=%d, gyrozy=%d, gyrozz=%d,\n" \
			"gyroyx=%d, gyroyy=%d, gyroyz=%d,\n" \

			//alscurve[20]
			"alscurve[0~4]: %d, %d, %d, %d, %d,\n" \
			"alscurve[5~9]: %d, %d, %d, %d, %d,\n" \
			"alscurve[10~14]: %d, %d, %d, %d, %d,\n" \
			"alscurve[15~19]: %d, %d, %d, %d, %d,\n" \
			"als_multiplier=%d\n\n>>end<<\n\n", \
			//mag
			file_content.calibration.magnx, file_content.calibration.magny, file_content.calibration.magnz, \
			file_content.calibration.magnxnx, file_content.calibration.magnxny, file_content.calibration.magnxnz, \
			file_content.calibration.magnyux, file_content.calibration.magnyuy, file_content.calibration.magnyuz, \
			file_content.calibration.magnxsx, file_content.calibration.magnxsy, file_content.calibration.magnxsz, \

			file_content.calibration.magnydx, file_content.calibration.magnydy, file_content.calibration.magnydz, \
			file_content.calibration.magnnx , file_content.calibration.magnny , file_content.calibration.magnnz , \
			file_content.calibration.magnsx , file_content.calibration.magnsy , file_content.calibration.magnsz , \

			//acc
			file_content.calibration.acclx , file_content.calibration.accly , file_content.calibration.acclz , \
			file_content.calibration.acclzx, file_content.calibration.acclzy, file_content.calibration.acclzz, \
			file_content.calibration.acclyx, file_content.calibration.acclyy, file_content.calibration.acclyz, \

			//gyr
			file_content.calibration.gyrox , file_content.calibration.gyroy , file_content.calibration.gyroz , \
			file_content.calibration.gyrozx, file_content.calibration.gyrozy, file_content.calibration.gyrozz, \
			file_content.calibration.gyroyx, file_content.calibration.gyroyy, file_content.calibration.gyroyz, \

			file_content.calibration.alscurve[0] , file_content.calibration.alscurve[1] , file_content.calibration.alscurve[2] ,\
			file_content.calibration.alscurve[3] , file_content.calibration.alscurve[4] ,\
			file_content.calibration.alscurve[5] , file_content.calibration.alscurve[6] , file_content.calibration.alscurve[7] ,\
			file_content.calibration.alscurve[8] , file_content.calibration.alscurve[9] ,\
			file_content.calibration.alscurve[10] , file_content.calibration.alscurve[11] , file_content.calibration.alscurve[12] ,\
			file_content.calibration.alscurve[13] , file_content.calibration.alscurve[14] ,\
			file_content.calibration.alscurve[15] , file_content.calibration.alscurve[16] , file_content.calibration.alscurve[17] ,\
			file_content.calibration.alscurve[18] , file_content.calibration.alscurve[19] ,\
			file_content.calibration.als_multiplier);

	printf("len = %d\n", len);
	if (0 > len) {
		printf("an output error for setAGM_cal.txt, len = %d\n.", len);
		return -1;
	}

	FILE* pFile = fopen("/storage/emulated/0/setAGM_cal.txt", "w+");
	if (!pFile)
		return -1;

	ALOGD("open .txt file.\n");
	fwrite(buf, 1, sizeof(buf), pFile);
	fclose(pFile);

	return 0;
}

void SwapData(SENSOR_DATA_T* a, SENSOR_DATA_T* b)
{
	SENSOR_DATA_T t;
	t.data.data.x = a->data.data.x;
	t.data.data.y = a->data.data.y;
	t.data.data.z = a->data.data.z;
	t.data.data.n = a->data.data.n;
	a->data.data.x = b->data.data.x;
	a->data.data.y = b->data.data.y;
	a->data.data.z = b->data.data.z;
	a->data.data.n = b->data.data.n;
	b->data.data.x = t.data.data.x;
	b->data.data.y = t.data.data.y;
	b->data.data.z = t.data.data.z;
	b->data.data.n = t.data.data.n;
}

int Filter(SENSOR_DATA_T* data, int size)
{
	//SENSOR_DATA_T a,g,m,sum;
	//unsigned long norm = 0;

	int i,j;
	//sort data
	for(i = 0; i < size*3; i++)	{
		data[i].data.data.n = data[i].data.data.x*data[i].data.data.x\
							  + data[i].data.data.y*data[i].data.data.y\
							  + data[i].data.data.z*data[i].data.data.z;
	}
	for(i=0; i< size; i++) {
		for(j = 0; j < i; j++) {
			if(data[i*3].data.data.n < data[j*3].data.data.n) { //swap
				SwapData(&data[i*3], &data[j*3]);
			}
			if(data[i*3+1].data.data.n < data[j*3+1].data.data.n) { //swap
				SwapData(&data[i*3+1], &data[j*3+1]);
			}
			if(data[i*3+2].data.data.n < data[j*3+2].data.data.n) { //swap
				SwapData(&data[i*3+2], &data[j*3+2]);
			}
		}
	}

	data[0].data.data.x = 0;
	data[0].data.data.y = 0;
	data[0].data.data.z = 0;
	data[1].data.data.x = 0;
	data[1].data.data.y = 0;
	data[1].data.data.z = 0;
	data[2].data.data.x = 0;
	data[2].data.data.y = 0;
	data[2].data.data.z = 0;
	for(i = 2; i< size-2; i++) {
		data[0].data.data.x += data[i*3].data.data.x;
		data[0].data.data.y += data[i*3].data.data.y;
		data[0].data.data.z += data[i*3].data.data.z;
		data[1].data.data.x += data[i*3+1].data.data.x;
		data[1].data.data.y += data[i*3+1].data.data.y;
		data[1].data.data.z += data[i*3+1].data.data.z;
		data[2].data.data.x += data[i*3+2].data.data.x;
		data[2].data.data.y += data[i*3+2].data.data.y;
		data[2].data.data.z += data[i*3+2].data.data.z;
	}
	data[0].data.data.x /=(size-4);
	data[0].data.data.y /=(size-4);
	data[0].data.data.z /=(size-4);
	data[1].data.data.x /=(size-4);
	data[1].data.data.y /=(size-4);
	data[1].data.data.z /=(size-4);
	data[2].data.data.x /=(size-4);
	data[2].data.data.y /=(size-4);
	data[2].data.data.z /=(size-4);
	return 0;
}


void *sensorAGM_read_data_loop(void *arg)
{

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	Sensor const* const* list;
	int count;
	int incalibrate = 0;

	SensorManager &mgr(SensorManager::getInstance());

	sp<SensorEventQueue> queue = mgr.createEventQueue();
	if (queue == NULL) {
		ALOGD("createEventQueue returned NULL\n");
		return 0;
	}

	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	if (queue_gyro == NULL) {
		ALOGD("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} 

	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();
	if (queue_mag == NULL) {
		ALOGD("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}

	count = mgr.getSensorList(&list);
	if(list == NULL)
		return 0;



	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACC_RAW, incalibrate);
	if (sensor_thread == NULL){
		ALOGD("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYRO_RAW, incalibrate);
	if (sensor_thread_gyro == NULL) {
		ALOGD("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_COMPS_RAW, incalibrate);
	if (sensor_thread_mag == NULL) {
		ALOGD("failed to create mag sensor thread\n");
		return 0;

	}

	ALOGD("%s:before run\n", __func__);

	for (int i=0 ; i<count ; i++) {
		if(list[i] == NULL)
			break;
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACC_RAW:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYRO_RAW:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_COMPS_RAW:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		ALOGD("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		ALOGD("get sensor of type:Gyr error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		ALOGD("get sensor of type:Mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		ALOGD("enable sensor of type Acc  error\n");
		return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		ALOGD("enable sensor of type gyro error\n");
		return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		ALOGD("enable sensor of type magcal error\n");
		return 0;
	}

	ALOGD("%s:after acc gyro magcal enable\n", __func__);

	sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
	sensor_thread_gyro->run("sensor-loop", PRIORITY_BACKGROUND);
	sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);

	enable_finish = 1;
	sensor_thread->join();
	sensor_thread_gyro->join();
	sensor_thread_mag->join();

	while(1) {
		ALOGD("after join(),go to while(1)\n");
		usleep(100000);

		if(1 == apk_exit) {
			ALOGD("when apk exit, sensorAGM_read_data_loop break\n");
			break;
		}
	}

	return NULL;
}

int readAccSensor(SENSOR_DATA_T *data)
{
	int i = 0;

	do {

		if(++i > 20)//for test
			break;

		data->data.accelMilliG.accelX = data_loop_acc.data.accelMilliG.accelX;
		data->data.accelMilliG.accelY = data_loop_acc.data.accelMilliG.accelY;
		data->data.accelMilliG.accelZ = data_loop_acc.data.accelMilliG.accelZ;

		usleep(5000);//1s / 300 -> 3 ms
	} while(0 == data->data.accelMilliG.accelX && 0 == data->data.accelMilliG.accelY && 0 == data->data.accelMilliG.accelZ);

	return 0;
}

int readGyrSensor(SENSOR_DATA_T *data)
{
	int i = 0;

	do {
		if(++i > 20)// for test
			break;

		data->data.gyroMilliDegreesPerSecond.gyroX = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX;
		data->data.gyroMilliDegreesPerSecond.gyroY = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY;
		data->data.gyroMilliDegreesPerSecond.gyroZ = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ;
		usleep(5000);
	} while( 0 == data->data.gyroMilliDegreesPerSecond.gyroX && 0 == data->data.gyroMilliDegreesPerSecond.gyroY && 0 == data->data.gyroMilliDegreesPerSecond.gyroZ );

	return 0;
}


int readMagSensor(SENSOR_DATA_T *data)
{
	int i = 0;

	do {
		if(++i > 20)//for test
			break;

		data->data.magFieldMilliGauss.magFieldX = data_loop_mag.data.magFieldMilliGauss.magFieldX;
		data->data.magFieldMilliGauss.magFieldY = data_loop_mag.data.magFieldMilliGauss.magFieldY;
		data->data.magFieldMilliGauss.magFieldZ = data_loop_mag.data.magFieldMilliGauss.magFieldZ;

		usleep(5000);
	} while(0 == data->data.magFieldMilliGauss.magFieldX && 0 == data->data.magFieldMilliGauss.magFieldY && 0 == data->data.magFieldMilliGauss.magFieldZ);

	return 0;
}

int GetProcessPoint()
{
	return point;
}

int ApkExit()
{
	apk_exit = 1;
	return 0;
}


void printAccData(int step, SENSOR_DATA_T *data, int tempFlag, int index)
{
	int PARAM_TEMP = 1;

	ALOGD("[%d/5]", step);

	if(tempFlag == PARAM_TEMP)
		ALOGD("temp[%2d]", index);
	else
		ALOGD("data");

	ALOGD(": <accelX   =%7d,\taccelY   =%7d,\taccelZ   =%7d>\n",\
			data->data.accelMilliG.accelX,\
			data->data.accelMilliG.accelY,\
			data->data.accelMilliG.accelZ);
}

void printGyroData(int step, SENSOR_DATA_T *data, int tempFlag, int index)
{
	int PARAM_TEMP = 1;

	ALOGD("[%d/5]", step);

	if(tempFlag == PARAM_TEMP)
		ALOGD("temp[%2d]", index);
	else
		ALOGD("data");

	ALOGD(": <gyroX    =%7d,\tgyroY    =%7d,\tgyroZ    =%7d>\n",\
			data->data.gyroMilliDegreesPerSecond.gyroX,\
			data->data.gyroMilliDegreesPerSecond.gyroY,\
			data->data.gyroMilliDegreesPerSecond.gyroZ);
}

void printMagnData(int step, SENSOR_DATA_T *data, int tempFlag, int index)
{
	int PARAM_TEMP = 1;

	ALOGD("[%d/5]", step);

	if(tempFlag == PARAM_TEMP)
		ALOGD("temp[%2d]", index);
	else
		ALOGD("data");

	ALOGD(": <magFieldX=%7d,\tmagFieldX=%7d,\tmagFieldX=%7d>\n",\
			data->data.magFieldMilliGauss.magFieldX,\
			data->data.magFieldMilliGauss.magFieldY,
			data->data.magFieldMilliGauss.magFieldZ);
}

void skipData(int count)
{
	int i;

	for(i =0; i < count; i++) {// skip 10 data...
		readAccSensor(&data);
		printAccData(1,&data,0,0);

		readGyrSensor(&data);
		printGyroData(1,&data,0,0);

		readMagSensor(&data);
		printMagnData(1,&data,0,0);

		usleep(200000);//note: magn update time is 50ms
	}
}
 
int SetAGM_STEP_A()
{
	apk_exit = 0;//avoid to run app again failed
	int i;
	int ret;
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;
	pthread_t tid;

#ifdef DEBUG_USE_ADB
	signal(SIGINT,child_handler);
#endif

	ret = pthread_create(&tid,NULL,sensorAGM_read_data_loop,(void*)0);
	if(ret) {
		ALOGD("create pthread error!ret = %d\n",ret);
		return -1;
	}

	printf("enable_finish= %d\n",enable_finish);
	while(!enable_finish)
		usleep(50000);


	//==============================================
	//step 1:

	ALOGD("[1/5]Lay the device on a flat surface, north, east, screen up \n");
	//ALOGD("[1/5]Lay the device on a flat surface with the Windows button pointing due South\n");

	//read the data...
	skipData(SKIP_DATA);

	for(i = 0; i < DATA_SIZE; i++) {//now read the data...
		ret_acc = readAccSensor(&temp[i*3]);
		printAccData(1,&temp[i*3],1,i*3);

		ret_gyr = readGyrSensor(&temp[i*3+1]);
		printGyroData(1,&temp[i*3+1],1,i*3+1);

		ret_mag = readMagSensor(&temp[i*3+2]);
		printMagnData(1,&temp[i*3+2],1,i*3+2);

		usleep(200000);//note: magn update time is 50ms
	}


	//filt data
	Filter(temp, DATA_SIZE);
	if ( 0 == ret_acc) {
		file_content.calibration.acclzx = temp[0].data.accelMilliG.accelX;
		file_content.calibration.acclzy = temp[0].data.accelMilliG.accelY;
		file_content.calibration.acclzz = temp[0].data.accelMilliG.accelZ;
		printAccData(1,&temp[0],1,0);
	}
	if (0 == ret_gyr) {
		file_content.calibration.gyrox = temp[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyroy = temp[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyroz = temp[1].data.gyroMilliDegreesPerSecond.gyroZ;
		printGyroData(1,&temp[1],1,1);
	}
	if (0 == ret_mag) {
		file_content.calibration.magnxnx = temp[2].data.magFieldMilliGauss.magFieldX;
		file_content.calibration.magnxny = temp[2].data.magFieldMilliGauss.magFieldY;
		file_content.calibration.magnxnz = temp[2].data.magFieldMilliGauss.magFieldZ;
		printMagnData(1,&temp[2],1,2);
	}


	ALOGD("The first step is completed\n");

	return 0;

}

int SetAGM_STEP_B()
{
	int i;
	int ret_mag = 0;
	//==============================================
	//step 2:

	ALOGD("[2/5]Rotate the screen 180 degrees clockwise, South, west, screen up \n");
	//ALOGD("[2/5]Rotate the screen 180 degrees clockwise so the Windows button is pointing due North\n");

	usleep(2000000);
	//read the data...
	for(i = 0; i < DATA_SIZE; i++) {
		ret_mag = readMagSensor(&temp[i*3+2]);
		printMagnData(2,&temp[i*3+2],1,i*3+2);
		usleep(50000); //note: magn update time is 50ms
	}

	ALOGD("\n");
	//filt data
	Filter(temp, DATA_SIZE);
	if (0 == ret_mag) {
		file_content.calibration.magnxsx = temp[2].data.magFieldMilliGauss.magFieldX;
		file_content.calibration.magnxsy = temp[2].data.magFieldMilliGauss.magFieldY;
	}

	ALOGD("The 2nd step is completed\n");
	return 0;
}

int SetAGM_STEP_C()
{
	int i;
	int ret_mag = 0;


	//==============================================
	//step 3:
	ALOGD("[3/5]Lay the device flat, north, east, screen down\n");
	//ALOGD("[3/5]Now Lay the device flat, screen down with the windows button pointing due South\n");
	usleep(2000000);

	//read the data...
	for(i = 0; i < DATA_SIZE; i++) {
		ret_mag = readMagSensor(&temp[i*3+2]);
		printMagnData(3,&temp[i*3+2],1,i*3+2);

		usleep(200000);//note: magn update time is 50ms
	}
	Filter(temp, DATA_SIZE);
	if (0 == ret_mag) {
		file_content.calibration.magnxsz = temp[2].data.magFieldMilliGauss.magFieldZ;
	}


	ALOGD("The 3rd step is completed\n");

	return 0;
}


int SetAGM_STEP_D()
{
	int i;
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;
	point = 0;


	//==============================================
	//step 4:
	ALOGD("[4/5]Lay the device flat with the screen up, rotate the device counter clockwise\n");
	usleep(2000000);

	//read the data...
	static const int SAMPLES = 20;
	SENSOR_DATA_T* buf = new SENSOR_DATA_T[SAMPLES*3];

	for(i = 0; i< SAMPLES;)	{
		usleep(200000); //gyro update is 20ms
		ret_gyr = readGyrSensor(&data);
		printGyroData(4,&data,0,0);

		if (0 == ret_gyr) {//retg
			if(data.data.gyroMilliDegreesPerSecond.gyroX > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroX < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ < -45000)	{
				ALOGD("[if=>]\n");
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroX = data.data.gyroMilliDegreesPerSecond.gyroX;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroY = data.data.gyroMilliDegreesPerSecond.gyroY;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroZ = data.data.gyroMilliDegreesPerSecond.gyroZ;


				ret_acc = readAccSensor(&data);
				printAccData(4,&data,0,0);
				if (0 == ret_acc) {
					buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
					buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
					buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
				}

				ret_mag = readMagSensor(&data);
				printMagnData(4,&data,0,0);
				if (0 == ret_mag) {
					buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
					buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
					buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
				}
				i++;

				ALOGD("data[4/5][if=>] i = %d\n",i);


			}
		} else {
			ALOGD("[else=>]\n");
			ret_acc = readAccSensor(&data);
			printAccData(4,&data,0,0);
			if (0 == ret_acc) {
				buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
				buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
				buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
			}

			ret_mag = readMagSensor(&data);
			printMagnData(4,&data,0,0);
			if (0 == ret_mag) {
				buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
				buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
				buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
			}

			i++;
			ALOGD("data[4/5]else=> i = %d\n",i);
		}
		point = i;
		ALOGD("point:%d\n",point);

		if(1 == apk_exit) {
			ALOGD("apk_exit\n");
			break;
		}
	}
	ALOGD("\n");
	//filt data
	Filter(buf, SAMPLES);
	if (0 == ret_gyr) {
		file_content.calibration.gyrozx = buf[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyrozy = buf[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyrozz = buf[1].data.gyroMilliDegreesPerSecond.gyroZ;
	}

	delete []buf;


	ALOGD("The 4th step is completed\n");

	return 0;
}

int SetAGM_STEP_E()
{
	int i;
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;
	point = 0;

	//==============================================
	//step 5:
	ALOGD("[5/5]Hold the device vertical, rotate the device counter clockwise along the axis between bottom and top\n");
	//ALOGD("[5/5]Hold the device vertical with the windows button on the bottom, rotate the device counter clockwise 
	//along the axis between the top of the screen and the windows button\n");
	usleep(2000000);



	//read the data...
	static const int SAMPLES = 20;
	SENSOR_DATA_T* buf = new SENSOR_DATA_T[SAMPLES*3];

	for(i = 0; i< SAMPLES;) {
		usleep(200000); //gyro update is 20ms

		ret_gyr = readGyrSensor(&data);
		printGyroData(5,&data,0,0);

		if (0 == ret_gyr) {
			if(data.data.gyroMilliDegreesPerSecond.gyroX > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroX < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ < -45000)	{
				ALOGD("[if=>]\n");
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroX = data.data.gyroMilliDegreesPerSecond.gyroX;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroY = data.data.gyroMilliDegreesPerSecond.gyroY;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroZ = data.data.gyroMilliDegreesPerSecond.gyroZ;

				ret_acc = readAccSensor(&data);
				printAccData(5,&data,0,0);
				if (0 == ret_acc) {
					buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
					buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
					buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
				}

				ret_mag = readMagSensor(&data);
				printMagnData(5,&data,0,0);
				if (0 == ret_mag)
				{
					buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
					buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
					buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
				}
				i++;
				ALOGD("data[5/5][if=>] i = %d\n",i);
			}
		} else {
			ALOGD("[else=>]\n");

			ret_acc = readAccSensor(&data);
			printAccData(5,&data,0,1);
			if (0 == ret_acc) {
				buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
				buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
				buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
			}

			ret_mag = readMagSensor(&data);
			printMagnData(5,&data,0,0);
			if (0 == ret_mag) {
				buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
				buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
				buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
			}
			i++;
			ALOGD("data[5/5]else=> i = %d\n",i);
		}

		point = i;
		ALOGD("data[5/5]point:%d\n",point);

		if(1 == apk_exit) {
			ALOGD("apk_exit\n");
			break;
		}
	}

	//filt data
	Filter(buf, SAMPLES);
	if (0 == ret_gyr) {
		file_content.calibration.gyroyx = buf[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyroyy = buf[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyroyz = buf[1].data.gyroMilliDegreesPerSecond.gyroZ;
	}
	if (0 == ret_acc) {
		file_content.calibration.acclyx = buf[0].data.accelMilliG.accelX;
		file_content.calibration.acclyy = buf[0].data.accelMilliG.accelY;
		file_content.calibration.acclyz = buf[0].data.accelMilliG.accelZ;
	}

	delete []buf;

	ALOGD("The 5th step is completed\n");

	Calibrated = 1;
	file_content.config.calibrated = 1;

	return 0;
}

int SetAGM_STEP_F()
{

	WriteDataToFile(AGM_SENSOR_FILE_NAME, &file_content, sizeof(file_content));
	WriteDataToFileInTxt();

	ALOGD("Save the files.\n");

	return 0;

}


#if DEBUG_USE_ADB
int main(int argc, const char *argv[])
{
	if (argc == 2) {
		int ret;
		pthread_t tid;
		int status;

		if (!strcmp(argv[1], "agm")) {
			signal(SIGINT,child_handler);

			ret = pthread_create(&tid,NULL,sensorAGM_read_data_loop,(void*)0);
			if(ret) {
				ALOGD("create pthread error!ret = %d\n",ret);
				return -1;
			}
		}
		pthread_join(tid, (void **)&status);
		return 0;
	}

	SetAGM_STEP_A();
	SetAGM_STEP_B();
	SetAGM_STEP_C();
	SetAGM_STEP_D();
	SetAGM_STEP_E();
	SetAGM_STEP_F();

}
#endif
