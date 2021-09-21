#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "iostream"
#include <tchar.h> 
#include "C_S_Exceptions.h"

using namespace std;
class Comms
{
public:
	static int SendData(SOCKET& socket, char* message);
	static int RecvData(SOCKET& socket, char& buffer);
	static bool CheckWinSock();
	static bool DefineASocket(SOCKET& _socket);
};
