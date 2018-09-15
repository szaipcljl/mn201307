//Save and read structure

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

struct MyRecord {
	char name[80];
	double balance;
	unsigned long account_num;
};


int main()
{
	struct MyRecord acc;

	//init structure
	strcpy(acc.name, "R");
	acc.balance = 1.3;
	acc.account_num = 34;

#if 1
	//save structure to a file
	ofstream outbal("balance.bin", ios::out | ios::binary);
	if(!outbal) {
		cout << "Cannot open file.\n";
		return 1;
	}

	outbal.write((char *) &acc, sizeof(struct MyRecord));
	outbal.close();
#endif

#if 1
	//read structure
	ifstream inbal("balance.bin", ios::in | ios::binary);
	if(!inbal) {
		cout << "Cannot open file.\n";
		return 1;
	}

	inbal.read((char *) &acc, sizeof(struct MyRecord));

	cout << acc.name << endl;
	cout << "Account # " << acc.account_num;
	cout.precision(2);
	cout.setf(ios::fixed);
	cout << endl << "Balance: $" << acc.balance << endl;

	inbal.close();
#endif

	return 0;
}
