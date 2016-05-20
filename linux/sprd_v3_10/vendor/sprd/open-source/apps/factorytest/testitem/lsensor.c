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

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s %d", TEXT_LS_LUX, light_value);

	if(light_pass == 1) {
		ui_set_color(CL_GREEN);
	} else {
		ui_set_color(CL_RED);
	}
	ui_show_text(row, 0, buf);
	gr_flip();
}

static void *lsensor_thread(void *param)
{
	int fd = -1;
	fd_set rfds;
	time_t start_time,now_time;
	struct input_event ev;
	struct timeval timeout;
	int ret;
	int count=0;
	int err;

	begin_time=time(NULL);
	LOGD("mmitest lsensor=%s",SPRD_PLS_INPUT_DEV);
	fd = find_input_dev(O_RDONLY, SPRD_PLS_INPUT_DEV);
	if(fd < 0) {
		ui_push_result(RL_FAIL);
		ui_set_color(CL_RED);
		ui_show_text(cur_row+2, 0, TEXT_OPEN_DEV_FAIL);
		gr_flip();
		sleep(1);
		return NULL;
	}
	err = system("/system/bin/ftmsensor &");
	if(err<0){
		LOGE("mmitest /system/bin/ftmsensor & failed");
		return NULL;
	}

	while(thread_run == 1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		ret = select(fd+1, &rfds, NULL, NULL, &timeout);
		if(FD_ISSET(fd, &rfds)) {
			ret = read(fd, &ev, sizeof(ev));
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

		if((light_pass == 1 && proximity_modifies > 1)) //||(now_time-start_time)>LSENSOR_TIMEOUT
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
	cur_row = ui_show_text(cur_row, 0, BOARD_HAVE_ACC);
	cur_row++;
	cur_row = ui_show_text(cur_row, 0, TEXT_ACC_OPER);
	lsensor_show();
	thread_run = 1;
	pthread_create(&thread, NULL, (void*)lsensor_thread, NULL);
	ret = ui_handle_button(NULL,NULL,NULL);//, TEXT_GOBACK
	thread_run = 0;
	pthread_join(thread, NULL); /* wait "handle key" thread exit. */
	save_result(CASE_TEST_LPSOR,ret);
	over_time=time(NULL);
	LOGD("mmitest casetime lsensor is %d s",(over_time-begin_time));
	return ret;
}
