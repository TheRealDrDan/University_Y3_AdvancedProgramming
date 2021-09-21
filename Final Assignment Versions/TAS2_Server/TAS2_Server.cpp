// Task2-MessagingProgram.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <winsock2.h>
#include <ws2tcpip.h>
#include "iostream"
#include <tchar.h> 
#pragma warning(disable:4996)

///TO RUN THE PROGRAM AS A SERVER "#define IS_SERVER" MUST BE UN-COMMENTED ELSE IT WILL RUN AS A CLIENT!!!
#define IS_SERVER

#ifdef IS_SERVER
	//SERVER
#include "_Server.h"
#else
	//CLIENT
#include "_Client.h"
#endif

using namespace std;

int main()
{
#ifdef IS_SERVER
	//SERVER
	if (!Comms::CheckWinSock())
		return 0;
	_Server* sr = new _Server(55555);
	sr->Run();
	delete sr;
	system("PAUSE");
	return 0;
#else
	//CLIENT
	if (!Comms::CheckWinSock())
		return 0;
	_Client* cl = new _Client(55555);
	cl->Run();
	delete cl;
	system("PAUSE");
	return 0;
#endif
}
