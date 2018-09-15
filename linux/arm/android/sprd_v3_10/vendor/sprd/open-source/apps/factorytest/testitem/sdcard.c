#include "testitem.h"
#include <sys/vfs.h>


static const char TCARD_VOLUME_NAME[]  = "sdcard";
static char TCARD_DEV_NAME[128];
static const char SYS_BLOCK_PATH[]     = "/sys/block";
static const char TCARD_FILE_NAME[]    = "/mnt/sdcard/sciautotest";
static const char TCARD_TEST_CONTENT[] = "SCI TCard: 2012-11-12";

extern char external_path[MAX_NAME_LEN];
extern char sdcard_testfile[MAX_NAME_LEN];
extern char sdcard_fm_testfile[MAX_NAME_LEN];

//------------------------------------------------------------------------------
int tcard_get_dev_path(char*path)
{
	DIR *dir;
	struct dirent *de;
	int found = 0;
	char dirpath_open[128] = "/sys/devices";
	char tcard_name[16];

	sprintf(path, "../devices");

	dir = opendir(dirpath_open);
	if (dir == NULL) {
		LOGE("%s open fail", dirpath_open);
		return -1;
	}
	/*try to find dir: sdio_sd, dt branch*/
	while((de = readdir(dir))) {
		if(strncmp(de->d_name, "sdio_sd", strlen("sdio_sd"))) {
			continue;
		}
		sprintf(dirpath_open, "%s/%s", dirpath_open, de->d_name);
		sprintf(path, "%s/%s", path, de->d_name);
		found = 1;
		break;
	}
	/*try to find dir: sdio_sd,no dt branch*/
	if(!found) {
		sprintf(dirpath_open, "%s/platform", dirpath_open);
		closedir(dir);
		dir = opendir(dirpath_open);
		if(dir == NULL) {
			LOGE("%s open fail", dirpath_open);
			return -1;
		}
		while((de = readdir(dir))) {
			if(strncmp(de->d_name, "sdio_sd", strlen("sdio_sd"))) {
				continue;
			}
			sprintf(dirpath_open, "%s/%s", dirpath_open, de->d_name);
			sprintf(path, "%s/platform/%s", path, de->d_name);
			found = 1;
			break;
		}
	}
	closedir(dir);
	if(!found) {
		LOGD("sdio_sd is not found");
		return -1;
	}
	found = 0;
	sprintf(dirpath_open, "%s/mmc_host", dirpath_open);
	dir = opendir(dirpath_open);
	if(dir == NULL) {
		LOGE("%s open failed", dirpath_open);
		return -1;
	}
	/*may be mmc0 or mmc1*/
	while((de = readdir(dir))) {
		if(strstr(de->d_name, "mmc") != NULL) {
			sprintf(dirpath_open, "%s/%s", dirpath_open, de->d_name);
			sprintf(path, "%s/mmc_host/%s", path, de->d_name);
			break;
		}
	}
	strncpy(tcard_name, de->d_name, sizeof(tcard_name)-1);
	if(strlen(de->d_name) < 16) {
		tcard_name[strlen(de->d_name)] = 0;
	} else {
		tcard_name[15] = 0;
	}
	closedir(dir);
	dir = opendir(dirpath_open);/*open dir: sdio_sd/mmc_host/mmc0 or mmc1*/
	if(dir == NULL) {
		LOGE("%s open failed", dirpath_open);
		return -1;
	}
	/* try to find: sdio_sd/mmc_host/mmcx/mmcx:xxxx,  if can find this dir ,T card work well */
	while((de = readdir(dir))) {
		if(strstr(de->d_name, tcard_name) == NULL) {
			continue;
		}
		sprintf(dirpath_open, "%s/%s", dirpath_open, de->d_name);
		sprintf(path, "%s/%s", path, de->d_name);
		found = 1;
		break;
	}
	closedir(dir);
	sprintf(dirpath_open, "%s/block",dirpath_open);
	dir = opendir(dirpath_open);
	if(dir == NULL) {
		LOGE("%s open failed", dirpath_open);
		return -1;
	}
	while((de = readdir(dir)))/*may be mmcblk0 or mmcblk1*/
	{
		if(strstr(de->d_name, "mmcblk") != NULL)
		{
			sprintf(dirpath_open, "%s/%s", dirpath_open, de->d_name);
			sprintf(path, "%s/block/%s", path, de->d_name);
			break;
		}
	}
	closedir(dir);
	if (found) {
		LOGD("the tcard dir is %s",path);
		return 0;
	}else {
		LOGD("the tcard dir is not found");
		return -1;
	}
}



//------------------------------------------------------------------------------
int tcardOpen( void )
{
	int ret = 0;
	ret = tcard_get_dev_path(TCARD_DEV_NAME);
	LOGD("ret = %d",ret);
	return ret;
}

//------------------------------------------------------------------------------
int tcardIsPresent( void )
{
    DIR * dirBlck = opendir(SYS_BLOCK_PATH);
    if( NULL == dirBlck ) {
        LOGD("opendir '%s' failed",SYS_BLOCK_PATH);
        return -1;
    }
    int present = -2;

    char realName[256];
    char linkName[128];
    strncpy(linkName, SYS_BLOCK_PATH, sizeof(linkName) - 1);
    char * pname = linkName + strlen(linkName);
    *pname++ = '/';

    struct dirent *de;
    while( (de = readdir(dirBlck)) ) {
        if (de->d_name[0] == '.' || DT_LNK != de->d_type )
            continue;

        strncpy(pname, de->d_name, 64);

        int len = readlink(linkName, realName, sizeof(realName)-1);
        if( len < 0 ) {
            LOGD("readlink failed");
            continue;
        }
		if(len < 256) {
			realName[len] = 0;
		} else {
			realName[255] = 0;
		}

        LOGD("link name = %s, real name = %s, TCARD_DEV_NAME=%s ", linkName, realName,TCARD_DEV_NAME);
        if(strstr(realName, TCARD_DEV_NAME) != NULL) {
            present = 1;
            LOGD("TCard is present. ");
            break;
        }
    }

    closedir(dirBlck);
    return present;
}

int tcardIsMount( void )
{
    char device[256];
    char mount_path[256];
    FILE *fp;
    char line[1024];

    if (!(fp = fopen("/proc/mounts", "r"))) {
        LOGD("open  /proc/mounts failed");
        return -1;
    }

    int mount = 0;
    while(fgets(line, sizeof(line), fp)) {
        line[strlen(line)-1] = '\0';
        sscanf(line, "%255s %255s\n", device, mount_path);
        LOGD("dev = %s, mount = %s ", device, mount_path);
        if( NULL != strstr(mount_path, TCARD_VOLUME_NAME)) {
            mount = 1;
            LOGD("TCard mount path: '%s' ", mount_path);
            break;
        }
    }

    fclose(fp);
    return mount;
}

int tcardRWTest( void )
{

	FILE * fp = fopen(TCARD_FILE_NAME, "w+");
    if( NULL == fp ) {
        LOGE("mmitest open '%s'(rw) failed",TCARD_FILE_NAME );
        return -1;
    }

    if( fwrite(TCARD_TEST_CONTENT, 1, sizeof(TCARD_TEST_CONTENT), fp) != sizeof(TCARD_TEST_CONTENT) ) {
        LOGE("mmitest write '%s' failed", TCARD_FILE_NAME);
        fclose(fp);
        return -2;
    }
    fclose(fp);

    fp = fopen(TCARD_FILE_NAME, "r+");
    if( NULL == fp ) {
        LOGE("mmitest open '%s'(ronly) failed",TCARD_FILE_NAME);
        return -3;
    }


    char buf[128];

    if( fread(buf, 1, sizeof(TCARD_TEST_CONTENT), fp) != sizeof(TCARD_TEST_CONTENT) ) {
        LOGE("mmitest read '%s' failed", TCARD_FILE_NAME);
        fclose(fp);
        return -4;
    }
    fclose(fp);

    unlink(TCARD_FILE_NAME);

    if( strncmp(buf, TCARD_TEST_CONTENT, sizeof(TCARD_TEST_CONTENT) - 1) ) {
        LOGD("mmitest read = %s, dst = %s", buf, TCARD_TEST_CONTENT);
        return -5;
    }

    LOGD("mmitest TFlash Card rw OK.");
    return 0;
}

//------------------------------------------------------------------------------
int tcardClose( void )
{
	return 0;
}

static int sdcard_rw(void)
{
	int fd;
	int ret = -1;
	unsigned char w_buf[RW_LEN];
	unsigned char r_buf[RW_LEN];
	int i = 0;

	for(i = 0; i < RW_LEN; i++) {
		w_buf[i] = 0xff & i;
	}

	fd = open(sdcard_testfile, O_CREAT|O_RDWR, 0666);
	if(fd < 0){
		LOGE("create %s failed", sdcard_testfile);
		goto RW_END;
	}

	if(write(fd, w_buf, RW_LEN) != RW_LEN){
		LOGE("write data failed");
		goto RW_END;
	}

	lseek(fd, 0, SEEK_SET);
	memset(r_buf, 0, sizeof(r_buf));

	read(fd, r_buf, RW_LEN);
	if(memcmp(r_buf, w_buf, RW_LEN) != 0) {
		LOGE("read data failed");
		goto RW_END;
	}

	ret = 0;
RW_END:
	if(fd > 0) close(fd);
	return ret;
}

int test_sdcard_pretest(void)
{
	int fd;
	int ret;
	system(SPRD_MOUNT_DEV);
	fd = open(SPRD_SD_DEV, O_RDWR);
	if(fd < 0) {
		ret= RL_FAIL;
	} else {
		close(fd);
		ret= RL_PASS;
	}

	save_result(CASE_TEST_SDCARD,ret);
	return ret;
}

int check_file_exit(void)
{
	int ret;
	int fd;
	if(access(sdcard_fm_testfile,F_OK)!=-1)
		ret=1;
	else
		ret=0;
	close(fd);
	return ret;
}

int sdcard_read_fm(int *rd)
{
	int fd;
	int ret;
	fd = open(sdcard_fm_testfile, O_CREAT|O_RDWR,0666);
	if(fd < 0)
        return -1;
	LOGD("mmitest opensdcard file is ok");
	lseek(fd, 0, SEEK_SET);
	ret=read(fd, rd, sizeof(int));
	LOGD("mmitest read file is=%d",*rd);
	close(fd);
	return ret;
}

int sdcard_write_fm(int *freq)
{
	int fd;
	int len=0;

	fd = open(sdcard_fm_testfile, O_CREAT|O_RDWR, 0666);
	if(fd < 0){
		LOGE("mmitest opensdcard file failed");
		return -1;
	}
	len=write(fd, freq, sizeof(int));
	if(len != sizeof(int)){
		LOGE("mmitest write file failed");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int test_sdcard_start(void)
{
	int fd_dev = -1,fd1_size = -1;
	int ret = RL_FAIL; //fail
	int cur_row = 2;
	char temp[64],buffer[64];
	int read_len = 0;
	unsigned long value=0;
	char *endptr;

	ui_fill_locked();
	ui_show_title(MENU_TEST_SDCARD);
	ui_set_color(CL_WHITE);
	cur_row = ui_show_text(cur_row, 0, TEXT_SD_START);
	gr_flip();
	fd_dev = open(SPRD_SD_DEV, O_RDWR);
	if(fd_dev < 0) {
		LOGE("open %s failed",SPRD_SD_DEV);
		ui_set_color(CL_RED);
		cur_row = ui_show_text(cur_row, 0, TEXT_SD_OPEN_FAIL);
		gr_flip();
		goto TEST_END;
	}
	ui_set_color(CL_GREEN);
	cur_row = ui_show_text(cur_row, 0, TEXT_SD_OPEN_OK);
	gr_flip();

	fd1_size = open(SPRD_SD_DEV_SIZE,O_RDONLY);
	if(fd1_size < 0) {
		LOGE("open %s failed",SPRD_SD_DEV_SIZE);
		ui_set_color(CL_RED);
		cur_row = ui_show_text(cur_row, 0, TEXT_SD_STATE_FAIL);
		gr_flip();
		goto TEST_END;
	}
	read_len = read(fd1_size,buffer,sizeof(buffer));
	if(read_len <= 0){
		LOGE("read %s failed,read_len=%d",SPRD_SD_DEV_SIZE,read_len);
		ui_set_color(CL_RED);
		cur_row = ui_show_text(cur_row, 0, TEXT_SD_STATE_FAIL);
		gr_flip();
		goto TEST_END;
	}

	value = strtoul(buffer,&endptr,0);
	LOGD("%s value = %lu, read_len = %d",SPRD_SD_DEV_SIZE, value, read_len);
	ui_set_color(CL_GREEN);
	cur_row = ui_show_text(cur_row, 0, TEXT_SD_STATE_OK);
	sprintf(temp, "%d MB", (value/2/1024));
	cur_row = ui_show_text(cur_row, 0, temp);
	gr_flip();

	if(sdcard_rw()< 0) {
		ui_set_color(CL_RED);
		cur_row = ui_show_text(cur_row, 0, TEXT_SD_RW_FAIL);
		gr_flip();
		goto TEST_END;
	} else {
		ui_set_color(CL_GREEN);
		cur_row = ui_show_text(cur_row, 0, TEXT_SD_RW_OK);
		gr_flip();
	}

	ret = RL_PASS;
TEST_END:
	if(ret == RL_PASS) {
		ui_set_color(CL_GREEN);
		cur_row = ui_show_text(cur_row, 0, TEXT_TEST_PASS);
	} else {
		ui_set_color(CL_RED);
		cur_row = ui_show_text(cur_row, 0, TEXT_TEST_FAIL);
	}
	gr_flip();
	sleep(1);
	if(fd_dev >= 0) close(fd_dev);
	if(fd1_size >= 0) close(fd1_size);
	save_result(CASE_TEST_SDCARD,ret);
	return ret;
}
