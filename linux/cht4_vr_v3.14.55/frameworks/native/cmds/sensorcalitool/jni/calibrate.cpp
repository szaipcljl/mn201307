#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include <string.h> //memset()

//#include <fstream>
#include <cstring>
//#include <iostream>

#include "mySensorManager.h"



//int WriteDataToFile(const char *pathname, char *buf, size_t count);
void SwapData(SENSOR_DATA_T* a, SENSOR_DATA_T* b);
int Filter(SENSOR_DATA_T* data, int size);
int Calibrated = 0;


//int type;
int flag_finish = 0;
int flag_acc_finish = 0;
int flag_gyr_finish = 0;
int flag_mag_finish = 0;
SENSOR_DATA_T data;
static const int SKIP_DATA = 10;
static const int DATA_SIZE = 11;
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
		/*float magnx,*/(float)(-5),
		/*float magny,*/305UL,
		/*float magnz,*/(float)(-118),
		/*float magnxnx;*/(float)(-15),
		/*float magnxny;*/(float)(-15),
		/*float magnxnz;*/(float)(-15),
		/*float magnyux;*/(float)(-15),
		/*float magnyuy;*/(float)(-15),
		/*float magnyuz;*/(float)(-15),
		/*float magnxsx;*/(float)(-15),
		/*float magnxsy;*/(float)(-15),
		/*float magnxsz;*/(float)(-15),
		/*float magnydx;*/(float)(-15),
		/*float magnydy;*/(float)(-15),
		/*float magnydz;*/(float)(-15),
		/*float magnnx,*/(float)(-15),
		/*float magnny,*/268UL,
		/*float magnnz,*/(float)(-225),
		/*float magnsx,*/(float)(-13),
		/*float magnsy,*/(float)(-233),
		/*float magnsz,*/241UL,
		/*float acclx,*/8UL,
		/*float accly,*/(float)(-17),
		/*float acclz,*/(float)(-30),
		/*float acclzx;//horizontal*/(float)(-15),
		/*float acclzy;*/(float)(-15),
		/*float acclzz;*/(float)(-15),
		/*float acclyx;//vertical*/(float)(-15),
		/*float acclyy;*/(float)(-15),
		/*float acclyz;*/(float)(-15),
		/*float gyrox,*/0UL,
		/*float gyroy,*/(float)(234),
		/*float gyroz,*/(float)(-242),
		/*float gyrozx;*/(float)(0),
		/*float gyrozy;*/(float)(-0),
		/*float gyrozz;*/(float)(-45),
		/*float gyroyx;*/(float)(-0),
		/*float gyroyy;*/(float)(-45),
		/*float gyroyz;*/0UL,
		/*float als_curve[20]*/12UL,0UL,24UL,0UL,35UL,1UL,47UL,4UL,59UL,10UL,71UL,28UL,82UL,75UL,94UL,206UL,106UL,565UL,118UL,1547UL,
		/*float als_multiplier*/125UL

	}
};

#if 1
int WriteDataToFile(const char *pathname, /*char*/void *buf, size_t count)
{
	int fd;

	if((fd = open(pathname,O_RDWR | O_CREAT | O_APPEND,0664)) == -1){
		perror("open");
		return -1;
	}

	//while((n = read(fd,buf,sizeof(buf))) > 0){
	write(fd,buf,count);
	//}

	close(fd);

	return 0;
}


int WriteDataToFileInTxt()
{
	char buf[1024];
	memset(buf,0xff,sizeof(buf));

	//sprintf(buf, "=%f,=%f,=%f,\n", file_content.calibration., file_content.calibration., file_content.calibration.);
	sprintf(buf, "magnx=%f,magny=%f,magnz=%f,\nmagnxnx=%f,magnxny=%f,magnxnz=%f,\nmagnyux=%f,magnyuy=%f,magnyuz=%f,\nmagnxsx=%f,magnxsy=%f,magnxsz=%f,\n", \
			file_content.calibration.magnnx, file_content.calibration.magnnx, file_content.calibration.magnnx, \
			file_content.calibration.magnxnx, file_content.calibration.magnxny, file_content.calibration.magnxnz, \
			file_content.calibration.magnxsx, file_content.calibration.magnxsy, file_content.calibration.magnxsz, \
			file_content.calibration.magnyux, file_content.calibration.magnyuy, file_content.calibration.magnyuz);

	FILE* pFile = fopen("setAGM_cal.txt", "w+");
	if (!pFile)
		return -1;

	fwrite(buf, 1, sizeof(buf), pFile);
	fclose(pFile);

	return 0;
}
#else

int WriteDataToFile(char * pathname, void * pdata, size_t size)
{
	using namespace std;
	//save structure to a file
	ofstream outbal(pathname /*"file_content.bin"*/, ios::out | ios::binary);
	if(!outbal) {
		cout << "Cannot open file.\n";
		return 1;
	}

	outbal.write((char *) pdata /*&file_content*/,size);
	outbal.close();

	return 0;
}

#endif

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
		data[i].data.data.n = data[i].data.data.x*data[i].data.data.x+data[i].data.data.y*data[i].data.data.y+data[i].data.data.z*data[i].data.data.z;
	}
	for(i=0; i< size; i++) {
		for(j = 0; j <i; j++) {
			if(data[i*3].data.data.n <data[j*3].data.data.n) { //swap
				SwapData(&data[i*3], &data[j*3]);
			}
			if(data[i*3+1].data.data.n <data[j*3+1].data.data.n) { //swap
				SwapData(&data[i*3+1], &data[j*3+1]);
			}
			if(data[i*3+2].data.data.n <data[j*3+2].data.data.n) { //swap
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


int SetAGM_STEP_A()
{


	int i;

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	SensorManager &mgr(SensorManager::getInstance());
	sp<SensorEventQueue> queue = mgr.createEventQueue();
	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();

	Sensor const* const* list;
	int count;
	int incalibrate = 0;


	if (queue == NULL)
	{
		printf("createEventQueue returned NULL\n");
		return 0;
	} else if (queue_gyro == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} else if (queue_mag == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}
	else {
		count = mgr.getSensorList(&list);
	}

	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACCELEROMETER, incalibrate);
	if (sensor_thread == NULL){
		printf("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYROSCOPE, incalibrate);
	if (sensor_thread_gyro == NULL) {
		printf("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_MAGNETIC_FIELD, incalibrate);
	if (sensor_thread_mag == NULL) {
		printf("failed to create mag sensor thread\n");
		return 0;

	}

	printf("before run\n");

	for (int i=0 ; i<count ; i++) {
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACCELEROMETER:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYROSCOPE:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		printf("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		printf("get sensor of type:Gyr error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		printf("get sensor of type:Mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type Acc  error\n");
		//return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type gyro error\n");
		//return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type magcal error\n");
		//return 0;
	}

	printf("after acc gyro magcal enable\n");




	//==============================================
	//step 1:

	//printf("Please check sensor drivers status all ready!\n\n");
	printf("[1/6]Lay the device on a flat surface with the Windows button pointing due South\n");
	//printf("Press enter when ready or press s to skip.\n");

	//waiting for key pressed

	//read the data...

	sensor_thread->data = &data;
	sensor_thread_gyro->data = &data;
	sensor_thread_mag->data = &data;
	for(i =0; i < SKIP_DATA; i++) {// skip 10 data...

#if 0
		reta = readSensorData(data, SENSOR_TYPE_ACCELEROMETER);
		retg = readSensorData(data, SENSOR_TYPE_GYROSCOPE);
		retm = readSensorData(data, SENSOR_TYPE_MAGNETIC_FIELD);
#else

		sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
		sensor_thread_gyro->run("sensor-loop", PRIORITY_BACKGROUND);
		sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);
		usleep(100000);//note: magn update time is 50ms
#endif
		//printf("\b=>");
	}
	printf("====[data end]=====\n");
	printf("<accelX=%9.4f,accelY=%9.4f,accelZ=%9.4f>\n",
			data.data.accelMilliG.accelX, data.data.accelMilliG.accelY, data.data.accelMilliG.accelZ);
	printf("<gyroX=%9.4f,=%9.4f,=%9.4f>\n",
			data.data.gyroMilliDegreesPerSecond.gyroX, data.data.gyroMilliDegreesPerSecond.gyroY, data.data.gyroMilliDegreesPerSecond.gyroZ);
	printf("<magFieldX=%9.4f,magFieldX=%9.4f,magFieldX=%9.4f>\n",
			data.data.magFieldMilliGauss.magFieldX, data.data.magFieldMilliGauss.magFieldY, data.data.magFieldMilliGauss.magFieldZ);
	printf("====[data end]=====\n");

	for(i = 0; i < DATA_SIZE; i++) {
#if 0
		reta = readSensorData(temp[i*3], SENSOR_TYPE_ACCELEROMETER);
		retg = readSensorData(temp[i*3+1], SENSOR_TYPE_GYROSCOPE);
		retm = readSensorData(temp[i*3+2], SENSOR_TYPE_MAGNETIC_FIELD);
		//sleep(75); //note: magn update time is 50ms
		printf("\b=>");
#else
		sensor_thread->data = &temp[i*3];
		sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);

		sensor_thread_gyro->data = &temp[i*3+1];
		sensor_thread_gyro->run("sensor-loop", PRIORITY_BACKGROUND);

		sensor_thread_mag->data = &temp[i*3+2];
		sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);

		usleep(100000);//note: magn update time is 50ms

#endif

	}
	usleep(200000);

	printf("====[temp start]=====\n");
	for (i = 0; i < DATA_SIZE; i++) {
		printf("temp[%d]:<accelX=%9.4f,accelY=%9.4f,accelZ=%9.4f>\n",i*3,
				temp[i*3].data.accelMilliG.accelX, temp[i*3].data.accelMilliG.accelY, temp[i*3].data.accelMilliG.accelZ);
		printf("temp[%d]:<gyroX=%9.4f,=%9.4f,=%9.4f>\n",i*3+1,
				temp[i*3+1].data.gyroMilliDegreesPerSecond.gyroX, temp[i*3+1].data.gyroMilliDegreesPerSecond.gyroY, temp[i*3+1].data.gyroMilliDegreesPerSecond.gyroZ);
		printf("<temp[%d]:magFieldX=%9.4f,magFieldX=%9.4f,magFieldX=%9.4f>\n",i*3+2,
				temp[i*3+2].data.magFieldMilliGauss.magFieldX, temp[i*3+2].data.magFieldMilliGauss.magFieldY, temp[i*3+2].data.magFieldMilliGauss.magFieldZ);
	}
	printf("====[temp end]=====\n");

	Filter(temp, DATA_SIZE);
	if (1) {
		file_content.calibration.acclzx = temp[0].data.accelMilliG.accelX;
		file_content.calibration.acclzy = temp[0].data.accelMilliG.accelY;
		file_content.calibration.acclzz = temp[0].data.accelMilliG.accelZ;
	}
	if (1) {
		file_content.calibration.gyrox = temp[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyroy = temp[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyroz = temp[1].data.gyroMilliDegreesPerSecond.gyroZ;
	}
	if (1) {
		file_content.calibration.magnxnx = temp[2].data.magFieldMilliGauss.magFieldX;
		file_content.calibration.magnxny = temp[2].data.magFieldMilliGauss.magFieldY;
		file_content.calibration.magnxnz = temp[2].data.magFieldMilliGauss.magFieldZ;
	}


#if 0
	sensor_thread->join();
	sensor_thread_gyro->join();
	sensor_thread_mag->join();
	if (verbose)
		printf("sensor thread terminated\n");

	err = queue->disableSensor(sensor);
	if (err != NO_ERROR) {
		printf("disableSensor() for '%s'failed (%d)\n",
				getSensorName(type), err);
		return 0;
	}
#endif

	printf("The first step is completed\n");

	return 0;

}

int SetAGM_STEP_B()
{
	int i;

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	SensorManager &mgr(SensorManager::getInstance());
	sp<SensorEventQueue> queue = mgr.createEventQueue();
	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();

	Sensor const* const* list;
	int count;
	int incalibrate = 0;


	if (queue == NULL)
	{
		printf("createEventQueue returned NULL\n");
		return 0;
	} else if (queue_gyro == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} else if (queue_mag == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}
	else {
		count = mgr.getSensorList(&list);
	}

	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACCELEROMETER, incalibrate);
	if (sensor_thread == NULL){
		printf("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYROSCOPE, incalibrate);
	if (sensor_thread_gyro == NULL) {
		printf("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_MAGNETIC_FIELD, incalibrate);
	if (sensor_thread_mag == NULL) {
		printf("failed to create mag sensor thread\n");
		return 0;

	}

	printf("before run\n");

	for (int i=0 ; i<count ; i++) {
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACCELEROMETER:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYROSCOPE:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		printf("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		printf("get sensor of type:Gyro error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		printf("get sensor of type:Mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type Acc  error\n");
		//return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type gyro error\n");
		//return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type magcal error\n");
		//return 0;
	}

	printf("after acc gyro magcal enable\n");




	//==============================================
	//step 2:

	printf("[2/6]Rotate the screen 180 degrees clockwise so the Windows button is pointing due North\n");
	//printf("Press enter when ready or press s to skip.\n");

	//read the data...
	for(i = 0; i < DATA_SIZE; i++) {
#if 0
		retm = readSensorData(temp[i*3+2], SENSOR_TYPE_MAGNETIC_FIELD);
		//sleep(75); //note: magn update time is 50ms
		printf("\b=>");
#else
		sensor_thread_mag->data = &temp[i*3+2];
		sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);

		usleep(100000);

#endif
	}
	printf("\n");
	Filter(temp, DATA_SIZE);

	if (1) {
		file_content.calibration.magnxsx = temp[2].data.magFieldMilliGauss.magFieldX;
		file_content.calibration.magnxsy = temp[2].data.magFieldMilliGauss.magFieldY;

	}




#if 0
	sensor_thread->join();
	sensor_thread_gyro->join();
	sensor_thread_mag->join();
	if (verbose)
		printf("sensor thread terminated\n");

	err = queue->disableSensor(sensor);
	if (err != NO_ERROR) {
		printf("disableSensor() for '%s'failed (%d)\n",
				getSensorName(type), err);
		return 0;
	}

#endif

	printf("The 2nd step is completed\n");
	return 0;
}

int SetAGM_STEP_C()
{
	int i;

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	SensorManager &mgr(SensorManager::getInstance());
	sp<SensorEventQueue> queue = mgr.createEventQueue();
	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();

	Sensor const* const* list;
	int count;
	int incalibrate = 0;


	if (queue == NULL)
	{
		printf("createEventQueue returned NULL\n");
		return 0;
	} else if (queue_gyro == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} else if (queue_mag == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}
	else {
		count = mgr.getSensorList(&list);
	}

	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACCELEROMETER, incalibrate);
	if (sensor_thread == NULL){
		printf("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYROSCOPE, incalibrate);
	if (sensor_thread_gyro == NULL) {
		printf("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_MAGNETIC_FIELD, incalibrate);
	if (sensor_thread_mag == NULL) {
		printf("failed to create mag sensor thread\n");
		return 0;

	}

	printf("before run\n");

	for (int i=0 ; i<count ; i++) {
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACCELEROMETER:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYROSCOPE:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		printf("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		printf("get sensor of type:gyr error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		printf("get sensor of type:mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type Acc  error\n");
		//return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type gyro error\n");
		//return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type magcal error\n");
		//return 0;
	}

	printf("after acc gyro magcal enable\n");




	//==============================================
	//step 3:
	printf("[3/6]Now Lay the device flat, screen down with the windows button pointing due South\n");
	//printf("Press enter when ready or press s to skip.\n");

	//read the data...
	for(i = 0; i < DATA_SIZE; i++) {
#if 0
		retm = readSensorData(temp[i*3+2], SENSOR_TYPE_MAGNETIC_FIELD);
		//sleep(75); //note: magn update time is 50ms
		printf("\b=>");
#else
		sensor_thread_mag->data = &temp[i*3+2];
		sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);

		usleep(100000);
#endif
	}
	printf("\n");
	Filter(temp, DATA_SIZE);
	if (1) {
		file_content.calibration.magnxsz = temp[2].data.magFieldMilliGauss.magFieldZ;
	}


	printf("The 3rd step is completed\n");

	return 0;
}


int SetAGM_STEP_D()
{
	int i;

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	SensorManager &mgr(SensorManager::getInstance());
	sp<SensorEventQueue> queue = mgr.createEventQueue();
	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();

	Sensor const* const* list;
	int count;
	int incalibrate = 0;


	if (queue == NULL)
	{
		printf("createEventQueue returned NULL\n");
		return 0;
	} else if (queue_gyro == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} else if (queue_mag == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}
	else {
		count = mgr.getSensorList(&list);
	}

	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACCELEROMETER, incalibrate);
	if (sensor_thread == NULL){
		printf("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYROSCOPE, incalibrate);
	if (sensor_thread_gyro == NULL) {
		printf("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_MAGNETIC_FIELD, incalibrate);
	if (sensor_thread_mag == NULL) {
		printf("failed to create mag sensor thread\n");
		return 0;

	}

	printf("before run\n");

	for (int i=0 ; i<count ; i++) {
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACCELEROMETER:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYROSCOPE:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		printf("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		printf("get sensor of type:Gyr error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		printf("get sensor of type:Mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type Acc  error\n");
		//return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type gyro error\n");
		//return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type magcal error\n");
		//return 0;
	}

	printf("after acc gyro magcal enable\n");




	//==============================================
	//step 4:
	printf("[4/6]Lay the device flat with the screen up, rotate the device counter clockwise\n");
	//printf("Press enter when ready or press s to skip.\n");
	//FlushKeyBuffer();

	//read the data...
	static const int SAMPLES = 20;
	SENSOR_DATA_T* buf = new SENSOR_DATA_T[SAMPLES*3];

	sensor_thread_gyro->data = &data;
	sensor_thread->data = &data;
	sensor_thread_mag->data = &data;
	for(i = 0; i< SAMPLES;)	{
#if 0
		//sleep(20); //gyro update is 20ms
		retg = readSensorData(data, SENSOR_TYPE_GYROSCOPE);
#else
		sensor_thread_gyro->run("sensor-loop", PRIORITY_BACKGROUND);

#endif
		if (1) {//retg
			if(data.data.gyroMilliDegreesPerSecond.gyroX > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroX < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ < -45000)	{
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroX = data.data.gyroMilliDegreesPerSecond.gyroX;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroY = data.data.gyroMilliDegreesPerSecond.gyroY;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroZ = data.data.gyroMilliDegreesPerSecond.gyroZ;


#if 0
				reta = readSensorData(data, SENSOR_TYPE_ACCELEROMETER);
				if (0 == reta) {
					buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
					buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
					buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
				}

				retm = readSensorData(data, SENSOR_TYPE_MAGNETIC_FIELD);
				if (0 == retm) {
					buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
					buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
					buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
				}
				i++;
				printf("\b=>");
#else
				sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
				if (1) {
					buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
					buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
					buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
				}

				sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);
				if (1) {
					buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
					buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
					buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
				}
				i++;



#endif
			}
		} else {
#if 0
			reta = readSensorData(data, SENSOR_TYPE_ACCELEROMETER);
			if (0 == reta) {
				buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
				buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
				buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
			}

			retm = readSensorData(data, SENSOR_TYPE_MAGNETIC_FIELD);
			if (0 == retm) {
				buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
				buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
				buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
			}
#else
			sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
			if (1) {
				buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
				buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
				buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
			}

			sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);
			if (1) {
				buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
				buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
				buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
			}

#endif
			i++;
			printf("\b=>");
		}
		//sleep(30);
	}
	printf("\n");
	Filter(buf, SAMPLES);
	if (1/*0 == retg*/) {
		file_content.calibration.gyrozx = buf[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyrozy = buf[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyrozz = buf[1].data.gyroMilliDegreesPerSecond.gyroZ;
	}
	/*
	//calibrate magnatic
	CalibrateZ(buf, SAMPLES, offsetx, offsety, offsetz);
	*/
	delete []buf;


	printf("The 4th step is completed\n");

	return 0;
}

int SetAGM_STEP_E()
{
	int i;

	int err;
	int delay = 20, type, verbose = 0;
	int batch_time_ms = 0, wakeup=0;

	sp<SensorThread> sensor_thread;
	sp<SensorThread> sensor_thread_gyro;
	sp<SensorThread> sensor_thread_mag;

	SensorManager &mgr(SensorManager::getInstance());
	sp<SensorEventQueue> queue = mgr.createEventQueue();
	sp<SensorEventQueue> queue_gyro = mgr.createEventQueue();
	sp<SensorEventQueue> queue_mag = mgr.createEventQueue();

	Sensor const* const* list;
	int count;
	int incalibrate = 0;


	if (queue == NULL)
	{
		printf("createEventQueue returned NULL\n");
		return 0;
	} else if (queue_gyro == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	} else if (queue_mag == NULL) {
		printf("queue_gyro: createEventQueue returned NULL\n");
		return 0;
	}
	else {
		count = mgr.getSensorList(&list);
	}

	Sensor const *sensor = NULL;
	Sensor const *sensor_gyro = NULL;
	Sensor const *sensor_mag = NULL;
	sensor_thread = new SensorThread(queue, SENSOR_TYPE_ACCELEROMETER, incalibrate);
	if (sensor_thread == NULL){
		printf("failed to create sensor thread\n");
		return 0;
	}
	sensor_thread_gyro = new SensorThread(queue_gyro, SENSOR_TYPE_GYROSCOPE, incalibrate);
	if (sensor_thread_gyro == NULL) {
		printf("failed to create gyro sensor thread\n");
		return 0;
	}
	sensor_thread_mag = new SensorThread(queue_mag, SENSOR_TYPE_MAGNETIC_FIELD, incalibrate);
	if (sensor_thread_mag == NULL) {
		printf("failed to create mag sensor thread\n");
		return 0;

	}

	printf("before run\n");

	for (int i=0 ; i<count ; i++) {
		if(list[i]->isWakeUpSensor() == wakeup) {
			switch (list[i]->getType()) {
			case SENSOR_TYPE_ACCELEROMETER:
				sensor = list[i];
				break;
			case SENSOR_TYPE_GYROSCOPE:
				sensor_gyro = list[i];
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				sensor_mag = list[i];
				break;
			}
		}
	}
	if (sensor == NULL){
		printf("get sensor of type:Acc error\n");
		return 0;
	}
	if (sensor_gyro == NULL) {
		printf("get sensor of type:Gyr error\n");
		return 0;
	}
	if (sensor_mag == NULL) {
		printf("get sensor of type:Mag error\n");
		return 0;
	}

	if (queue->enableSensor(sensor->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type Acc  error\n");
		//return 0;
	}

	if (queue_gyro->enableSensor(sensor_gyro->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type gyro error\n");
		//return 0;
	}

	if (queue_mag->enableSensor(sensor_mag->getHandle(), ns2us(ms2ns(delay)),
				ns2us(ms2ns(batch_time_ms)), wakeup ? SENSORS_BATCH_WAKE_UPON_FIFO_FULL : 0) != NO_ERROR) {
		printf("enable sensor of type magcal error\n");
		//return 0;
	}

	printf("after acc gyro magcal enable\n");




	//==============================================

	//step 5:
	printf("[5/6]Hold the device vertical with the windows button on the bottom, rotate the device counter clockwise along the axis between the top of the screen and the windows button\n");
	//printf("Press enter when ready or press s to skip.\n");
	//FlushKeyBuffer();



	//read the data...
	static const int SAMPLES = 20;
	SENSOR_DATA_T* buf = new SENSOR_DATA_T[SAMPLES*3];

	sensor_thread->data = &data;
	sensor_thread_gyro->data = &data;
	sensor_thread_mag->data = &data;
	for(i = 0; i< SAMPLES;) {
		usleep(200000); //gyro update is 20ms

		//retg = readSensorData(data, SENSOR_TYPE_GYROSCOPE);
		sensor_thread_gyro->run("sensor-loop", PRIORITY_BACKGROUND);

		printf("in for\n");
		usleep(200000);
		if (1/*0 == retg*/) {
			if(data.data.gyroMilliDegreesPerSecond.gyroX > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroX < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroY < -45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ > 45000 ||
					data.data.gyroMilliDegreesPerSecond.gyroZ < -45000)	{
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroX = data.data.gyroMilliDegreesPerSecond.gyroX;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroY = data.data.gyroMilliDegreesPerSecond.gyroY;
				buf[i*3+1].data.gyroMilliDegreesPerSecond.gyroZ = data.data.gyroMilliDegreesPerSecond.gyroZ;
				//reta = readSensorData(data, SENSOR_TYPE_ACCELEROMETER);
				sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
				if (1/*0 == reta*/) {
					buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
					buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
					buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
				}
				//retm = readSensorData(data, SENSOR_TYPE_MAGNETIC_FIELD);
				sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);
				usleep(10000);
				if (1/*0 == retm*/)
				{
					buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
					buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
					buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
				}
				i++;
				printf("if=>\n");
			}
		} else {
			//reta = readSensorData(data, SENSOR_TYPE_ACCELEROMETER);
			sensor_thread->run("sensor-loop", PRIORITY_BACKGROUND);
			usleep(10000);
			if (1/*0 == reta*/) {
				buf[i*3].data.accelMilliG.accelX = data.data.accelMilliG.accelX;
				buf[i*3].data.accelMilliG.accelY = data.data.accelMilliG.accelY;
				buf[i*3].data.accelMilliG.accelZ = data.data.accelMilliG.accelZ;
			}
			//retm = readSensorData(data, SENSOR_TYPE_MAGNETIC_FIELD);
			sensor_thread_mag->run("sensor-loop", PRIORITY_BACKGROUND);
			usleep(10000);
			if (1/*0 == retm*/) {
				buf[i*3+2].data.magFieldMilliGauss.magFieldX = data.data.magFieldMilliGauss.magFieldX;
				buf[i*3+2].data.magFieldMilliGauss.magFieldY = data.data.magFieldMilliGauss.magFieldY;
				buf[i*3+2].data.magFieldMilliGauss.magFieldZ = data.data.magFieldMilliGauss.magFieldZ;
			}
			i++;
			printf("else=>\n");
		}
		usleep(30000);
	}
	printf("\n");
	Filter(buf, SAMPLES);
	if (1/*0 == retg*/) {
		file_content.calibration.gyroyx = buf[1].data.gyroMilliDegreesPerSecond.gyroX;
		file_content.calibration.gyroyy = buf[1].data.gyroMilliDegreesPerSecond.gyroY;
		file_content.calibration.gyroyz = buf[1].data.gyroMilliDegreesPerSecond.gyroZ;
	}
	if (1/*0 == reta*/) {
		file_content.calibration.acclyx = buf[0].data.accelMilliG.accelX;
		file_content.calibration.acclyy = buf[0].data.accelMilliG.accelY;
		file_content.calibration.acclyz = buf[0].data.accelMilliG.accelZ;
	}
	/*
	//calibrate magnatic
	CalibrateZ(buf, SAMPLES, offsetx, offsety, offsetz);
	*/
	delete []buf;

	printf("The 5th step is completed\n");

	Calibrated = 1;
	file_content.config.calibrated = 1;

	return 0;
}

int SetAGM_STEP_F()
{

	WriteDataToFile(AGM_SENSOR_FILE_NAME, &file_content, sizeof(file_content));
	//saveStructureToFile("file_content.bin", &file_content, sizeof(SENSOR_CALIBRATION));

	WriteDataToFileInTxt();
	printf("Done.\n");

	return 0;

}

#if 0
int main(int argc, const char *argv[])
{
	SetAGM_STEP_A();
	SetAGM_STEP_B();
	SetAGM_STEP_C();
	//SetAGM_STEP_D();
	//SetAGM_STEP_E();
	SetAGM_STEP_F();

}
#endif
