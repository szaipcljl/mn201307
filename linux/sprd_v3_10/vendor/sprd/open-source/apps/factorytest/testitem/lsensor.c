#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <cutils/log.h>

#include <hardware/sensors.h>
#include <utils/Timers.h>
#include "testitem.h"

static int thread_run;
static int proximity_value=1;
static int proximity_modifies=0;
static int light_value=0;
static int light_pass=0;
static int cur_row = 2;
time_t begin_time,over_time;
struct sensors_poll_device_t *device;
struct sensors_module_t *module;
struct sensor_t const *list;
int count = 0;
int type_num;

#define S_ON	1
#define S_OFF	0

#if defined(BOARD_HAVE_PLS_TP) //160510 add
#define DEVICE_HWINFO_PATH "/sys/board_properties/tp_information"

typedef struct {
	const char* vendor;
	const char* dev_name;
}ctp_info;

const ctp_info ctp_dev_info[] = {
	{"ICN85xx", "ctp_dev"}, //160701 add
	{"FT6336GU", "focaltech_ts"},//160330 add
	{"FT6x36", "focaltech_ts"},
	{"FT6x06", "focaltech_ts"},
	{"FT5206", "focaltech_ts"},
	{"MSG2133A", "msg2133a_ts"},
	{"MSG22XX", "msg2xxx_ts"},//150924 add
	{"GT9147", "goodix_ts"},
	{"GT9157", "goodix_ts"},
	{"himax 852Xes", "himax-touchscreen"},//150924 add
	{NULL,NULL},	//dont remove!
};

const char* GetTpInputName(void)
{
	int fd = -1;
	char tp[256];
	int ret=-1;
	int idx=0;

	memset((void*)tp,0,256);
	fd = open(DEVICE_HWINFO_PATH,O_RDONLY);
	if(fd < 0){
		ALOGE("open tp hw_info failed!!");
		return 0;
	}
	ret = read(fd,tp,sizeof(tp));
	close(fd);
	if(ret < 0){
		ALOGE("Get TP HWinfo failed!!");
		return 0;
	}

	while(ctp_dev_info[idx].vendor != NULL){
		if(strstr(tp,ctp_dev_info[idx].vendor)!=NULL)	{
			ALOGE("TP vendor is %s\n",ctp_dev_info[idx].vendor);
			return ctp_dev_info[idx].dev_name;
		}
		idx++;
	}
	return ctp_dev_info[idx].dev_name;
}
#endif //160510 add-e

static void lsensor_show()
{
	char buf[64];
	int row = cur_row;

	ui_clear_rows(row, 2);
	if(proximity_modifies >= 2) {
		ui_set_color(CL_GREEN);
	} else {
		ui_set_color(CL_RED);
	}

	if(proximity_value == 0){
		row = ui_show_text(row, 0, TEXT_PS_NEAR);
	} else {
		row = ui_show_text(row, 0, TEXT_PS_FAR);
	}

#ifndef BOARD_HAVE_PLS_TP  //150723 add
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s %d", TEXT_LS_LUX, light_value);

	if(light_pass == 1) {
		ui_set_color(CL_GREEN);
	} else {
		ui_set_color(CL_RED);
	}
	ui_show_text(row, 0, buf);
#endif
	gr_flip();
}

static int lsensor_enable(int enable)
{
	int fd;
	int ret = -1;

#ifndef BOARD_HAVE_PLS_TP  //150723 add
	fd = open(SPRD_LS_CTL, O_RDWR);
	if(fd < 0) {
		LOGD("[%s]:open %s fail\n", __FUNCTION__, SPRD_LS_CTL);
		return -1;
	}

	if(fd > 0) {
		if(ioctl(fd, LTR_IOCTL_SET_LFLAG, &enable) < 0) {
			LOGD("[%s]:set lflag %d fail, err:%s\n", __FUNCTION__, enable, strerror(errno));
			ret = -1;
		}
#ifndef BOARD_HAVE_PLS_ELAN
		if(ioctl(fd, LTR_IOCTL_SET_PFLAG, &enable) < 0) {
			LOGD("[%s]:set pflag %d fail, err:%s\n", __FUNCTION__, enable, strerror(errno));
			ret = -1;
		}
#endif
		close(fd);
	}
#endif
	return ret;
}

#if defined(BOARD_HAVE_PLS_TP) || defined(BOARD_HAVE_PLS_ELAN)
static int psensor_enable(int enable)
{
	int fd;
	int ret = -1;
#ifdef BOARD_HAVE_PLS_TP  //150723 add
	char buffer[8];

	fd = open(SPRD_PS_CTL, O_RDWR);

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%d", enable);

	LOGD("%s: enable=%s",__FUNCTION__, buffer);

	if(fd > 0) {	
		write(fd, buffer, strlen(buffer));
		close(fd);
	}
#else
	fd = open(SPRD_PS_CTL, O_RDWR);
	if(fd < 0) {
		LOGD("[%s]:open %s fail\n", __FUNCTION__, SPRD_PS_CTL);
		return -1;
	}

	if(fd > 0) {
		if(ioctl(fd, LTR_IOCTL_SET_PFLAG, &enable) < 0) {
			LOGD("[%s]:set pflag %d fail, err:%s\n", __FUNCTION__, enable, strerror(errno));
			ret = -1;
		}
		close(fd);
	}
#endif
	return ret;
}
#endif
static void *lsensor_thread()
{
	int fd_ls = -1;
	int fd_ps = -1;
	fd_set rfds;
	time_t start_time,now_time;
	struct input_event ev;
	struct timeval timeout;
	int ret;
	int count=0;
#if defined(BOARD_HAVE_PLS_TP) //160510 add
	char * ctp_dev_name=NULL;
#endif

#ifndef BOARD_HAVE_PLS_TP  //150723 add
	LOGD("mmitest huyougang  lsensor=%s\n",SPRD_LS_INPUT_DEV);
	fd_ls = find_input_dev(O_RDONLY, SPRD_LS_INPUT_DEV);

	if(fd_ls < 0) {
		ui_push_result(RL_FAIL);
		ui_set_color(CL_RED);
		ui_show_text(cur_row+2, 0, TEXT_OPEN_DEV_FAIL);
		gr_flip();
		sleep(1);
		return NULL;
	}
	lsensor_enable(1);
#endif

#if defined(BOARD_HAVE_PLS_TP) //160510 add
	ctp_dev_name=(const char*)GetTpInputName();
	LOGD("mmitest huyougang  ctp_dev_name=%s\n",ctp_dev_name);
	fd_ps = find_input_dev(O_RDONLY, ctp_dev_name);
#else
	LOGD("mmitest huyougang  psensor=%s\n",SPRD_PS_INPUT_DEV);
	fd_ps = find_input_dev(O_RDONLY, SPRD_PS_INPUT_DEV);
#endif

	if(fd_ps < 0) {
		ui_set_color(CL_RED);
		ui_show_text(cur_row, 0, TEXT_OPEN_DEV_FAIL);
		gr_flip();
		return NULL;
	}
#if defined(BOARD_HAVE_PLS_TP) || defined(BOARD_HAVE_PLS_ELAN)
	psensor_enable(1);
#endif
	//start_time=time(NULL);
	while(thread_run == 1) {
#ifndef BOARD_HAVE_PLS_TP  //150723 add	
		FD_ZERO(&rfds);
		FD_SET(fd_ls, &rfds);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		ret = select(fd_ls+1, &rfds, NULL, NULL, &timeout);
		if(FD_ISSET(fd_ls, &rfds)) {
			ret = read(fd_ls, &ev, sizeof(ev));
			if(ret == sizeof(ev)){
				if(ev.type == EV_ABS) {
					switch(ev.code){
						case ABS_DISTANCE:
							proximity_modifies++;
							proximity_value = ev.value;
							//LOGD("mmitest lsensor P:%d", ev.value);
							lsensor_show();
							break;
						case ABS_MISC:
							//LOGD("L:%d", ev.value);
							if(light_value!=ev.value)
								count++;
							if(count>=2)
								light_pass = 1;
							light_value = ev.value;
							lsensor_show();
							break;
					}
				}
			}
		}
#endif

		//#if defined(BOARD_HAVE_PLS_TP) || defined(BOARD_HAVE_PLS_ELAN)
		FD_ZERO(&rfds);
		FD_SET(fd_ps, &rfds);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		ret = select(fd_ps+1, &rfds, NULL, NULL, &timeout);
		if(FD_ISSET(fd_ps, &rfds)) {
			ret = read(fd_ps, &ev, sizeof(ev));
			if(ret == sizeof(ev)){
				if(ev.type == EV_ABS) {
					switch(ev.code){
					case ABS_DISTANCE:
						proximity_modifies++;
						proximity_value = ev.value;
						LOGD("P:%d\n", ev.value);
						lsensor_show();
						break;
					case ABS_MISC:
						LOGD("L:%d\n", ev.value);
						if(light_value!=ev.value)
							count++;
						if(count>=2)
							light_pass = 1;
						light_value = ev.value;
						lsensor_show();
						break;
					}
				}
			}
		}
		//#endif

#ifndef BOARD_HAVE_PLS_TP  //150723 add
		if((light_pass == 1 && proximity_modifies > 1)) //||(now_time-start_time)>LSENSOR_TIMEOUT
#else
			if(proximity_modifies > 1) //||(now_time-start_time)>LSENSOR_TIMEOUT
#endif
		{
			ui_push_result(RL_PASS);
			ui_set_color(CL_GREEN);
			ui_show_text(cur_row+2, 0, TEXT_TEST_PASS);
			gr_flip();
			sleep(1);
			goto func_end;
		}
	}
func_end:
	lsensor_enable(0);
#if defined(BOARD_HAVE_PLS_TP) || defined(BOARD_HAVE_PLS_ELAN)
	psensor_enable(0);
#endif
	return NULL;
}

int test_lsensor_start(void)
{
	int ret = 0;
	pthread_t thread;
	proximity_value=1;
	proximity_modifies=0;
	light_value=0;
	light_pass=0;
	cur_row = 2;

	ui_fill_locked();
	ui_show_title(MENU_TEST_LSENSOR);
	ui_set_color(CL_WHITE);
	cur_row = ui_show_text(cur_row, 0, TEXT_SENSOR_DEV_INFO);
	cur_row = ui_show_text(cur_row, 0, BOARD_HAVE_PLS);
	cur_row++;
	cur_row = ui_show_text(cur_row, 0, TEXT_ACC_OPER);
	lsensor_show();
	thread_run = 1;
	pthread_create(&thread, NULL, (void*)lsensor_thread, NULL);
	ret = ui_handle_button(NULL,NULL,NULL);//, TEXT_GOBACK
	thread_run = 0;
	pthread_join(thread, NULL); /* wait "handle key" thread exit. */
	save_result(CASE_TEST_LSENSOR,ret);
	over_time=time(NULL);
	LOGD("mmitest casetime lsensor is %ld s",(over_time-begin_time));
	return ret;
}
