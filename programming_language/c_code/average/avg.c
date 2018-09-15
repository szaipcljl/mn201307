#include <stdio.h>

#define MAX_NUM (100 * 10000)

typedef long long int  int_64;

void calc_sum_avg(int_64* sum, int_64 data, int_64 count, int_64* avg)
{
	*sum += data;
	*avg = *sum / count;
}

//avg[n] = ((n-1) * avg[n-1] + data[n]) / n
int main(int argc, const char *argv[])
{
	int_64 count, data = 0;
	int_64 avg = 0, sum = 0;

	for (count = 1; count <= MAX_NUM; count++) {
		data = count;

		//sum += data;
		//avg = sum / count;
		calc_sum_avg(&sum, data, count, &avg);

	}
	printf("sum = %lld, avg = %lld\n", sum, avg);


	return 0;
}
