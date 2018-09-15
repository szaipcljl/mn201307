#include <stdio.h>
#include <time.h>

#define ct_size 20
/******************************************************************************
 * 函数功能: 获得当前系统时间
 * 输入: szCurTimr: 字串型时间结果缓冲地址
 * 		 iFormat:    时间类型格式
 * 返回值:
 *******************************************************************************/
int GetCurrentTime(char *szCurTime, int bufsize, int iFormat)
{
	time_t  tTime;
	struct tm *localTime;

	time(&tTime);
	localTime=localtime(&tTime);
	if (NULL == localTime)
		return -1;

	if (iFormat == 1) {
		snprintf(szCurTime, bufsize,"%d%02d%02d",localTime->tm_year+1900,
				localTime->tm_mon+1,localTime->tm_mday);
	} else {
		snprintf(szCurTime, bufsize,"%d-%02d-%02d %02d:%02d:%02d",
				localTime->tm_year+1900, localTime->tm_mon+1, localTime->tm_mday,
				localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
	}

	return 0;
}

int main(int argc, const char *argv[])
{
	int iFormat = 0;
	char szCurTime[ct_size];

	GetCurrentTime(szCurTime, sizeof(szCurTime), iFormat);

	printf("the current time: %s\n",szCurTime);
	return 0;
}
