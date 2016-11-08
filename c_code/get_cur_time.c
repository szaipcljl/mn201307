#include <stdio.h>
#include <time.h>

/******************************************************************************
 * 函数功能: 获得当前系统时间
 * 输入: szCurTimr: 结果缓冲地址
 * 		 iFormat:    时间类型格式
 * 返回值r: 返回字串型时间的地址
 *******************************************************************************/
char *GetCurrentTime(char *szCurTime, int iFormat)
{
	time_t  tTime;
	struct tm *localTime;

	time(&tTime);
	localTime=localtime(&tTime);

	if (iFormat == 1) {
		snprintf(szCurTime,100,"%d%02d%02d",localTime->tm_year+1900,
				localTime->tm_mon+1,localTime->tm_mday);
	} else {
		snprintf(szCurTime,100,"%d-%02d-%02d %02d:%02d:%02d",localTime->tm_year+1900,
				localTime->tm_mon+1,localTime->tm_mday,localTime->tm_hour,localTime->tm_min,localTime->tm_sec);
	}

	return(szCurTime);
} 

int main(int argc, const char *argv[])
{
	int iFormat = 0;
	char szCurTime[100];

	GetCurrentTime(szCurTime, iFormat);

	printf("the current time: %s\n",szCurTime);
	return 0;
}
