#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //memset()
#include <sys/time.h>
#include <cstring>

#include "calibrate.h"
#include "sensorthread.h"

#ifdef DEBUG_USE_ADB
#define AGM_SENSOR_BIN_FILE_NAME	"/data/sensor_cali_AGM_by_exe.bin"
#define AGM_SENSOR_TXT_FILE_NAME	"/data/sensor_cali_AGM_by_exe.txt"
int print_acc_log = 1;
int print_gyr_log = 1;
int print_mag_log = 1;
int calc_gyr_integral = 1;
char* reqSnr_g [3];
#else
//#define AGM_SENSOR_BIN_FILE_NAME	"/storage/emulated/0/sensor_calibration_AGM.bin"
//#define AGM_SENSOR_TXT_FILE_NAME	"/storage/emulated/0/setAGM_cal.txt"
#define AGM_SENSOR_BIN_FILE_NAME	"/data/sensor_calibration_AGM.bin"
#define AGM_SENSOR_TXT_FILE_NAME	"/data/setAGM_cal.txt"
#endif

#define TXT_BUF_SIZE 1024
#define ct_size 20


void SwapData(SENSOR_DATA_T* a, SENSOR_DATA_T* b);
int Filter(SENSOR_DATA_T* data, int size);

int Calibrated = 0;

int enable_finish = 0;
int apk_exit = 0;
int point = 0;
int agm_data[9] = {0};

extern SENSOR_DATA_T data_loop_acc;
extern SENSOR_DATA_T data_loop_gyr;
extern SENSOR_DATA_T data_loop_mag;

struct my_fifo acc_fifo;
struct my_fifo gyr_fifo;
struct my_fifo mag_fifo;

#define MAX_FRQ(a,b) (a > b ? a:b)
#define ACC_FRQ 1000
#define GYR_FRQ 1000
#define MAG_FRQ 100

extern void *sensorAGM_read_data_loop(void *arg);

static const int SKIP_DATA = 100;
static const int DATA_SIZE_A = 2 * 10000;
static const int DATA_SIZE = 100;
SENSOR_DATA_T temp[3];
SENSOR_CALC_TOOL calc_tool[3];
#define ACC_INDEX 0
#define GYR_INDEX 1
#define MAG_INDEX 2

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

int GetCurrentTime(char *szCurTime, int bufsz)
{
	time_t  tTime;
	struct tm *localTime;

	time(&tTime);
	localTime = localtime(&tTime);
	if (NULL == localTime)
		return -1;

	snprintf(szCurTime, bufsz, "%d-%02d-%02d %02d:%02d:%02d",
			localTime->tm_year+1900, localTime->tm_mon+1, localTime->tm_mday,
			localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

	return 0;
}

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
	char szCurTime[ct_size];
	int len = 0;
	memset(buf,0xff,sizeof(buf));
	memset(szCurTime,0,sizeof(szCurTime));

	GetCurrentTime(szCurTime, sizeof(szCurTime));

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
			"als_multiplier=%d\n\n>>end<<\ntime: %s\n", \
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
			file_content.calibration.als_multiplier, szCurTime);

	ALOGD("len = %d\n", len);
	if (0 > len) {
		ALOGD("an output error for setAGM_cal.txt, len = %d\n.", len);
		return -1;
	}

	FILE* pFile = fopen(AGM_SENSOR_TXT_FILE_NAME, "w+");
	if (!pFile)
		return -1;

	ALOGD("open .txt file.\n");
	fwrite(buf, 1, len, pFile);
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

int readAccSensor(SENSOR_DATA_T *data)
{
	int i = 0;
	int read_again = 0;
	int ret = 0;
	int len;

#if 1
	while (!(ret = myfifo_out(&acc_fifo, (void*)data, 1)));
	//len = my_fifo_len(acc_fifo);
	//ALOGD("%s:len = %d\n",len);
#else
	do {

		if (++i > 500) {
			ALOGD("can not read acc sensor data\n");
			return -1; //no data
		}

		if (data->snr_time.acc_time == data_loop_acc.snr_time.acc_time) {
			read_again = 1;
			usleep(200);//1s / 500 -> 2 ms
			continue;
		}

		data->data.accelMilliG.accelX = data_loop_acc.data.accelMilliG.accelX;
		data->data.accelMilliG.accelY = data_loop_acc.data.accelMilliG.accelY;
		data->data.accelMilliG.accelZ = data_loop_acc.data.accelMilliG.accelZ;
		data->snr_time.acc_time = data_loop_acc.snr_time.acc_time;

		if (0 == data->data.accelMilliG.accelX && 0 == data->data.accelMilliG.accelY && 0 == data->data.accelMilliG.accelZ) {
			usleep(200);//1s / 500 -> 2 ms
			read_again = 1;
		} else {
			read_again = 0;
		}
	} while(read_again);
#endif

	return 0;
}

int readGyrSensor(SENSOR_DATA_T *data)
{
	int i = 0;
	int read_again = 0;
	int ret = 0;

#if 1
	while (!(ret = myfifo_out(&gyr_fifo, (void*)data, 1)));
#else
	do {
		if (++i > 500) {
			ALOGD("can not read gyro sensor data\n");
			return -1; //no data
		}

		if (data->snr_time.gyr_time == data_loop_gyr.snr_time.gyr_time) {
			read_again = 1;
			usleep(200);//1s / 500 -> 2 ms
			continue;
		}

		data->data.gyroMilliDegreesPerSecond.gyroX = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroX;
		data->data.gyroMilliDegreesPerSecond.gyroY = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroY;
		data->data.gyroMilliDegreesPerSecond.gyroZ = data_loop_gyr.data.gyroMilliDegreesPerSecond.gyroZ;
		data->snr_time.gyr_time = data_loop_gyr.snr_time.gyr_time;

		if(0 == data->data.gyroMilliDegreesPerSecond.gyroX && 0 == data->data.gyroMilliDegreesPerSecond.gyroY && 0 == data->data.gyroMilliDegreesPerSecond.gyroZ) {
			usleep(200);//1s / 500 -> 2 ms
			read_again = 1;
		} else {
			read_again = 0;
		}
	} while(read_again);
#endif

	return 0;
}


int readMagSensor(SENSOR_DATA_T *data)
{
	int i = 0;
	int read_again = 0;
	int ret = 0;

#if 1
	while (!(ret = myfifo_out(&mag_fifo, (void*)data, 1)));
#else
	do {
		if (++i > 500) {
			ALOGD("can not read magn sensor data\n");
			return -1; //no data
		}

		if (data->snr_time.mag_time == data_loop_mag.snr_time.mag_time) {
			read_again = 1;
			usleep(200);//1s / 500 -> 2 ms
			continue;
		}

		data->data.magFieldMilliGauss.magFieldX = data_loop_mag.data.magFieldMilliGauss.magFieldX;
		data->data.magFieldMilliGauss.magFieldY = data_loop_mag.data.magFieldMilliGauss.magFieldY;
		data->data.magFieldMilliGauss.magFieldZ = data_loop_mag.data.magFieldMilliGauss.magFieldZ;
		data->snr_time.mag_time = data_loop_mag.snr_time.mag_time;

		if (0 == data->data.magFieldMilliGauss.magFieldX && 0 == data->data.magFieldMilliGauss.magFieldY && 0 == data->data.magFieldMilliGauss.magFieldZ) {
			usleep(200);//1s / 500 -> 2 ms
			read_again = 1;
		} else {
			read_again = 0;
		}
	} while(read_again);
#endif

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


void printAccData(int step, SENSOR_DATA_T *data, int count,int size, int index)
{
	ALOGD("[%d/5][%d/%d]temp[%2d]", step, count, size,index);

	ALOGD(": <accelX   =%7d,\taccelY   =%7d,\taccelZ   =%7d,\ttime  =%lld>\n",\
			data->data.accelMilliG.accelX,\
			data->data.accelMilliG.accelY,\
			data->data.accelMilliG.accelZ,\
			data->snr_time.acc_time);
}

void printGyroData(int step, SENSOR_DATA_T *data, int count, int size, int index)
{
	ALOGD("[%d/5][%d/%d]temp[%2d]", step, count, size,index);

	ALOGD(": <gyroX    =%7d,\tgyroY    =%7d,\tgyroZ    =%7d,\ttime  =%lld>\n",\
			data->data.gyroMilliDegreesPerSecond.gyroX,\
			data->data.gyroMilliDegreesPerSecond.gyroY,\
			data->data.gyroMilliDegreesPerSecond.gyroZ,\
			data->snr_time.gyr_time);
}

void printMagnData(int step, SENSOR_DATA_T *data, int count, int size, int index)
{
	ALOGD("[%d/5][%d/%d]temp[%2d]", step, count, size,index);

	ALOGD(": <magFieldX=%7d,\tmagFieldY=%7d,\tmagFieldZ=%7d,\ttime=%lld>\n",\
			data->data.magFieldMilliGauss.magFieldX,\
			data->data.magFieldMilliGauss.magFieldY,\
			data->data.magFieldMilliGauss.magFieldZ,\
			data->snr_time.mag_time);
}

void skipData(int count)
{
	int i;

	for(i =0; i < count; i++) {// skip 10 data...
		readAccSensor(&temp[ACC_INDEX]);
		printAccData(1,&temp[ACC_INDEX],i,count,ACC_INDEX);

		readGyrSensor(&temp[GYR_INDEX]);
		printGyroData(1,&temp[GYR_INDEX],i,count,GYR_INDEX);

		readMagSensor(&temp[MAG_INDEX]);
		printMagnData(1,&temp[MAG_INDEX],i,count,MAG_INDEX);

		//usleep(2000);//note: magn update time is 2ms
	}
}

typedef long long int int_64;
void calc_agm_sum_avg(SENSOR_CALC_TOOL* calc_tool, SENSOR_DATA_T* data, int_64 count)
{
	int i;

	for (i = 0; i < 3; i ++) {
		ALOGD("sum_x = %lld,sum_y = %lld,sum_z = %lld,\tavg_x = %lld,\tavg_y = %lld,\tavg_z = %lld,\tcount = %lld\n",
				calc_tool[i].snr_sum.sum_x, calc_tool[i].snr_sum.sum_y,calc_tool[i].snr_sum.sum_z,
				calc_tool[i].snr_avg.avg_x, calc_tool[i].snr_avg.avg_y, calc_tool[i].snr_avg.avg_z, count);
		calc_tool[i].snr_sum.sum_x += (int_64)(data[i].data.data.x);
		calc_tool[i].snr_sum.sum_y += (int_64)(data[i].data.data.y);
		calc_tool[i].snr_sum.sum_z += (int_64)(data[i].data.data.z);

		calc_tool[i].snr_avg.avg_x = calc_tool[i].snr_sum.sum_x / count;
		calc_tool[i].snr_avg.avg_y = calc_tool[i].snr_sum.sum_y / count;
		calc_tool[i].snr_avg.avg_z = calc_tool[i].snr_sum.sum_z / count;
	}
}

void calc_single_sum_avg(SENSOR_CALC_TOOL* calc_tool, SENSOR_DATA_T* data, int_64 count, int index)
{
	ALOGD("sum_x = %lld,sum_y = %lld,sum_z = %lld,\tavg_x = %lld,\tavg_y = %lld,\tavg_z = %lld,\tcount = %lld\n",
			calc_tool[index].snr_sum.sum_x, calc_tool[index].snr_sum.sum_y,calc_tool[index].snr_sum.sum_z,
			calc_tool[index].snr_avg.avg_x, calc_tool[index].snr_avg.avg_y, calc_tool[index].snr_avg.avg_z, count);

	calc_tool[index].snr_sum.sum_x += (int_64)(data[index].data.data.x);
	calc_tool[index].snr_sum.sum_y += (int_64)(data[index].data.data.y);
	calc_tool[index].snr_sum.sum_z += (int_64)(data[index].data.data.z);

	calc_tool[index].snr_avg.avg_x = calc_tool[index].snr_sum.sum_x / count;
	calc_tool[index].snr_avg.avg_y = calc_tool[index].snr_sum.sum_y / count;
	calc_tool[index].snr_avg.avg_z = calc_tool[index].snr_sum.sum_z / count;
}

void get_amg_snr_avg(SENSOR_CALC_TOOL* calc_tool, SENSOR_DATA_T* data)
{
	int i = 3;

	for (i = 0; i < 3; i ++) {
		data[i].data.data.x = (int)(calc_tool[i].snr_avg.avg_x);
		data[i].data.data.y = (int)(calc_tool[i].snr_avg.avg_y);
		data[i].data.data.z = (int)(calc_tool[i].snr_avg.avg_z);
	}
}

void get_single_snr_avg(SENSOR_CALC_TOOL* calc_tool, SENSOR_DATA_T* data, int index)
{

	data[index].data.data.x = (int)(calc_tool[index].snr_avg.avg_x);
	data[index].data.data.y = (int)(calc_tool[index].snr_avg.avg_y);
	data[index].data.data.z = (int)(calc_tool[index].snr_avg.avg_z);
}

int collect_agm_static_data_avg(int size)
{
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;
	int i, max_frq,acc_count = 0, gyr_count = 0, mag_count = 0;

	struct timeval tv, tv1;
	gettimeofday(&tv, NULL);

	ALOGD("enter %s, timestamp is %d, %lld \n", __func__, tv.tv_sec, tv.tv_usec);

	myfifo_reset(&acc_fifo);
	myfifo_reset(&gyr_fifo);
	myfifo_reset(&mag_fifo);

	max_frq = MAX_FRQ(ACC_FRQ,GYR_FRQ);
	max_frq = MAX_FRQ(max_frq,MAG_FRQ);
	int acc_samp_period = max_frq/ACC_FRQ;
	int gyr_samp_period = max_frq/GYR_FRQ;
	int mag_samp_period = max_frq/MAG_FRQ;

	for(i = 1; i <= size; i++) {//now read the data...
		ALOGD("\n");

#if 0
		ret_acc = readAccSensor(&temp[ACC_INDEX]);
		printAccData(1,&temp[ACC_INDEX],i,size,ACC_INDEX);
		if (-1 == ret_acc)
			return -1;

		ret_gyr = readGyrSensor(&temp[GYR_INDEX]);
		printGyroData(1,&temp[GYR_INDEX],i,size,GYR_INDEX);
		if (-1 == ret_gyr)
			return -1;

		ret_mag = readMagSensor(&temp[MAG_INDEX]);
		printMagnData(1,&temp[MAG_INDEX],i,size,MAG_INDEX);
		if (-1 == ret_mag)
			return -1;

		calc_agm_sum_avg(calc_tool, temp, i);
#else

		if (!(i % acc_samp_period)) {
			acc_count++;
			ret_acc = readAccSensor(&temp[ACC_INDEX]);
			printAccData(1,&temp[ACC_INDEX], acc_count, size,ACC_INDEX);
			if (-1 == ret_acc)
				return -1;

			calc_single_sum_avg(calc_tool, temp, acc_count, ACC_INDEX);
		}

		if (!(i % gyr_samp_period)) {
			gyr_count++;
			ret_gyr = readGyrSensor(&temp[GYR_INDEX]);
			printGyroData(1,&temp[GYR_INDEX], gyr_count,size,GYR_INDEX);
			if (-1 == ret_gyr)
				return -1;
			calc_single_sum_avg(calc_tool, temp, gyr_count, GYR_INDEX);
		}

		if (!(i % mag_samp_period)) {
			mag_count++;
			ret_mag = readMagSensor(&temp[MAG_INDEX]);
			printMagnData(1,&temp[MAG_INDEX], mag_count,size,MAG_INDEX);
			if (-1 == ret_mag)
				return -1;
			calc_single_sum_avg(calc_tool, temp, mag_count, MAG_INDEX);
		}
#endif

		//usleep(2000);//note: magn update time is 2ms

		if (1 == apk_exit)
			return 0;
	}

	gettimeofday(&tv1, NULL);
	ALOGD("Done %s, latency is %d s, %lld us \n", __func__, tv1.tv_sec - tv.tv_sec, tv1.tv_usec - tv.tv_usec);

	return 0;
}

int collect_rota_data_avg(int step, int size)
{
	int i;
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;

	//static const int SAMPLES = 20 * 1000;
	//SENSOR_DATA_T* buf = new SENSOR_DATA_T[SAMPLES*3];

	for(i = 1; i < size;) {
		if(1 == apk_exit)
			return 0;

		//usleep(2000); //gyro update is 2ms

		ret_gyr = readGyrSensor(&temp[GYR_INDEX]);
		printGyroData(step,&temp[GYR_INDEX],i, size,GYR_INDEX);
		if (0 != ret_gyr)
			return -1;


		if (0 == ret_gyr) {
			if(temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroX > 45000 ||
					temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroX < -45000 ||
					temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroY > 45000 ||
					temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroY < -45000 ||
					temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroZ > 45000 ||
					temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroZ < -45000) {
				ALOGD("[%d/5][if=>]\n",step);
				calc_single_sum_avg(calc_tool,temp, i, GYR_INDEX);

				ret_acc = readAccSensor(&temp[ACC_INDEX]);
				printAccData(step,&temp[ACC_INDEX],i,size,ACC_INDEX);
				if (0 == ret_acc) {
					calc_single_sum_avg(calc_tool,temp, i, ACC_INDEX);
				}

				ret_mag = readMagSensor(&temp[MAG_INDEX]);
				printMagnData(step,&temp[MAG_INDEX],i,size,MAG_INDEX);
				if (0 == ret_mag){
					calc_single_sum_avg(calc_tool,temp, i, MAG_INDEX);
				}
				i++;
				ALOGD("data[%d/5][if=>] i = %d\n", step,i);
			}
		} else {
			ALOGD("[%d/5][else=>]\n", step);

			ret_acc = readAccSensor(&temp[ACC_INDEX]);
			printAccData(step,&temp[ACC_INDEX],i, size,ACC_INDEX);
			if (0 == ret_acc) {
				calc_single_sum_avg(calc_tool,temp, i, ACC_INDEX);
			}

			ret_mag = readMagSensor(&temp[MAG_INDEX]);
			printMagnData(step,&temp[MAG_INDEX],i, size,MAG_INDEX);
			if (0 == ret_mag) {
				calc_single_sum_avg(calc_tool,temp, i, MAG_INDEX);
			}
			i++;
			ALOGD("data[%d/5]else=> i = %d\n", step, i);
		}

		point = i;
		ALOGD("data[%d/5]point:%d\n", step, point);
	}

	return 0;
}

int collect_static_magn_data_avg(int step, int size)
{
	int i;
	int ret_mag = 0;

	for(i = 1; i < size; i++) {
		if (1 == apk_exit)
			return 0;

		ret_mag = readMagSensor(&temp[MAG_INDEX]);
		printMagnData(step,&temp[MAG_INDEX],i,size,MAG_INDEX);
		if (0 != ret_mag)
			return -1;

		calc_single_sum_avg(calc_tool, temp, i, MAG_INDEX);

		//usleep(2000); //note: magn update time is 2ms
	}

	return 0;
}

int SetAGM_STEP_A()
{
	apk_exit = 0;//avoid to run app again failed
	enable_finish = 0;//wait for enable finish when run app again

	int i;
	int ret;
	pthread_t tid;

#ifdef DEBUG_USE_ADB
	//signal(SIGINT,child_handler);
#endif

	memset(calc_tool, 0,sizeof(calc_tool));
	memset(temp, 0,sizeof(temp));

	ret = pthread_create(&tid,NULL,sensorAGM_read_data_loop, NULL);
	if(ret) {
		ALOGD("create pthread error!ret = %d\n",ret);
		return -1;
	}

	ALOGD("enable_finish= %d\n",enable_finish);
	while(!enable_finish)
		usleep(10000);


	//==============================================
	//step 1:

	ALOGD("[1/5]Lay the device on a flat surface, north, east, screen up \n");
	//ALOGD("[1/5]Lay the device on a flat surface with the Windows button pointing due South\n");

	//read the data...
	skipData(SKIP_DATA);

	memset(calc_tool, 0, sizeof(calc_tool));
	//to select data
	ret = collect_agm_static_data_avg(DATA_SIZE_A);
	if (ret != 0)
		return -1;

	get_amg_snr_avg(calc_tool, temp);

	file_content.calibration.acclzx = temp[ACC_INDEX].data.accelMilliG.accelX;
	file_content.calibration.acclzy = temp[ACC_INDEX].data.accelMilliG.accelY;
	file_content.calibration.acclzz = temp[ACC_INDEX].data.accelMilliG.accelZ;
	printAccData(1,&temp[ACC_INDEX],1,1,ACC_INDEX);

	file_content.calibration.gyrox = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroX;
	file_content.calibration.gyroy = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroY;
	file_content.calibration.gyroz = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroZ;
	printGyroData(1,&temp[GYR_INDEX],1,1,GYR_INDEX);

	file_content.calibration.magnxnx = temp[MAG_INDEX].data.magFieldMilliGauss.magFieldX;
	file_content.calibration.magnxny = temp[MAG_INDEX].data.magFieldMilliGauss.magFieldY;
	file_content.calibration.magnxnz = temp[MAG_INDEX].data.magFieldMilliGauss.magFieldZ;
	printMagnData(1,&temp[MAG_INDEX],1,1,MAG_INDEX);


	ALOGD("The first step is completed\n");

	return 0;

}

int SetAGM_STEP_B()
{
	int i;
	int ret;
	//==============================================
	//step 2:

	ALOGD("[2/5]Rotate the screen 180 degrees clockwise, South, west, screen up \n");
	//ALOGD("[2/5]Rotate the screen 180 degrees clockwise so the Windows button is pointing due North\n");

	usleep(1000000);

	memset(calc_tool, 0, sizeof(calc_tool));
	//read the data...
	collect_static_magn_data_avg(2, DATA_SIZE);

	get_single_snr_avg(calc_tool, temp, MAG_INDEX);

	file_content.calibration.magnxsx = temp[MAG_INDEX].data.magFieldMilliGauss.magFieldX;
	file_content.calibration.magnxsy = temp[MAG_INDEX].data.magFieldMilliGauss.magFieldY;

	ALOGD("The 2nd step is completed\n");
	return 0;
}

int SetAGM_STEP_C()
{
	int i;
	int ret;


	//==============================================
	//step 3:
	ALOGD("[3/5]Lay the device flat, north, east, screen down\n");
	//ALOGD("[3/5]Now Lay the device flat, screen down with the windows button pointing due South\n");
	usleep(1000000);

	memset(calc_tool, 0, sizeof(calc_tool));
	//read the data...
	ret = collect_static_magn_data_avg(3,DATA_SIZE);
	if (0 != ret)
		return -1;

	get_single_snr_avg(calc_tool, temp, MAG_INDEX);

	file_content.calibration.magnxsz = temp[2].data.magFieldMilliGauss.magFieldZ;


	ALOGD("The 3rd step is completed\n");

	return 0;
}


int SetAGM_STEP_D()
{
	int i, ret;
	point = 0;


	//==============================================
	//step 4:
	ALOGD("[4/5]Lay the device flat with the screen up, rotate the device counter clockwise\n");
	usleep(1000000);

	memset(calc_tool, 0, sizeof(calc_tool));
	//read the data...
	ret = collect_rota_data_avg(4,DATA_SIZE);
	if (0 != ret)
		return -1;

	get_amg_snr_avg(calc_tool, temp);

	file_content.calibration.gyrozx = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroX;
	file_content.calibration.gyrozy = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroY;
	file_content.calibration.gyrozz = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroZ;



	ALOGD("The 4th step is completed\n");

	return 0;
}

int SetAGM_STEP_E()
{
	int i,ret;
	int ret_acc = 0;
	int ret_gyr = 0;
	int ret_mag = 0;
	point = 0;

	//==============================================
	//step 5:
	ALOGD("[5/5]Hold the device vertical, rotate the device counter clockwise along the axis between bottom and top\n");
	//ALOGD("[5/5]Hold the device vertical with the windows button on the bottom, rotate the device counter clockwise
	//along the axis between the top of the screen and the windows button\n");
	usleep(1000000);

	memset(calc_tool, 0, sizeof(calc_tool));

	//read the data...
	ret = collect_rota_data_avg(5,DATA_SIZE);
	if (0 != ret)
		return -1;

	get_amg_snr_avg(calc_tool, temp);

	file_content.calibration.gyroyx = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroX;
	file_content.calibration.gyroyy = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroY;
	file_content.calibration.gyroyz = temp[GYR_INDEX].data.gyroMilliDegreesPerSecond.gyroZ;

	file_content.calibration.acclyx = temp[ACC_INDEX].data.accelMilliG.accelX;
	file_content.calibration.acclyy = temp[ACC_INDEX].data.accelMilliG.accelY;
	file_content.calibration.acclyz = temp[ACC_INDEX].data.accelMilliG.accelZ;


	ALOGD("The 5th step is completed\n");

	Calibrated = 1;
	file_content.config.calibrated = 1;

	return 0;
}

int SetAGM_STEP_F()
{

	if (0 != WriteDataToFile(AGM_SENSOR_BIN_FILE_NAME, &file_content, sizeof(file_content))) {
		ALOGD("WriteDataToFile failed\n");
		return -1;
	}

	if (0 != WriteDataToFileInTxt()) {
		ALOGD("WriteDataToFileInTxt failed\n");
		return -1;
	}

	ALOGD("Save the files successfully.\n");
	ALOGD("The bin file path: "AGM_SENSOR_BIN_FILE_NAME"\n");
	ALOGD("The txt file path: "AGM_SENSOR_TXT_FILE_NAME"\n");

	return 0;
}


#if DEBUG_USE_ADB
int main(int argc, const char *argv[])
{
	if (argc < 2) {
		reqSnr_g[0] = "Acc_raw";
		reqSnr_g[1] = "Gyro_raw";
		reqSnr_g[2] = "Comps_raw";
		print_acc_log = 0;
		print_gyr_log = 0;
		print_mag_log = 0;
		calc_gyr_integral = 0;

		SetAGM_STEP_A();

		if(1 == apk_exit)
			return -1;
		SetAGM_STEP_B();

		if(1 == apk_exit)
			return -1;
		SetAGM_STEP_C();

		if(1 == apk_exit)
			return -1;
		SetAGM_STEP_D();

		if(1 == apk_exit)
			return -1;
		SetAGM_STEP_E();

		if(1 == apk_exit)
			return -1;
		SetAGM_STEP_F();

		return 0;
	} else {
		int ret;
		pthread_t tid;
		int status;

		int print_gyr_log = 1;
		int i;
		for (optind, i = 0; optind < argc && i < 3; ++optind, i++) {
			ALOGD("optind=%d\n",optind);
			reqSnr_g[i] = (char*)argv[optind];
			//sensor_name_list.push_back(argv[optind]);
		}
		for (i; i < 3; i++) {
			reqSnr_g[i] = NULL;
		}

		memset(calc_tool, 0,sizeof(calc_tool));
		memset(temp, 0,sizeof(temp));

		ret = pthread_create(&tid,NULL,sensorAGM_read_data_loop,(void*)0);
		if(ret) {
			ALOGD("create pthread error!ret = %d\n",ret);
			return -1;
		}

		pthread_join(tid, (void **)&status);
		return 0;
	}

}
#endif
