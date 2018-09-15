#include "testitem.h"
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include <cutils/properties.h>

static void *sprd_handle_camera_dl;

int flashlightSetValue(int value)
{
    int ret = 0;
    char cmd[200] = " ";

    sprintf(cmd, "echo 0x%02x > /sys/class/flash_test/flash_test/flash_value", value);
    ret = system(cmd) ? -1 : 0;
    LOGD("cmd = %s,ret = %d", cmd,ret);

    return ret;
}

int test_bcamera_start(void)
{
	volatile int  rtn = RL_FAIL;
	char lib_full_name[60] = { 0 };
	char prop[PROPERTY_VALUE_MAX] = { 0 };
	int is_show = 0;

	LOGD("enter");

	ui_clear_rows(0,20); 


	property_get("ro.board.platform", prop, NULL);
	sprintf(lib_full_name, "%scamera.%s.so", LIBRARY_PATH, prop);
	LOGD("mmitest %s",lib_full_name);
	sprd_handle_camera_dl = dlopen(lib_full_name,RTLD_NOW);
	if(sprd_handle_camera_dl == NULL){
		LOGD("fail dlopen");
		rtn = RL_FAIL;
		goto go_end;
	}

	
	typedef int (*pf_eng_tst_camera_init)(int32_t camera_id);

	pf_eng_tst_camera_init eng_tst_camera_init = (pf_eng_tst_camera_init)dlsym(sprd_handle_camera_dl,"eng_tst_camera_init" );

	if(eng_tst_camera_init){
		if(eng_tst_camera_init(0)){   //init back camera and start preview
			LOGE(" fail to call eng_test_camera_init");
		}
	}else{
		LOGE("fail to find eng_test_camera_init()");
		rtn = RL_FAIL;
		goto go_end;
	}

	flashlightSetValue(17);

	rtn = ui_handle_button(TEXT_PASS,NULL,TEXT_FAIL);//, TEXT_GOBACK

	flashlightSetValue(16);

	typedef void (*pf_eng_tst_camera_deinit)(void);
	pf_eng_tst_camera_deinit eng_tst_camera_deinit = (pf_eng_tst_camera_deinit)dlsym(sprd_handle_camera_dl,"eng_tst_camera_deinit" );
	if(eng_tst_camera_deinit){
		eng_tst_camera_deinit();   //init back camera and start preview
	}else{
		LOGE("fail to find eng_test_camera_close");
	}

go_end:
  
	save_result(CASE_TEST_BCAMERA,rtn);
	save_result(CASE_TEST_FLASH,rtn);
	return rtn;
}
