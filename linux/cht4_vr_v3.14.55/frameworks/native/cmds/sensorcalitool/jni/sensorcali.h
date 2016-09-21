#define AGM_SENSOR_FILE_NAME	"./sensor_calibration_AGM.bin"

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
	float magnx;
	float magny;
	float magnz;
	float magnxnx;
	float magnxny;
	float magnxnz;
	float magnyux;
	float magnyuy;
	float magnyuz;
	float magnxsx;
	float magnxsy;
	float magnxsz;
	float magnydx;
	float magnydy;
	float magnydz;
	float magnnx;
	float magnny;
	float magnnz;
	float magnsx;
	float magnsy;
	float magnsz;
	float acclx;
	float accly;
	float acclz;
	float acclzx;//horizontal
	float acclzy;
	float acclzz;
	float acclyx;//vertical
	float acclyy;
	float acclyz;
	float gyrox;
	float gyroy;
	float gyroz;
	float gyrozx;//horizontal
	float gyrozy;
	float gyrozz;
	float gyroyx;//vertical
	float gyroyy;
	float gyroyz;
	float alscurve[20];
	float als_multiplier;
} FUSION_CALIBRATION, *PFUSION_CALIBRATION;

typedef struct _SENSOR_CALIBRATION
{
	FUSION_CONFIG config;
	FUSION_CALIBRATION calibration;
} SENSOR_CALIBRATION, *PSENSOR_CALIBRATION;

