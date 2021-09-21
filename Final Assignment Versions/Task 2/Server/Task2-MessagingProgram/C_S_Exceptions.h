#include <exception>
#include <string>
#include "iostream"
#pragma once

using namespace std;

class C_S_Exceptions :
	public exception
{
public:
	int ID;
	C_S_Exceptions(int id) :ID(id) {}
	const char* what() const throw();
};