#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "factorytest.h"
#include "common.h"

#define MAX_LINE_LEN			256
#define PCBA_SUPPORT_CONFIG   "/productinfo/PCBA.conf"

static int parse_string(char * buf, char gap, char* value)
{
    int len = 0;
    char *ch = NULL;
    char str[10] = {0};

    if(buf != NULL && value  != NULL){
        ch = strchr(buf, gap);
        if(ch != NULL){
            len = ch - buf ;
            strncpy(str, buf, len);
            *value = atoi(str);
        }
    }
    return len;
}

int parse_2_entries(char *type, char* arg1, char* arg2)
{
	int len;
	char *str = type;

	/* sanity check */
	if(str == NULL) {
		LOGD("type is null!");
		return -1;
	}
	
	if((len = parse_string(str, '\t', arg1)) <= 0)	return -1;

	str += len + 1;
	if(str == NULL) {
		LOGD("mmitest type is null!");
		return -1;
	}
	if((len = parse_string(str, '\t', arg2)) <= 0)	return -1;

	return 0;
}

int parse_config()
{
	FILE *fp;
	int ret = 0, count = 0;
	char id,flag;
	char buffer[MAX_LINE_LEN]={0};
	
	fp = fopen(PCBA_SUPPORT_CONFIG, "r");
	if(fp == NULL) {
		LOGE("mmitest open %s failed", PCBA_SUPPORT_CONFIG);
		return -1;
	}

	/* parse line by line */
	ret = 0;
	while(fgets(buffer, MAX_LINE_LEN, fp) != NULL) {
		if(buffer[0] == '#')
			continue;
		ret = parse_2_entries(buffer,&id,&flag);
		if(ret != 0) {
			LOGD("mmitest parse %s return %d.  reload", PCBA_SUPPORT_CONFIG,ret);
			fclose(fp);
			return -1;
		}
		support_result[count].id = id;
		support_result[count++].support= flag;
	}

	fclose(fp);
	if(count < TOTAL_NUM) {
		LOGD("mmitest parse slog.conf failed");
	}

	return ret;
}
