#pragma once
#include "Comms.h"
#include <string>
#include <algorithm>

class _Client :
	public Comms
{
public:
	_Client(int port);
	~_Client();
	void Run();
private:
	SOCKET clientSocket;
	int port;
	char username[200];

	bool EstablishConnection();
	bool ConnectToServer();
	static DWORD WINAPI RecvDat(LPVOID lp);
};