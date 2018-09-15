#include <stdio.h>
//Quaternion  -->  rotation matrix:

int quat2rot(float quat[4], float* rmx)
{
	float x,y,z,w;
	float xx2, yy2, zz2, xy2, wz2, zx2, wy2, yz2, wx2;

	x = quat[0];
	y = quat[1];
	z = quat[2];
	w = quat[3];

	xx2 = 2 * x * x;
	yy2 = 2 * y * y;
	zz2 = 2 * z * z;
	xy2 = 2 * x * y;
	wz2 = 2 * w * z;
	zx2 = 2 * z * x;
	wy2 = 2 * w * y;
	yz2 = 2 * y * z;
	wx2 = 2 * w * x;

	rmx[0] = 1. - yy2 - zz2;
	rmx[1] = xy2 - wz2;
	rmx[2] = zx2 + wy2;
	rmx[3] = xy2 + wz2;
	rmx[4] = 1. - xx2 - zz2;
	rmx[5] = yz2 - wx2;
	rmx[6] = zx2 - wy2;
	rmx[7] = yz2 + wx2;
	rmx[8] = 1. - xx2 - yy2;

}

void print_rmx(float* rmx, char* name)
{
	printf("%s:\n", name);

	int i;
	for(i = 0; i < 9; i++) {
		printf("rmx[%d] = %f\t", i, rmx[i]);
		if (0 == (i+1)%3)//3x3
			printf("\n");
	}
}

int main(int argc, const char *argv[])
{
	float quat[4] = {-0.56347, 0.287301, 0.342528, 0.694719};
	float rmx[9];

	quat2rot(quat, rmx);
	print_rmx(rmx, "rotation matrix");


	return 0;
}
