#ifndef _cmr_dev_info_h
#define _cmr_dev_info_h

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x, args...) printf(x, ##args)
#else
#define DEBUG_PRINT(x, args...)
#endif

#define BOARD_SXQC1801
#ifdef BOARD_SXQC1801
enum fg_ind_gpio_inx {
	GPIO_CUR_IDX,
	GPIO_VOL_IDX,
	GPIO_NUM_MAX,
};
#define MAX_FD_IND_PINS GPIO_NUM_MAX
#define CAM_PWR_NFLAG 3 /*FLAG gose low when overcurrent ...*/
#define CAM_PWR_NUVOV 4 /*UVOV goes low when input voltage is higher or lower than normal*/

#define ADC_CHAN 1
#define ADC_SYSFS_FILE_RAW "/sys/devices/platform/e8000000.apb/e801d000.adc/iio:device0/in_voltage%d_raw"
#define ADC_SYSFS_FILE_SCALE "/sys/devices/platform/e8000000.apb/e801d000.adc/iio:device0/in_voltage%d_scale"
#endif

#define STAT_NOMAL 1
#define STAT_ERR 0


/* load switch API*/
struct load_switch_dev {
	char chip_name[16];
	int ft_ind_pins[MAX_FD_IND_PINS]; /* FLAG, UVOV*/
	int adc_chan;
};

//internal
static inline int read_sysfs_file(const char* file, void *buf, size_t count)
{
	int fd;

	if ((fd = open((const char*)file, O_RDONLY, 0)) < 0) {
		perror((const char*)file);
		close(fd);
		return -1;
	}

	if (read(fd, buf, count) < 0) {
		perror("read failed");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

static inline int get_gpio_state(struct load_switch_dev *ld_sw_dev, int gpio_idx)
{
	char state_buf[4];
	int state = 0;
	char val_file[30] = {0};
	int gpio = ld_sw_dev->ft_ind_pins[gpio_idx];
	memset(state_buf, 0, 4);

	snprintf(val_file, 30, "/sys/class/gpio/gpio%d/value", gpio);
	if (read_sysfs_file(val_file, state_buf, 4) < 0)
		return -1;

	state_buf[3] = '\0';
	state = atoi(state_buf);
	printf("gpio%d: %d\n", ld_sw_dev->ft_ind_pins[gpio_idx], state);
	return state;
}

/*
 * echo 3 > /sys/class/gpio/export
 * echo in > /sys/class/gpio/gpio3/direction
 * */
static inline int write_sysfs_file_once(char *file_name, void *buf, size_t count)
{
	int fd;
	// open the sysfs file
	if ((fd = open((const char*)file_name, O_WRONLY, 0)) < 0) {
		perror((const char*)file_name);
		return -1;
	}
	if (write(fd, buf, count) < 0) {
		perror("write failed");
		return -1;
	}
	close(fd);

	return 0;
}

static inline int gpio_init(int gpio)
{
	char gpio_name[4] = {0};
	char *exp_file = "/sys/class/gpio/export";
	char dir_file[36] = {0};
	char *dir = "in";

	snprintf(dir_file, 36, "/sys/class/gpio/gpio%d/direction", gpio);
	snprintf(gpio_name, 4, "%d", gpio);

	if (access(dir_file, F_OK)) {
		if (write_sysfs_file_once(exp_file, gpio_name, 4) < 0) {
			return -1;
		}

		if (write_sysfs_file_once(dir_file, dir, 3) < 0) {
			return -1;
		}
	} else {
		printf("gpio%d has been exported: file existence\n", gpio);
	}

	return 0;
}

static inline int gpio_exit(int gpio)
{

	return 0;
}

/* return adc raw*/
static inline int get_current_adc_val(int adc_chan)
{
	char cur_adc_file[80] = {0};
	char raw_buf[5] = {0};
	char scale_buf[16] = {0};
	double raw;
	double scale;
	double real_vol;

	snprintf(cur_adc_file, 80, ADC_SYSFS_FILE_RAW , adc_chan);
	if (read_sysfs_file(cur_adc_file, raw_buf, 5) < 0)
		return -1;
	raw_buf[4] = '\0';
	raw = atof(raw_buf);
	printf("adc_chan(%d) raw:%s\n", adc_chan, raw_buf);

	snprintf(cur_adc_file, 80, ADC_SYSFS_FILE_SCALE , adc_chan);
	if (read_sysfs_file(cur_adc_file, scale_buf, 16) < 0)
		return -1;
	scale_buf[15] = '\0';
	scale = atof(scale_buf);

	real_vol = raw*scale;
	printf("adc_chan(%d) scale:%s, cur_val:%f\n", adc_chan, scale_buf, real_vol);

	//TODO: voltage -> current

	return raw;
}

int ld_sw_adc_chan_init(struct load_switch_dev *ld_sw_dev);
int ld_sw_get_current_state(struct load_switch_dev *ld_sw_dev);
int ld_sw_get_voltage_state(struct load_switch_dev *ld_sw_dev);
bool ld_sw_is_current_normal(struct load_switch_dev *ld_sw_dev);
bool ld_sw_is_voltage_normal(struct load_switch_dev *ld_sw_dev);
int ld_sw_get_current_val(struct load_switch_dev* ld_sw_dev);
int ld_sw_gpio_init(struct load_switch_dev* ld_sw_dev);

/* camera device info API*/
struct cmr_dev_info { /* sensor info*/
	char chip_name[16];
	int i2c_addr;
	int iav_fd;
	int snr_fd;
	int temp_reg;
	struct load_switch_dev ld_sw_dev;
	int initialized;
};

int snr_open(int addr);

static inline int video_dev_open(char *dev)
{
	int fd_iav;
	// open the device
	if ((fd_iav = open(dev, O_RDWR, 0)) < 0) {
		perror(dev);
		return -1;
	}
	DEBUG_PRINT("#mn:[%s] open iav Succeed\n", __func__);
	return fd_iav;
}

int snr_get_temp(struct cmr_dev_info *cmr_dev);

static inline int video_dev_close(int fd)
{
	// close the device
	close(fd);
	return 0;
}


#endif
