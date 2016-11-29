#include <iostream>
#include <cmath>
#include <limits>
//#include <cstdlib>	//system

using namespace std;
typedef double ValType;
struct Quat;
struct Matrix;
struct Quat {
	ValType _v[4];//x, y, z, w
	/// Length of the quaternion = sqrt( vec . vec )
	ValType length() const {
		return sqrt( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3]);
	}
	/// Length of the quaternion = vec . vec
	ValType length2() const {
		return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
	}
};
struct Matrix {
	ValType _mat[3][3];
};

#define QX q._v[0]
#define QY q._v[1]
#define QZ q._v[2]
#define QW q._v[3]
void Quat2Matrix(const Quat& q, Matrix& m)
{
	double length2 = q.length2();
	if (fabs(length2) <= std::numeric_limits<double>::min()) {
		m._mat[0][0] = 0.0; m._mat[1][0] = 0.0; m._mat[2][0] = 0.0;
		m._mat[0][1] = 0.0; m._mat[1][1] = 0.0; m._mat[2][1] = 0.0;
		m._mat[0][2] = 0.0; m._mat[1][2] = 0.0; m._mat[2][2] = 0.0;
	} else {
		double rlength2;
		// normalize quat if required.
		// We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
		// That is a square of a square root, so it is possible to avoid that
		if (length2 != 1.0) {
			rlength2 = 2.0/length2;
		} else {
			rlength2 = 2.0;
		}

		// Source: Gamasutra, Rotating Objects Using Quaternions
		//
		//http://www.gamasutra.com/features/19980703/quaternions_01.htm

		double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		// calculate coefficients
		x2 = rlength2 * QX;
		y2 = rlength2 * QY;
		z2 = rlength2 * QZ;

		xx = QX * x2;
		xy = QX * y2;
		xz = QX * z2;

		yy = QY * y2;
		yz = QY * z2;
		zz = QZ * z2;

		wx = QW * x2;
		wy = QW * y2;
		wz = QW * z2;

		// Note. Gamasutra gets the matrix assignments inverted, resulting
		// in left-handed rotations, which is contrary to OpenGL and OSG's
		// methodology. The matrix assignment has been altered in the next
		// few lines of code to do the right thing.
		// Don Burns - Oct 13, 2001
		m._mat[0][0] = 1.0 - (yy + zz);
		m._mat[1][0] = xy - wz;
		m._mat[2][0] = xz + wy;


		m._mat[0][1] = xy + wz;
		m._mat[1][1] = 1.0 - (xx + zz);
		m._mat[2][1] = yz - wx;

		m._mat[0][2] = xz - wy;
		m._mat[1][2] = yz + wx;
		m._mat[2][2] = 1.0 - (xx + yy);
	}
}
void Matrix2Quat(const Matrix& m, Quat& q)
{
	ValType s;
	ValType tq[4];
	int    i, j;
	// Use tq to store the largest trace
	tq[0] = 1 + m._mat[0][0]+m._mat[1][1]+m._mat[2][2];
	tq[1] = 1 + m._mat[0][0]-m._mat[1][1]-m._mat[2][2];
	tq[2] = 1 - m._mat[0][0]+m._mat[1][1]-m._mat[2][2];
	tq[3] = 1 - m._mat[0][0]-m._mat[1][1]+m._mat[2][2];
	// Find the maximum (could also use stacked if's later)
	j = 0;
	for(i=1;i<4;i++) j = (tq[i]>tq[j])? i : j;
	// check the diagonal
	if (j==0) {
		/* perform instant calculation */
		QW = tq[0];
		QX = m._mat[1][2]-m._mat[2][1];
		QY = m._mat[2][0]-m._mat[0][2];
		QZ = m._mat[0][1]-m._mat[1][0];
	} else if (j==1) {
		QW = m._mat[1][2]-m._mat[2][1];
		QX = tq[1];
		QY = m._mat[0][1]+m._mat[1][0];
		QZ = m._mat[2][0]+m._mat[0][2];
	} else if (j==2) {
		QW = m._mat[2][0]-m._mat[0][2];
		QX = m._mat[0][1]+m._mat[1][0];
		QY = tq[2];
		QZ = m._mat[1][2]+m._mat[2][1];
	} else /* if (j==3) */ {
		QW = m._mat[0][1]-m._mat[1][0];
		QX = m._mat[2][0]+m._mat[0][2];
		QY = m._mat[1][2]+m._mat[2][1];
		QZ = tq[3];
	}
	s = sqrt(0.25/tq[j]);
	QW *= s;
	QX *= s;
	QY *= s;
	QZ *= s;
}

void printMatrix(const Matrix& r, string name)
{
	cout<<"RotMat "<<name<<" = "<<endl;
	cout<<"\t"<<r._mat[0][0]<<" "<<r._mat[0][1]<<" "<<r._mat[0][2]<<endl;
	cout<<"\t"<<r._mat[1][0]<<" "<<r._mat[1][1]<<" "<<r._mat[1][2]<<endl;
	cout<<"\t"<<r._mat[2][0]<<" "<<r._mat[2][1]<<" "<<r._mat[2][2]<<endl;
	cout<<endl;
}

void printQuat(const Quat& q, string name)
{
	cout<<"Quat "<<name<<" = "<<endl;
	cout<<"\t"<<q._v[0]<<" "<<q._v[1]<<" "<<q._v[2]<<" "<<q._v[3]<<endl;
	cout<<endl;
}

int main()
{
	ValType phi, omiga, kappa;
	phi = 1.32148229302237 ;
	omiga = 0.626224465189316 ;
	kappa = -1.4092143985971;
	ValType a1,a2,a3,b1,b2,b3,c1,c2,c3;

	a1 = cos(phi)*cos(kappa) - sin(phi)*sin(omiga)*sin(kappa);
	a2 = -cos(phi)*sin(kappa) - sin(phi)*sin(omiga)*cos(kappa);
	a3 = -sin(phi)*cos(omiga);
	b1 = cos(omiga)*sin(kappa);
	b2 = cos(omiga)*cos(kappa);
	b3 = -sin(omiga);
	c1 = sin(phi)*cos(kappa) + cos(phi)*sin(omiga)*sin(kappa);
	c2 = -sin(phi)*sin(kappa) + cos(phi)*sin(omiga)*cos(kappa);
	c3 = cos(phi)*cos(omiga);

	Matrix r;
	r._mat[0][0] = a1;
	r._mat[0][1] = a2;
	r._mat[0][2] = a3;
	r._mat[1][0] = b1;
	r._mat[1][1] = b2;
	r._mat[1][2] = b3;
	r._mat[2][0] = c1;
	r._mat[2][1] = c2;
	r._mat[2][2] = c3;
	printMatrix(r, "r");

	Quat q;
	Matrix2Quat(r, q);
	printQuat(q, "q");

	Matrix _r;
	Quat2Matrix(q, _r);
	printMatrix(_r, "_r");

	//system("pause");
	return 0;
}
