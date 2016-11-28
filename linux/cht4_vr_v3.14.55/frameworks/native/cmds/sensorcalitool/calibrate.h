#include "circ_buf.h"
typedef struct _SENSOR_DATA {
	union {
		/*! \struct accelMilliG
		 *  \brief Accelerometer sensor data.
		 * */
		struct {
			int accelX; /*!< \brief X axis data */
			int accelY; /*!< \brief Y axis data */
			int accelZ; /*!< \brief Z axis data */
		} accelMilliG;

		struct {
			int gyroX; /*!< \brief X axis data */
			int gyroY; /*!< \brief Y axis data */
			int gyroZ; /*!< \brief Z axis data */
		} gyroMilliDegreesPerSecond;

		struct {
			int magFieldX; /*!< \brief X axis data */
			int magFieldY; /*!< \brief Y axis data */
			int magFieldZ; /*!< \brief Z axis data */
		}magFieldMilliGauss;
		struct{
			unsigned int illuminance;
		} alsLux;
		struct {
			int x;
			int y;
			int z;
			int n;
		} data;
	}data;
	struct _SNR_TIME {
		long long int acc_time;
		long long int gyr_time;
		long long int mag_time;
	}snr_time;
}SENSOR_DATA_T;


typedef struct _SENSOR_CALC_TOOL {
	struct {
		long long int sum_x;
		long long int sum_y;
		long long int sum_z;
	} snr_sum;

	struct {
		long long int avg_x;
		long long int avg_y;
		long long int avg_z;
	} snr_avg;

#ifdef DEBUG_USE_ADB
	struct {
		float agl_x;
		float agl_y;
		float agl_z;
		long long int starttime;
		long long int stoptime;
		long long int delta_T;
		long long int time_step;
	} gyro_angle;
#endif
} SENSOR_CALC_TOOL;

typedef struct _FUSION_CONFIG
{
	int calibrated;
	int spen;
	int shake_th;
	int shake_shock;
	int shake_quiet;
	int accl_tolarence;
	int gyro_tolarence;
	int magn_tolarence;
	int stableMS;
} FUSION_CONFIG, *PFUSION_CONFIG;

typedef struct _FUSION_CALIBRATION
{
	int magnx;
	int magny;
	int magnz;

	int magnxnx;
	int magnxny;
	int magnxnz;

	int magnyux;
	int magnyuy;
	int magnyuz;

	int magnxsx;
	int magnxsy;
	int magnxsz;

	int magnydx;
	int magnydy;
	int magnydz;

	int magnnx;
	int magnny;
	int magnnz;

	int magnsx;
	int magnsy;
	int magnsz;


	int acclx;
	int accly;
	int acclz;
	int acclzx;//horizontal
	int acclzy;
	int acclzz;
	int acclyx;//vertical
	int acclyy;
	int acclyz;

	int gyrox;
	int gyroy;
	int gyroz;
	int gyrozx;//horizontal
	int gyrozy;
	int gyrozz;
	int gyroyx;//vertical
	int gyroyy;
	int gyroyz;

	int alscurve[20];
	int als_multiplier;
} FUSION_CALIBRATION, *PFUSION_CALIBRATION;

typedef struct _SENSOR_CALIBRATION
{
	FUSION_CONFIG config;
	FUSION_CALIBRATION calibration;
} SENSOR_CALIBRATION, *PSENSOR_CALIBRATION;
