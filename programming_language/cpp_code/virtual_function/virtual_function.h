#ifndef _VIRTUAL_FUNCTION_H
#define _VIRTUAL_FUNCTION_H

using namespace std;

class Base {
public:
	virtual void f() { cout << "Base::f" << endl; }
	virtual void g() { cout << "Base::g" << endl; }
	virtual void h() { cout << "Base::h" << endl; }

};

#endif
