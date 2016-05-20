#include "testitem.h"
#include <dlfcn.h>
#include <cutils/properties.h>


static void *sprd_handle_camera_dl;
static int eng_front_camera =0;

int test_fcamera_start(void)
{
	int rtn = RL_NA;
	int is_show = 0;
	char lib_full_name[60] = { 0 };
	char prop[PROPERTY_VALUE_MAX] = { 0 };
	ui_clear_rows(0,20);

	LOGD("enter");
	LOGD("mmitest before open lib");
	property_get("ro.board.platform", prop, NULL);
	sprintf(lib_full_name, "%scamera.%s.so", LIBRARY_PATH, prop);
	LOGD("mmitest %s",lib_full_name);
	sprd_handle_camera_dl = dlopen(lib_full_name,RTLD_NOW);
	if(sprd_handle_camera_dl == NULL){
		LOGE("mmitest %s fail dlopen");
		rtn = RL_FAIL;
		goto go_exit;
	}

	LOGD("mmitest after open lib");
	typedef int (*pf_eng_tst_camera_init)(int32_t camera_id);
	pf_eng_tst_camera_init eng_tst_camera_init = (pf_eng_tst_camera_init)dlsym(sprd_handle_camera_dl,"eng_tst_camera_init" );

	if(eng_tst_camera_init){
		if(eng_tst_camera_init(1)){   //init front camera and start preview
			LOGE(" fail to call eng_test_camera_init");
		}
	}else{
		LOGE("fail to find eng_test_camera_init()");
		rtn = RL_FAIL;
		goto go_exit;
	}

	LOGD("mmitest start preview with front camera");
	//eng_draw_handle_softkey(ENG_ITEM_FCAMERA);
	rtn = ui_handle_button(TEXT_PASS,NULL,TEXT_FAIL);//, TEXT_GOBACK

	typedef void (*pf_eng_tst_camera_deinit)(void);
	pf_eng_tst_camera_deinit eng_tst_camera_deinit = (pf_eng_tst_camera_deinit)dlsym(sprd_handle_camera_dl,"eng_tst_camera_deinit" );
	if(eng_tst_camera_deinit){
		eng_tst_camera_deinit();   //init back camera and start preview
	}

go_exit:
	save_result(CASE_TEST_FCAMERA,rtn);
	return rtn;
}
