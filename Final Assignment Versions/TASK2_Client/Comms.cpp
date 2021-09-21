#include "Comms.h"
int Comms::SendData(SOCKET& socket, char* message)
{
	///<summary> SendData sends data to a target socket.
	///<param>	"socket" takes the address of the socket thats sending data. "message" is the char array that it'll send.
	///<returns> returns an integer as validation for error handling. A value below 0 is considered Erroneous.
	///</summary>	
	int byteCount = SOCKET_ERROR;
	byteCount = send(socket, message, 200, 0);
	return byteCount;
}

int Comms::RecvData(SOCKET& socket, char& buffer)
{
	///<summary> RecvData receives data from a target socket.
	///<param> "socket" takes the address of the socket thats receiving data. "buffer" is the address of the char array that'll populate with the received data.
	///<remark> "buffer" is an address to allow the function to return an integer value, whilst still returning the received data.
	///<returns> returns an integer as validation for error handling. A value below 0 is considered Erroneous.
	int byteCount = SOCKET_ERROR;
	byteCount = recv(socket, &buffer, 200, 0);
	return byteCount;
}

bool Comms::CheckWinSock()
{
	///<summary> CheckWinSock() checks for inclusion of Winsock dll file.
	///<returns> Returns true is Winsock dll is included.
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	try
	{
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
			throw (C_S_Exceptions(0));
		cout << "The Winsock dll found!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
	}
	catch (C_S_Exceptions e)
	{
		switch (e.ID)
		{
		case -1:
			cout << "The Winsock dll not found!" << endl;
			return false;
			break;
		default:
			e.what();
			return false;
		}
	}
	catch (std::exception & e)
	{
		cout << e.what() << endl;
		return false;
	}
	return true;
}

bool Comms::DefineASocket(SOCKET& _socket)
{
	///<summary> DefineASocket() initialises a socket.
	///<param> "_socket" contains the address of the socket that is to be defined.
	///<returns> Returns true is socket is initialised successfully.
	try
	{
		_socket = INVALID_SOCKET;
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket == INVALID_SOCKET)
			throw(C_S_Exceptions(WSAGetLastError()));
		cout << "socket() is OK!" << endl;
	}
	catch (C_S_Exceptions e)
	{
		cout << "Error at socket(): " << e.ID << endl;
		WSACleanup();
		return false;
	}
	catch (std::exception & e)
	{
		cout << e.what() << endl;
		return false;
	}
	return true;
}
