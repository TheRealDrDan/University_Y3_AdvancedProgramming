#pragma once
#include "Comms.h"
#include <vector>
#include <algorithm>
class _Server :
	public Comms
{
public:
	_Server(int port);
	~_Server();
	void Run();
private:
	SOCKET serverSocket;
	SOCKET acceptSocket;
	int port;
	static vector<SOCKET> acceptedSockets;

	bool Bind();
	bool Listen();

	static DWORD WINAPI SendData(LPVOID lpParam);
	static DWORD WINAPI ServerThread(LPVOID lpParam);
};

