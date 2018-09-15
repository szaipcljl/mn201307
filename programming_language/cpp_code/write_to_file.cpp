#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;

struct MyRecord {
	char name[80];
	double balance;
	unsigned long account_num;
};

//save structure to a file
int saveStructureToFile(char * file_name,void * pdata, size_t size)
{
	ofstream outbal(file_name /*"balance.bin"*/, ios::out | ios::binary);
	if(!outbal) {
		cout << "Cannot open file.\n";
		return 1;
	}

	outbal.write((char *) pdata /*&acc*/,size /*sizeof(struct MyRecord)*/);
	outbal.close();

	return 0;
}


// write to file
int main ()
{

	struct MyRecord acc;

	//init structure
	strcpy(acc.name, "R");
	acc.balance = 1.3;
	acc.account_num = 34;

	saveStructureToFile("balance.bin", &acc, sizeof(MyRecord));

	return 0;
}
