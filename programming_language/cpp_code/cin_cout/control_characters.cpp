#include <iostream>
#include <iomanip>//control characters

using namespace std;

int main(int argc, const char *argv[])
{
	double x = 123.456789012345;

	cout << x << endl;//123.457
	cout << setprecision(9) << x << endl;//123.456789
	cout << setprecision(6) << x << endl;//default format,precision = 6

	cout << setiosflags(ios::fixed) << x << endl;//123.456789
	cout << setiosflags(ios::fixed) << setprecision(8) << x << endl;//123.45678901


	//各行小数点对齐
	double a = 123.456, b=3.14159, c=-3214.67;
	cout << setiosflags(ios::fixed) << setiosflags(ios::right) << setprecision(2);
	cout << setw(10) << a << endl;
	cout << setw(10) << b << endl;
	cout << setw(10) << c <<endl;

	return 0;
}
