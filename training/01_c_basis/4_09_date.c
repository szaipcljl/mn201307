#include <stdio.h>

int main(int argc, const char *argv[])
{
	int year,month,day;
	int flag = 0;
	int sum = 0;

	printf("input the date(year-month-day):\n");
	if(scanf("%d-%d-%d",&year,&month,&day) != 3){
		printf("the date input error\n");
		return -1;
	}

	if(day > 31 || day < 1){
		printf("error1:day > 31 or day < 1\n");
		return -1;
	}

	if(day > 30){
		if(month == 4 || month == 6 || month == 9 || month == 11){
			printf("error2:day > 30\n");
			return -1;
		}
	}

	if(year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
		flag = 1;

	if(month == 2 && flag == 1){
		if(day > 29){
			printf("error3:day > 29\n");
			return -1;
		}
	}


	if(month == 2 && flag == 0){
		if(day > 28){
			printf("error4:day > 28\n");
			return -1;
		}
	}

	switch(month)
	{
	case 12:
		sum += 30;
	case 11:
		sum += 31;
	case 10:
		sum += 30;
	case 9:
		sum += 31;
	case 8:
		sum += 31;
	case 7:
		sum += 30;
	case 6:
		sum += 31;
	case 5:
		sum += 30;
	case 4:
		sum += 31;
	case 3:
		sum += 28 + flag;
	case 2:
		sum += 31;
	case 1:
		sum += day;
		break;
	default:
		printf("month error\n");
		return -1;
	}

	printf("the %dth day of the year\n",sum);
	return 0;
}
