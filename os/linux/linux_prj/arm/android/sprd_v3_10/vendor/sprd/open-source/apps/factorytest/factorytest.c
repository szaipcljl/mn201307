#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include "minui.h"
#include "common.h"

#include "resource.h"
#include "testitem.h"
#include <cutils/android_reboot.h>

typedef struct {
	unsigned char num;
	char* title;
	int (*func)(void);
}menu_info;

typedef struct {
	char* result;
	char* title;
	int (*func)(void);
}menu_result_info;

static int auto_all_test(void);
static int PCBA_auto_all_test(void);
static int show_phone_test_menu(void);
static int show_pcba_test_menu(void);
static int show_suggestion_test_menu(void);
static int show_phone_info_menu(void);

static int show_phone_test_result(void);
static int show_pcba_test_result(void);
static int show_suggestion_test_result(void);
static int phone_shutdown(void);
static int phone_reboot(void);

static int test_result_mkdir(void);
static void write_bin(char * pathname );
static void read_bin(void);
extern void temp_set_visible(int is_show);
extern int parse_config();

static pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;

static int at_fd[MAX_MODEM_COUNT];
static int fm_freq=875;

char sdcard_fm_state;
char pcba_phone=0;
char mode_type = 0; //0:w,1:lte

char external_path[MAX_NAME_LEN];
char sdcard_testfile[MAX_NAME_LEN];
char sdcard_fm_testfile[MAX_NAME_LEN];

mmi_result phone_result[TOTAL_NUM];
mmi_result pcba_result[TOTAL_NUM];
hardware_result support_result[TOTAL_NUM];
extern int text_tp_rows,text_rows;
extern unsigned char menu_change;
extern unsigned cheight;

#define AT_BUFFER_SIZE			2048
#define MULTI_TEST_CNT	5

menu_info menu_root[] = {
#define MENUTITLE(num,title, func) \
	[ K_ROOT_##title ] = {num,MENU_##title, func, },
#include "./res/menu_root.h"
#undef MENUTITLE
    [K_MENU_ROOT_CNT]={0,MENU_FACTORY_RESET, phone_shutdown,},
};

menu_info menu_auto_test[] = {
#define MENUTITLE(num,title, func) \
	[ A_PHONE_##title ] = {num,MENU_##title, func, },
#include "./res/menu_auto_test.h"
#undef MENUTITLE
	//[K_MENU_AUTO_TEST_CNT] = {0,0,MENU_BACK, 0,},
};

menu_info menu_phone_test[] = {
#define MENUTITLE(num,title, func) \
	[ K_PHONE_##title ] = {num,MENU_##title, func, },
#include "./res/menu_phone_test.h"
#undef MENUTITLE
	//[K_MENU_PHONE_TEST_CNT] = {0,0,MENU_BACK, 0,},
};


menu_info menu_pcba_test[] = {
#define MENUTITLE(num,title, func) \
	[ P_PHONE_##title ] = {num,MENU_##title, func, },
#include "./res/menu_pcba_test.h"
#undef MENUTITLE
	//[P_MENU_PHONE_TEST_CNT] = {0,0,MENU_BACK, 0,},
};

menu_info menu_not_auto_test[] = {
#define MENUTITLE(num,title, func) \
	[ D_PHONE_##title ] = {num,MENU_##title, func, },
#include "./res/menu_not_autotest.h"
#undef MENUTITLE
	//[K_MENU_AUTO_TEST_CNT] = {0,0,MENU_BACK, 0,},
};

menu_info menu_phone_info_testmenu[] = {
#define MENUTITLE(num,title, func) \
	[ B_PHONE_##title ] = {num,MENU_##title, func, },
#include "./res/menu_phone_info.h"
#undef MENUTITLE
	//[K_MENU_INFO_CNT] = {0,0,MENU_BACK, 0,},
};

menu_result_info menu_phone_result_menu[64] = {0};
menu_result_info menu_pcba_result_menu[64] = {0};
menu_result_info menu_not_suggestion_result_menu[64] = {0};

static mmi_show_data mmi_data_table[] = {
    {CASE_TEST_LCD,               MENU_TEST_LCD,			test_lcd_start,},
    {CASE_TEST_TP,                MENU_TEST_TP,			test_tp_start,},
    {CASE_TEST_MULTITOUCH,        MENU_TEST_MULTI_TOUCH,    		test_multi_touch_start,},
    {CASE_TEST_VIBRATOR,          MENU_TEST_VIBRATOR,	    	 	test_vb_bl_start,},
    {CASE_TEST_BACKLIGHT,         MENU_TEST_BACKLIGHT,	    		test_vb_bl_start,},
#ifdef SUPPORT_TEST_ITEM_LED
    {CASE_TEST_LED,               MENU_TEST_LED,			test_led_start,},
#endif
    {CASE_TEST_KEY,               MENU_TEST_KEY,			test_key_start,},
    {CASE_TEST_FCAMERA,           MENU_TEST_FCAMERA,			test_fcamera_start,},
    {CASE_TEST_BCAMERA,           MENU_TEST_BCAMERA,			test_bcamera_start,},
    {CASE_TEST_FLASH,             MENU_TEST_FLASH, 			test_bcamera_start,},
    {CASE_TEST_MAINLOOP,          MENU_TEST_MAINLOOP,			test_mainloopback_start,},
    //{CASE_TEST_ASSISLOOP,         MENU_TEST_ASSISLOOP,			test_assisloopback_start,},
    {CASE_TEST_RECEIVER,          MENU_TEST_RECEIVER,			test_receiver_start,},
    {CASE_TEST_CHARGE,            MENU_TEST_CHARGE,			test_charge_start,},
    {CASE_TEST_SDCARD,            MENU_TEST_SDCARD,			test_sdcard_start,},
    {CASE_TEST_EMMC,              MENU_TEST_EMMC,			test_emmc_start,},
    {CASE_TEST_SIMCARD,           MENU_TEST_SIMCARD,			test_sim_start,},
    {CASE_TEST_RTC,               MENU_TEST_RTC,			test_rtc_start,},
    {CASE_TEST_HEADSET,           MENU_TEST_HEADSET,			test_headset_start,},
    {CASE_TEST_FM,                MENU_TEST_FM,			test_fm_start,},
    {CASE_TEST_GSENSOR,           MENU_TEST_GSENSOR,			test_gsensor_start,},
    {CASE_TEST_LPSOR,             MENU_TEST_LSENSOR,			test_lsensor_start,NULL},
    {CASE_TEST_BT,                MENU_TEST_BT,			test_bt_start,},
    {CASE_TEST_WIFI,              MENU_TEST_WIFI,			test_wifi_start,},
    {CASE_TEST_GPS,               MENU_TEST_GPS,			test_gps_start,},
    {CASE_TEST_TEL,               MENU_TEST_TEL,			test_tel_start},
    {CASE_TEST_OTG,               MENU_TEST_OTG,			test_otg_start},
};

menu_info multi_test_item[MULTI_TEST_CNT] = {
	{CASE_SDCARD_INDEX,A_PHONE_TEST_SDCARD, test_sdcard_pretest},
	{CASE_SIM_INDEX,A_PHONE_TEST_SIMCARD, test_sim_pretest},
	{CASE_WIFI_INDEX,A_PHONE_TEST_WIFI, test_wifi_pretest},
	{CASE_BT_INDEX,A_PHONE_TEST_BT, test_bt_pretest},
	{CASE_GPS_INDEX,A_PHONE_TEST_GPS, test_gps_pretest}
};

mmi_result* get_result_ptr(char id)
{
    mmi_result*ptr = NULL;

    if(id<TOTAL_NUM){
        if(1 == pcba_phone)
            ptr=&pcba_result[id];
        else
            ptr=&phone_result[id];
    }
    return ptr;
}

void save_result(char id,char key_result)
{
    mmi_result*ptr ;

    ptr = get_result_ptr(id);
    ptr->pass_faild=key_result;

    if(CASE_TEST_TEL == id ||CASE_TEST_OTG == id){
        pcba_phone = 1;
        ptr = get_result_ptr(id);
        ptr->pass_faild=key_result;
    }
}

static void read_bin(void)
{
    int fd_pcba = -1 ,fd_whole = -1;
    int len;

    fd_whole = open(PHONETXTPATH,O_RDWR);
    if (fd_whole < 0){
        LOGE("mmitest open %s failed,err = %s",PHONETXTPATH);
        goto out;
    }

    pthread_mutex_lock(&result_mutex);
    len=read(fd_whole,phone_result,sizeof(phone_result));
    if(len < 0){
        LOGE("read %s failed,len = %d,err = %s",PHONETXTPATH,len);
        goto out;
    }

    fd_pcba = open(PCBATXTPATH,O_RDWR);
    if (fd_pcba < 0 ) {
        LOGE("mmitest open %s failed.",PHONETXTPATH);
        goto out;
    }
    len=read(fd_pcba,pcba_result,sizeof(pcba_result));
    if(len < 0){
        LOGE("read %s failed, len = %d,err = %s",PCBATXTPATH,len);
        goto out;
    }
    pthread_mutex_unlock(&result_mutex);

out:
    if(fd_whole >=  0)
        close(fd_whole);
    if(fd_pcba >= 0)
        close(fd_pcba);
}

static void write_bin(char * pathname)
{
    int len;
    int fd;

    fd = open(pathname,O_RDWR);
    if (fd < 0 ) {
        LOGE("mmitest open %s failed.",pathname);
        goto out;
    }

    pthread_mutex_lock(&result_mutex);
    if(1 == pcba_phone){
        len = write(fd,pcba_result,sizeof(pcba_result));
        if(len < 0){
            LOGE("write %s failed,len = %d",pathname,len);
            goto out;
        }
    }else {
        len = write(fd,phone_result,sizeof(phone_result));
        if(len < 0){
            LOGE("write %s failed,len = %d",pathname,len);
            goto out;
        }
    }
    sync();
    pthread_mutex_unlock(&result_mutex);

    LOGD("%s pcba_phone = %d,len = %d",pathname,pcba_phone,len);

 out:
    if(fd >= 0 )
        close(fd);
    return ;
}

static int show_root_menu(void)
{
	int chosen_item = -1;
	int i = 0,time_consume = 0,total_time = 0;
	const char* items[K_MENU_ROOT_CNT+2];
	int menu_cnt = K_MENU_ROOT_CNT+1;
	menu_info* pmenu = menu_root;
	time_t start_time,end_time;

	temp_set_visible(1);

	for(i = 0; i < menu_cnt; i++) {
		items[i] = pmenu[i].title;
	}
	items[menu_cnt] = NULL;

	while(1) {
		chosen_item = ui_show_menu(MENU_TITLE_ROOT, items, 1, chosen_item,K_MENU_ROOT_CNT+1);
		LOGD("mmitest chosen_item = %d",  chosen_item);
		if(chosen_item >= 0 && chosen_item < menu_cnt) {
			if(pmenu[chosen_item].func != NULL) {
				LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
				start_time = time(NULL);
				pmenu[chosen_item].func();
				end_time = time(NULL);
				time_consume = end_time -start_time;
				total_time += time_consume;
				LOGD("mmitest select menu = <%s> consume time = %d",  pmenu[chosen_item].title,time_consume);
				LOGD("mmitest select menu = <%s> total time consum = %d", MENU_TITLE_ROOT,total_time);
			}
		}
		LOGD("mmitest chosen_item = %d,text_tp_rows=%d", chosen_item,text_tp_rows);
		if(chosen_item < text_tp_rows)
			menu_change=0;
		else menu_change=2;
	}
	return 0;
}

static void show_multi_test_result(void)
{
	int ret = RL_NA;
	int row = 3;
	char tmp[128];
	char* rl_str;
	int i,j;
	menu_info* ptest=multi_test_item;
	menu_info* pmenu = menu_auto_test;
	ui_fill_locked();
	ui_show_title(MENU_MULTI_TEST);
	gr_flip();

	for(i = 0; i < MULTI_TEST_CNT; i++) {
		ret = ptest[i].func();
		if(ret == RL_PASS) {
			ui_set_color(CL_GREEN);
			rl_str = TEXT_PASS;
		} else {
			ui_set_color(CL_RED);
			rl_str = TEXT_FAIL;
		}
		memset(tmp, 0, sizeof(tmp));
		for(j=0;i<CASE_NUM;j++){
			if(ptest[i].num==pmenu[j].num)
				break;
		}
		sprintf(tmp, "%s: %s", (pmenu[j].title+1), rl_str);
		row = ui_show_text(row, 0, tmp);
		gr_flip();
	}

	sleep(1);
}
static int auto_all_test(void)
{
	int i = 0;
	int j = 0;
	int result = 0,time_consume = 0;
	char* rl_str;
	time_t start_time,end_time;

	test_gps_init();
	test_bt_wifi_init();
	menu_info* pmenu = menu_auto_test;
	pcba_phone=0;
	for(i = 0; i < K_MENU_AUTO_TEST_CNT; i++){
		for(j = 0; j < MULTI_TEST_CNT; j++) {
			if(pmenu[i].num== multi_test_item[j].num) {
				LOGD("mmitest break, id=%d", i);
				break;
			}
		}
		if(j < MULTI_TEST_CNT) {
			continue;
		}
		LOGD("mmitest Do, id=%d", i);
		if(pmenu[i].func != NULL) {
			start_time = time(NULL);
			result = pmenu[i].func();
			end_time = time(NULL);
			time_consume = end_time -start_time;
			LOGD("mmitest select menu = <%s> consume time = %d", pmenu[i].title,time_consume);
		}
	}
	gpsStop();
	sleep(1);
	gpsClose();
	show_multi_test_result();
	ui_handle_button(NULL,NULL,TEXT_GOBACK);
	write_bin(PHONETXTPATH);

	return 0;
}


static int PCBA_auto_all_test(void)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int result = 0,time_consume = 0;
	char* rl_str;
	time_t start_time,end_time;

	test_gps_init();
	test_bt_wifi_init();
	menu_info* pmenu = menu_auto_test;
	pcba_phone=1;

	for(i = 1; i < K_MENU_AUTO_TEST_CNT; i++){
		for(j = 0; j < MULTI_TEST_CNT; j++) {
			if(pmenu[i].num == multi_test_item[j].num) {
				LOGD("mmitest break, id=%d", i);
				break;
			}
		}
		if(j < MULTI_TEST_CNT) {
			continue;
		}
		if(pmenu[i].func != NULL) {
			LOGD("mmitest Do id=%d", i);
			start_time = time(NULL);
			result = pmenu[i].func();
			end_time = time(NULL);
			time_consume = end_time -start_time;
			LOGD("mmitest select menu = <%s> consume time = %d", pmenu[i].title,time_consume);
		}

	}
	gpsStop();
	sleep(1);
	gpsClose();
	show_multi_test_result();
	ui_handle_button(NULL,NULL,TEXT_GOBACK);
	write_bin(PCBATXTPATH);
	return 0;
}


static int show_phone_test_menu(void)
{
	int chosen_item = -1;
	int i = 0;
	char* items[K_MENU_PHONE_TEST_CNT+1];
	int menu_cnt = K_MENU_PHONE_TEST_CNT;
	int result = 0,time_consume = 0;;
	time_t start_time,end_time;

	menu_info* pmenu = menu_phone_test;
	pcba_phone=0;

	for(i = 0; i < menu_cnt; i++) {
		items[i] = pmenu[i].title;
	}
	items[menu_cnt] = NULL;

	while(1) {
		LOGD("mmitest back to main");
		chosen_item = ui_show_menu(MENU_TITLE_PHONETEST, items, 0, chosen_item,K_MENU_PHONE_TEST_CNT);
		LOGD("mmitest chosen_item = %d", chosen_item);
		if(chosen_item >= 0 && chosen_item < menu_cnt) {
			LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
			if(chosen_item >= K_MENU_PHONE_TEST_CNT) {
				return 0;
			}
			if(pmenu[chosen_item].func != NULL) {
				start_time = time(NULL);
				result = pmenu[chosen_item].func();
				LOGD("mmitest result=%d", result);
				end_time = time(NULL);
				time_consume = end_time -start_time;
				LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
			}
			write_bin(PHONETXTPATH);
		}else if (chosen_item < 0){
			return 0;
		}
    }
	return 0;
}

static int show_pcba_test_menu(void)
{
	int chosen_item = -1;
	int i = 0;
	char* items[P_MENU_PHONE_TEST_CNT+1];
	int menu_cnt = P_MENU_PHONE_TEST_CNT;
	int result = 0,time_consume = 0;
	time_t start_time,end_time;
	unsigned char txt_flag=1;
	menu_info* pmenu = menu_pcba_test;
	int ret;
	pcba_phone=1;

	for(i = 0; i < menu_cnt; i++) {
		items[i] = pmenu[i].title;
	}
	items[menu_cnt] = NULL;

	while(1) {
		chosen_item = ui_show_menu(MENU_TITLE_PHONETEST, items, 0, chosen_item,P_MENU_PHONE_TEST_CNT);
		LOGD("mmitest chosen_item = %d", chosen_item);
		if(chosen_item >= 0 && chosen_item < menu_cnt) {
			LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
			if(chosen_item >= P_MENU_PHONE_TEST_CNT) {
				return 0;
			}
			if(pmenu[chosen_item].func != NULL) {
				start_time = time(NULL);
				result = pmenu[chosen_item].func();
				LOGD("mmitest result=%d", result);
				end_time = time(NULL);
				time_consume = end_time -start_time;
				LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);

			}
			write_bin(PCBATXTPATH);
		}
		else if (chosen_item < 0){
			return 0;
		}
    }
	return 0;
}

static int show_suggestion_test_menu(void)
{
	int chosen_item = -1;
	int i = 0;
	char* items[K_MENU_NOT_AUTO_TEST_CNT+1];
	int menu_cnt = K_MENU_NOT_AUTO_TEST_CNT;
	int result = 0,time_consume = 0;
	time_t start_time,end_time;
	menu_info* pmenu = menu_not_auto_test;
	pcba_phone=2;

	for(i = 0; i < menu_cnt; i++) {
		items[i] = pmenu[i].title;
	}
	items[menu_cnt] = NULL;

	while(1) {
		LOGD("mmitest back to main");
		chosen_item = ui_show_menu(MENU_NOT_AUTO_TEST, items, 0, chosen_item,K_MENU_NOT_AUTO_TEST_CNT);
		LOGD("mmitest chosen_item = %d",chosen_item);
		if(chosen_item >= 0 && chosen_item < menu_cnt) {
			LOGD("mmitest select menu = <%s>",  pmenu[chosen_item].title);
			if(chosen_item >= K_MENU_NOT_AUTO_TEST_CNT) {
				return 0;
			}
			if(pmenu[chosen_item].func != NULL) {
				start_time = time(NULL);
				result = pmenu[chosen_item].func();
				LOGD("mmitest result=%d", result);
				end_time = time(NULL);
				time_consume = end_time -start_time;
				LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
			}
			write_bin(PHONETXTPATH);
			write_bin(PCBATXTPATH);
		}else if (chosen_item < 0){
			return 0;
		}
	}
	return 0;
}

static int show_phone_info_menu(void)
{
        int chosen_item = -1;
        int i = 0;
        char* items[K_MENU_INFO_CNT+1];
        int menu_cnt = K_MENU_INFO_CNT;
        int result = 0,time_consume = 0;
        time_t start_time,end_time;

        menu_info* pmenu = menu_phone_info_testmenu;
        for(i = 0; i < menu_cnt; i++) {
            items[i] = pmenu[i].title;
        }
        LOGD("mmitest menu_cnt=%d",menu_cnt);
        items[menu_cnt] = NULL;
        while(1) {
            chosen_item = ui_show_menu(MENU_PHONE_INFO, items, 0, chosen_item,K_MENU_INFO_CNT);
            LOGD("mmitest chosen_item = %d", chosen_item);
            if(chosen_item >= 0 && chosen_item < menu_cnt) {
                LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
                if(chosen_item >= K_MENU_INFO_CNT) {
                    return 0;
                }
                if(pmenu[chosen_item].func != NULL) {
                        start_time = time(NULL);
                        result = pmenu[chosen_item].func();
                        end_time = time(NULL);
                        time_consume = end_time -start_time;
                        LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
                }
            }else if (chosen_item < 0){
                    return 0;
            }
        }
        return 0;
}

static int show_phone_test_result(void)
{
	int i = 0;
	int id,num;
	char tmp[64][64];
	mmi_result*ptr;
	int chosen_item = -1;
	char* items[64+1];
	int result = 0,time_consume = 0;
	time_t start_time,end_time;

	pcba_phone=0;
	num = sizeof(mmi_data_table)/sizeof(mmi_data_table[0])-2;
	while(1) {
		for(i = 0; i < num; i++){
                    id = mmi_data_table[i].id;
                    ptr = get_result_ptr(id);
                    memset(tmp[i], 0, sizeof(tmp[i]));
                    switch(ptr->pass_faild) {
				case RL_NA:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i].name+2),TEXT_NA);
					break;
				case RL_FAIL:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i].name+2),TEXT_FAIL);
					break;
				case RL_PASS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i].name+2),TEXT_PASS);
					break;
				case RL_NS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i].name+2),TEXT_NS);
					break;
				default:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i].name+2),TEXT_NA);
					break;
                    }
                    menu_phone_result_menu[i].title=tmp[i];
                    menu_phone_result_menu[i].func= mmi_data_table[i].func;
                    items[i] = menu_phone_result_menu[i].title;
                    LOGD("mmitest <%d>-%s", i, menu_phone_result_menu[i].title);
		}
		menu_result_info* pmenu = menu_phone_result_menu;
		items[i] = NULL;
		chosen_item = ui_show_menu(MENU_PHONE_REPORT, items, 0, chosen_item,num);
		LOGD("mmitest chosen_item = %d", chosen_item);
		if(chosen_item >= 0 && chosen_item < num) {
                    LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
                    if(chosen_item >= num) {
                        return 0;
                    }
                    if(pmenu[chosen_item].func != NULL) {
                        start_time = time(NULL);
                        result = pmenu[chosen_item].func();
                        end_time = time(NULL);
                        time_consume = end_time -start_time;
                        LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
                    }
                    write_bin(PHONETXTPATH);
		}else if (chosen_item < 0){
                    return 0;
		}
	}

      return 0;
}

static int show_pcba_test_result(void)
{
	int i = 0;
	int id,num;
	char tmp[64][64];
	mmi_result*ptr;
	int chosen_item = -1;
	char* items[64+1];
	int result = 0,time_consume = 0;
	time_t start_time,end_time;

	pcba_phone=1;
	num = sizeof(mmi_data_table)/sizeof(mmi_data_table[0])-2;
      while(1) {
		for(i = 0; i < num-1; i++){
                    id = mmi_data_table[i+1].id;
                    ptr = get_result_ptr(id);
                    memset(tmp[i], 0, sizeof(tmp[i]));
                    switch(ptr->pass_faild) {
				case RL_NA:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i+1].name+2),TEXT_NA);
					break;
				case RL_FAIL:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i+1].name+2),TEXT_FAIL);
					break;
				case RL_PASS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i+1].name+2),TEXT_PASS);
					break;
				case RL_NS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i+1].name+2),TEXT_NS);
					break;
				default:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[i+1].name+2),TEXT_NA);
					break;
                    }
                    menu_pcba_result_menu[i].title=tmp[i];
                    menu_pcba_result_menu[i].func= mmi_data_table[i+1].func;
                    items[i] = menu_pcba_result_menu[i].title;
                    LOGD("mmitest <%d>-%s", i, menu_pcba_result_menu[i].title);
		}
		menu_result_info* pmenu = menu_pcba_result_menu;
		items[i] = NULL;
		chosen_item = ui_show_menu(MENU_BOARD_REPORT, items, 0, chosen_item,num-1);
		LOGD("mmitest chosen_item = %d", chosen_item);
		if(chosen_item >= 0 && chosen_item < num) {
                    LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
                    if(chosen_item >= num) {
                        return 0;
                    }
                    if(pmenu[chosen_item].func != NULL) {
                        start_time = time(NULL);
                        result = pmenu[chosen_item].func();
                        end_time = time(NULL);
                        time_consume = end_time -start_time;
                        LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
                    }
                    write_bin(PCBATXTPATH);
		}else if (chosen_item < 0){
                    return 0;
		}
	}

      return 0;
}

static int show_suggestion_test_result(void)
{
	int i = 0;
	int id,num;
	char tmp[64][64];
	mmi_result*ptr;
	int chosen_item = -1;
	char* items[3];
	int result = 0,time_consume = 0;
	time_t start_time,end_time;

	pcba_phone=2;
	num = sizeof(mmi_data_table)/sizeof(mmi_data_table[0]);
	while(1) {
		for(i = 0; i < 2; i++){
                    id = mmi_data_table[num-2+i].id;
                    ptr = get_result_ptr(id);
                    memset(tmp[i], 0, sizeof(tmp[i]));
                    switch(ptr->pass_faild) {
				case RL_NA:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[num-2+i].name+2),TEXT_NA);
					break;
				case RL_FAIL:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[num-2+i].name+2),TEXT_FAIL);
					break;
				case RL_PASS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[num-2+i].name+2),TEXT_PASS);
					break;
				case RL_NS:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[num-2+i].name+2),TEXT_NS);
					break;
				default:
					sprintf(tmp[i],"%s:%s",(mmi_data_table[num-2+i].name+2),TEXT_NA);
					break;
                    }
                    menu_not_suggestion_result_menu[i].title=tmp[i];
                    menu_not_suggestion_result_menu[i].func= mmi_data_table[num-2+i].func;
                    items[i] = menu_not_suggestion_result_menu[i].title;
                    LOGD("mmitest <%d>-%s", i, menu_not_suggestion_result_menu[i].title);
		}
		menu_result_info* pmenu = menu_not_suggestion_result_menu;
		items[i] = NULL;
		chosen_item = ui_show_menu(MENU_NOT_AUTO_REPORT, items, 0, chosen_item,2);
		LOGD("mmitest chosen_item = %d", chosen_item);
		if(chosen_item >= 0 && chosen_item < num) {
                    LOGD("mmitest select menu = <%s>", pmenu[chosen_item].title);
                    if(chosen_item >= num) {
                        return 0;
                    }
                    if(pmenu[chosen_item].func != NULL) {
                        start_time = time(NULL);
                        result = pmenu[chosen_item].func();
                        end_time = time(NULL);
                        time_consume = end_time -start_time;
                        LOGD("mmitest select menu = <%s> consume time = %d", pmenu[chosen_item].title,time_consume);
                    }
                    write_bin(PHONETXTPATH);
                    write_bin(PCBATXTPATH);
		}else if (chosen_item < 0){
                    return 0;
		}
      }

      return 0;
}

static int phone_reboot(void)
{
    LOGD("==== phone_reboot enter ====\n");
    sync();
    android_reboot(ANDROID_RB_RESTART2, 0, "normal");
    return 0;
}
static int phone_shutdown(void)
{
    int fd;
    int ret = -1;
    int time_consume = 0;;
    time_t start_time,end_time;

    start_time = time(NULL);
    ret = mkdir("/cache/recovery/",S_IRWXU | S_IRWXG | S_IRWXO);
    if (-1 == ret && (errno != EEXIST)) {
        LOGE("mkdir /cache/recovery/ failed.");
    }

    fd=open("/cache/recovery/command",O_WRONLY|O_CREAT,0777);
    if (fd >= 0) {
        write(fd,"--wipe_data\n--locale=zh_CN", strlen("--wipe_data\n--locale=zh_CN") + 1);
        write(fd, "--reason=wipe_data_via_recovery\n", strlen("--reason=wipe_data_via_recovery\n") + 1);
        sync();
        close(fd);
    } else {
        LOGE("open /cache/recovery/command failed");
        return -1;
    }

    end_time = time(NULL);
    time_consume = end_time -start_time;
    LOGD("mmitest select menu = <%s> consume time = %d",MENU_FACTORY_RESET,time_consume);
    usleep(200*1000);
    android_reboot(ANDROID_RB_RESTART2, 0, "recovery");
    return 0;
}

void eng_bt_wifi_start(void)
{
    LOGD("==== eng_bt_wifi_start ====");
    eng_wifi_scan_start();
    eng_bt_scan_start();
    LOGD("==== eng_bt_wifi_end ====");
}

int test_bt_wifi_init(void)
{
    pthread_t bt_wifi_init_thread;
    pthread_create(&bt_wifi_init_thread, NULL, (void *)eng_bt_wifi_start, NULL);
    return 0;
}

void test_gps_open(void)
{
    int ret;
    ret = gpsOpen();
    if( ret < 0){
        LOGE("gps open error = %d",ret);
    }
}

int test_gps_init(void)
{
    pthread_t gps_init_thread;
    pthread_create(&gps_init_thread, NULL, (void *)test_gps_open, NULL);
    return 0;
}

int test_channel_init(void *fd)
{
    int len;
    char atbuf[AT_BUFFER_SIZE];
    int fp = *((int*)fd);

    LOGD("mmitest fp = %d",fp);
    for(;;){
        memset(atbuf, 0, AT_BUFFER_SIZE);
        len= read(fp, atbuf, AT_BUFFER_SIZE);
        if (len <= 0) {
            LOGE("mmitest [fp:%d] read stty_lte0 length error %s",fp);
            sleep(1);
            continue;
        }
    }

    return 0;
}

void* test_printlog_thread(void *x)
{
    int ret = -1;
    int fd = -1;

    LOGD("test_printlog_thread start");

    if (0 != access("/data/local/factorytest_log",F_OK) ){
        ret = mkdir("/data/local/factorytest_log",S_IRWXU | S_IRWXG | S_IRWXO);
        if (-1 == ret && (errno != EEXIST)) {
            LOGE("mkdir /data/local/factorytest_log failed.");
            return 0;
        }
    }
    ret = chmod("/data/local/factorytest_log",S_IRWXU | S_IRWXG | S_IRWXO);
    if (-1 == ret) {
        LOGE("chmod /data/local/factorytest_log failed.");
        return 0;
    }

    if (0 == access("/data/local/factorytest_log/last_factorytest.log",F_OK)){
        ret = remove("/data/local/factorytest_log/last_factorytest.log");
        if (-1 == ret) {
            LOGE("remove failed.");
            return 0;
        }
    }

    if (0 == access("/data/local/factorytest_log/factorytest.log",F_OK)){
        ret =  rename("/data/local/factorytest_log/factorytest.log","/data/local/factorytest_log/last_factorytest.log");
        if (-1 == ret) {
            LOGE("rename failed.");
            return 0;
        }
    }

    fd = open("/data/local/factorytest_log/factorytest.log",O_RDWR|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1 && (errno != EEXIST)) {
        LOGE("creat /data/local/factorytest_log/factorytest.log failed.");
        return 0;
    }
    if (fd >= 0 )
        close(fd);

    ret = chmod("/data/local/factorytest_log/factorytest.log",0777);
    if (-1 == ret) {
        LOGE("chmod /data/local/factorytest_log/factorytest.log failed.");
        return 0;
    }

    ret = system("logcat -v threadtime -f /data/local/factorytest_log/factorytest.log &");
    if(!WIFEXITED(ret) || WEXITSTATUS(ret) || -1 ==  ret){
        LOGE("system failed.");
        return 0;
    }

    system("sync");

    return 1;
}

int test_printlog_init(void)
{
    pthread_t printlog_init_thread;
    pthread_create(&printlog_init_thread, NULL, (void *)test_printlog_thread, NULL);
    return 0;
}

int test_tel_init(void)
{
    int thread;
    pthread_t test_tel_init_thread0,test_tel_init_thread1;

    if(1 == mode_type){
        at_fd[0]=open(tel_lte_port[0],O_RDWR);
        at_fd[1]=open(tel_lte_port[1],O_RDWR);
    }else{
        at_fd[0]=open(tel_w_port[0],O_RDWR);
        at_fd[1]=open(tel_w_port[1],O_RDWR);
    }

    if(at_fd[0]<0||at_fd[1]<0){
        LOGE("mmitest tel test is faild");
        return RL_FAIL;
    }

    thread = pthread_create(&test_tel_init_thread0, NULL, (void *)test_channel_init,(void*)&(at_fd[0]));
    if (0 !=  thread){
        LOGE("mmitest read %s thread start failed.",tel_lte_port[0]);
    }
    thread = pthread_create(&test_tel_init_thread1, NULL, (void *)test_channel_init,(void*)&(at_fd[1]));
    if (0 !=  thread){
        LOGE("mmitest read %s thread start failed.",tel_lte_port[1]);
    }

    return 0;
}

static int test_result_mkdir(void)
{
    int i,ret,len = 0;
    int fd_pcba = -1,fd_whole = -1;
    mmi_result result[64] = {0};

    ret = chmod("/productinfo",0777);
    if (-1 == ret) {
        LOGE("mmitest chmod /productinfo failed.");
        goto out;
    }

    if (0 != access(PCBATXTPATH,F_OK)){
	fd_pcba = open(PCBATXTPATH,O_RDWR|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd_pcba < 0) {
	    LOGE("mmitest,create %s failed.",PCBATXTPATH);
	    goto out;
	}
	parse_config();
	//init /productinfo/PCBAtest.txt
	for(i = 0;i < 64; i++){
	    result[i].type_id = 1;
	    result[i].function_id = i;
	    result[i].eng_support= support_result[i].support;
	    result[i].pass_faild = 0;
	}
      result[0].eng_support= 0;//lcd not support
      len = write(fd_pcba,result,sizeof(result));
	if(len < 0){
	    LOGE("mmitest %s write_len = %d.",PCBATXTPATH,len);
	    goto out;
	}
	fsync(fd_pcba);
    }

    if (0 != access(PHONETXTPATH,F_OK)){
	fd_whole = open(PHONETXTPATH,O_RDWR|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd_whole < 0) {
	    LOGE("mmitest create %s failed.",PHONETXTPATH);
	    goto out;
	}
	//init /productinfo/wholetest.txt
	for(i = 0;i < 64; i++){
	    result[i].type_id = 0;
	    result[i].function_id= i;
	    result[i].eng_support= support_result[i].support;
	    result[i].pass_faild = 0;
	}
	len = write(fd_whole,result,sizeof(result));
	if(len < 0){
	    LOGE("mmitest write %s failed! write_len = %d.",PHONETXTPATH,len);
          goto out;
	}
	fsync(fd_whole);
    }

out:
    if(fd_pcba >= 0)
	close(fd_pcba);
    if(fd_whole >= 0)
	close(fd_whole);

    return 0;
}

static void test_result_init(void)
{
	int i = 0;
	int value = -1;
      char property[32] = {0};

	  //ui init
	ui_init();
	ui_set_background(BACKGROUND_ICON_NONE);
	read_bin();

	property_get("persist.modem.l.enable", property, "not_find");
	if(!strcmp(property, "1")){
            mode_type = 1;
	}
      LOGD("persist.modem.l.enable = %s,mode_type:%d", property,mode_type);
}

void test_bt_conf_init(void)
{

    if (0 != access("/data/misc/",F_OK) ){
        if (-1 == mkdir("/data/misc/",0777)) {
            LOGE("mkdir /data/misc/ failed.");
        }
        system("chown system:misc /data/misc/");
    }else{
        LOGD("/data/misc/ exist.");
    }

    if (0 != access("/data/misc/bluedroid",F_OK) ){
        if (-1 == mkdir("/data/misc/bluedroid",0777)) {
            LOGE("mkdir /data/misc/bluedroid failed.");
        }
        system("chown bluetooth:bluetooth /data/misc/bluedroid");
    }else{
        LOGD("/data/misc/bluedroid exist.");
    }

    if (0 != access("/data/misc/bluedroid/bt_stack.conf ",F_OK) ){
	  system("cp /system/etc/bluetooth/bt_stack.conf /data/misc/bluedroid/bt_stack.conf");
	  system("chown system:bluetooth /data/misc/bluedroid/bt_stack.conf");
    }else{
        LOGD("/data/misc/bluedroid/bt_stack.conf exist.");
    }

}

static void test_item_init(void)
{
      test_modem_init();//SIM
      //test_bt_wifi_init();//BT WIFI
      //test_gps_init();//GPS
      test_bt_conf_init();
      test_tel_init();//telephone
}

void sdcard_fm_init(void)
{
    int empty;
    char *p;

    p = getenv("SECONDARY_STORAGE");
    if(p == NULL)
	p = getenv("EXTERNAL_STORAGE");
    if(p == NULL){
	LOGE("mmitest Can't find the external storage environment");
    }
    strncpy(external_path, p, MAX_NAME_LEN-1);
    LOGD("mmitest the external_path : %s",external_path);

    sprintf(sdcard_testfile, "%s/test.txt",external_path);
    LOGD("mmitest the sdcard_testfile : %s", sdcard_testfile);

    sprintf(sdcard_fm_testfile, "%s/fmtest.txt",external_path);
    LOGD("mmitest the sdcard_fm_testfile : %s",sdcard_fm_testfile);

    empty=check_file_exit();

    LOGD("mmitest empty=%d",empty);
    if(0 == empty)
        sdcard_fm_state=sdcard_write_fm(&fm_freq);
    else
        sdcard_fm_state=0;
}

//factory测试程序的main函数
int main(int argc, char **argv)
{
    LOGD("==== factory test start ====");
    test_printlog_init();
    test_result_mkdir();
	//test result init
    test_result_init();
    test_item_init();
    sdcard_fm_init();
    show_root_menu();

    return 1;
}
