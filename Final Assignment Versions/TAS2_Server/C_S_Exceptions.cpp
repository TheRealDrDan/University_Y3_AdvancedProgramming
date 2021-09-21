#include "C_S_Exceptions.h"
const char* C_S_Exceptions::what() const throw()
{
	string e = "An Unknown Exception Occured; ";
	e += to_string(ID);
	char eA[100];
	strcpy_s(eA, 100, e.c_str());

	cout << eA << endl;

	return eA;
}