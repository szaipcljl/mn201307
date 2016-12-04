#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUF_SIZE 1024

int progress_update(unsigned long total_max, unsigned long total_val, int child_max, int child_val, struct timeval *last_refresh)
{
	double total_percent, child_percent;
	int i;
	char pstr[BUF_SIZE];
	struct timeval *org_last_refresh = last_refresh;

	if (total_max < 1 || total_val < 0 || child_max < 1 || child_val < 0) {
		printf("[%s:%d] total_max or child_max < 1; total_val or child_val < 0\n", __func__, __LINE__);
		return -1;
	}

	if (total_val > total_max || child_val > child_max) {
		printf("[%s:%d] total_val > total_max or child_val > child_max\n", __func__, __LINE__);
		return -1;
	}

	if (total_max != total_val || child_max != child_val) {
		struct timeval current_time;
		gettimeofday(&current_time, NULL);

		if (current_time.tv_sec - org_last_refresh->tv_sec < 1) {
			printf("time intervals < 1\n");
			return -1;
		}
	}

	gettimeofday(org_last_refresh, NULL);
	memset(pstr, 0, BUF_SIZE);

	// total progress: xx.xx%
	total_percent = 100.00 * total_val / total_max;
	sprintf(pstr, "Total: %6.2f%s", total_percent, "%");
	// [======       ]
	strcat(pstr, "\t[");
	for (i= 0; i < total_percent / 100 * 30; i++)
		strcat(pstr, "=");
	for (; i < 30 ; i++)
		strcat(pstr, " ");
	strcat(pstr, "]");

	// child progress
	child_percent = 100.00 * child_val / child_max;
	sprintf(pstr, "%s\tChild: %6.2f%s", pstr, child_percent, "%");
	// [======       ]
	strcat(pstr, "\t[");
	for (i=0; i < child_percent / 100 * 30; i++)
		strcat(pstr, "=");
	for (; i < 30; i++)
		strcat(pstr, " " );
	strcat(pstr, "]" );

	// refresh screen
	fflush(stdout);
	for (i = 0; i < (strlen(pstr) * 1.5); i++)
		printf("\b");//退格键 backspace
	printf("%s",pstr);

	if (total_max == total_val && child_max == child_val)
		printf("\n");

	return 0;
}


int main(int argc, const char *argv[])
{
	unsigned long total_max = 100;
	unsigned long total_val = 0;
	int child_max = 100;
	int child_val = 0;
	int ret;
	struct timeval last_refresh;

	while (1) {
		usleep(1000000);
		ret = progress_update(total_max, total_val, child_max, child_val, &last_refresh);
		if (ret < 0) {
			printf("[%s:%d\n] failed", __func__, __LINE__);
			return -1;
		}

		if (total_val == total_max && child_val == child_max)
			break;
		if (total_val < total_max)
			total_val += 10;
		if (child_val < child_max)
			child_val += 10;
	}

	return 0;
}
