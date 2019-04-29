#include <stdio.h>
//位结构体的使用

#define u8 unsigned char
#define u16 unsigned short

//位结构体
struct data {
	u8 flag1:	1;		//占用1位, 最多表示0或1
	u8 flag2:	2;		//占用2位, 最多表示0,1,2,3
	u8 flag3:	5;		//下面以此类推
} data_s = {1, 0, 5};

//位结构体
struct data1 {

	u16 flag1:	1;
	u16 flag2:	1;
	u16 flag3:	6;
} data_s1 = {1, 0, 5};

int main(int argc, const char *argv[])
{
	printf("占用内存：%d位\n", sizeof(data_s)*8);
	printf("%d %d %d\n", data_s.flag1, data_s.flag2, data_s.flag3);

	printf("占用内存：%d位\n", sizeof(data_s1)*8);
	printf("%d %d %d\n", data_s.flag1, data_s.flag2, data_s.flag3);

	return 0;
}

/*
 * The declaration of a bit-field has the following form inside a structure −
 *
 * struct {
 *    type [member_name] : width ;
 * };
 */
