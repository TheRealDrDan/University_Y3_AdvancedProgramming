#include "_Server.h"

///Static vector of sockets, used to track currently connected clients. Is static to allow threads to interact with it.
vector<SOCKET> _Server::acceptedSockets;

_Server::_Server(int port)
{
	///<summary> The constructor initialises "serverSocket", "acceptSocket" and "port"
	///<remark> The constructor also defines the server's socket.
	serverSocket = INVALID_SOCKET;
	acceptSocket = INVALID_SOCKET;
	this->port = port;

	Comms::DefineASocket(serverSocket);
}

_Server::~_Server()
{
	///<summary> The deconstructor allows for a safe closure of the server's code.
	///<remark> The server socket is closed and wiped, while variables are set to "NULL" or cleared.
	WSACleanup();
	serverSocket = INVALID_SOCKET;
	shutdown(serverSocket, NULL);
	closesocket(serverSocket);
	acceptedSockets.clear();
}

bool _Server::Bind()
{
	///<summary> Bind() is responsible for binding an IP address for the server's socket.
	///<return> Returns true is the binding was successful.
	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(port);
	try
	{
		int byteCount = bind(serverSocket, (SOCKADDR*)&service, sizeof(service));
		if (byteCount < 0)
			throw(C_S_Exceptions(WSAGetLastError()));
		cout << "bind() is OK!" << endl;
	}
	catch (C_S_Exceptions e)
	{
		switch (e.ID) {
		case 10022:
			cout << "bind() failed: " << e.ID << endl;
			closesocket(serverSocket);
			WSACleanup();
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

bool _Server::Listen()
{
	///<summary> Listen() is responsible for listening for the first new connection.
	///<return> Returns true is listen is successful.
	try
	{
		int byteCount = listen(serverSocket, 1);
		if (byteCount < 0)
			throw C_S_Exceptions(WSAGetLastError());
		cout << "listen() is OK, waiting for connections..." << endl;
	}
	catch (C_S_Exceptions e)
	{
		switch (e.ID) {
		case 10022:
			cout << "listen() failed: " << e.ID << endl;
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




DWORD WINAPI _Server::SendData(LPVOID)
{
	///<summary> SendData() is a threading-function responsible for transmitting User inputted data to connected clients.
	///<remark> SendData() constantly waits for any Console input, then cycles connected clients to transmit the data to.
	///<return> Returns a 0 upon successful execution and a -1 upon unsuccessful execution.
	char buffer[200] = {};
	while (1) {
		char serverMessage[200] = { "Server : " };
		cin.getline(buffer, 200);
		strncat_s(serverMessage, buffer, 200);
		for (int i = 0; i < acceptedSockets.size(); i++) {
			try
			{
				int byteCount = Comms::SendData(acceptedSockets[i], serverMessage);
				if (byteCount < 0)
					throw (C_S_Exceptions(WSAGetLastError()));
				cout << "Server: sent " << byteCount << endl;
			}
			catch (C_S_Exceptions e)
			{
				switch (e.ID) {
				case 10054:
					acceptedSockets.erase(std::remove(acceptedSockets.begin(), acceptedSockets.end(), acceptedSockets[i]), acceptedSockets.end());
					cout << "(10054) Connection lost to a Client..." << endl;
					break;
				case 10038:
					acceptedSockets.erase(std::remove(acceptedSockets.begin(), acceptedSockets.end(), acceptedSockets[i]), acceptedSockets.end());
					break;
				default:
					e.what();
					return -1;
				}
			}
			catch (std::exception & e)
			{
				cout << e.what() << endl;
				return -1;
			}
		}
	}
	return 0;
}

///IGNORE 
//struct Data {				
//	SOCKET socket;
//  int id;
//};

DWORD WINAPI _Server::ServerThread(LPVOID lpParam) {

	//struct Data* data = (struct Data*)lpParam;						///IGNORE (Testing to see if LPVOID could be casted to and from a struct type, which it can :D. This is to allow multiple values to be passed to LPVOID in one shot.)
	//SOCKET socket = data->socket;										///Potential for future work!							
	//int ID = data->id;

	///<summary> ServerThread() is a threading-function responsbile for listening to incoming messages and broadcasting them to the console and all connected clients.
	///<param> "lpParam" is used to pass the socket this function will represent. Is casted from LPVOID to SOCKET.
	///<return> Returns a 0 upon successful execution and a -1 upon unsuccessful execution.
	///<remark> If a 0 is returned, the client safely closed the connection. If a -1 is returned, the client illegally closed the connection.
	SOCKET socket = (SOCKET)lpParam;
	int byteCount;
	char buffer[200] = "";
	while (1) {
		//CHECK IS SOCKET IS VALID
		if (socket != INVALID_SOCKET) {
			// Block until data received
			byteCount = Comms::RecvData(socket, *buffer);

			//IF RECVDATA is SOCKET_ERROR, Client closed the connection in error
			if (byteCount == SOCKET_ERROR) {
				cout << "Connection closed" << endl;
				acceptedSockets.erase(std::remove(acceptedSockets.begin(), acceptedSockets.end(), socket), acceptedSockets.end());
				return -1;
			}
			// Client connection was closed 
			if (byteCount == 0) {
				shutdown(socket, SD_SEND);
				closesocket(socket);
				socket = INVALID_SOCKET;
				return -1;
			}
			else {
				cout << "From Client : " << buffer << endl;
				for (int i = 0; i < acceptedSockets.size(); i++)
				{
					if (acceptedSockets[i] != socket) {
						//char sendBuffer[200] = "Message Received";
						byteCount = Comms::SendData(acceptedSockets[i], buffer);

						if (byteCount == SOCKET_ERROR) {
							cout << "Server send error " << WSAGetLastError() << endl;
							closesocket(acceptedSockets[i]);
							acceptedSockets.erase(std::remove(acceptedSockets.begin(), acceptedSockets.end(), acceptedSockets[i]), acceptedSockets.end());
							return -1;
						}
						else {
							cout << "Server: sent " << byteCount << endl;
						}
					}
				}
			}
		}
	}
	closesocket(socket);
	return 0;
}

void _Server::Run()
{
	///<summary> Run() executes the Server code.
	///<remarks> Run() is responsible for executing the Server code in the correct order of execution.
	///<remarks> Run() also continously listens for new connecting clients.
	///<return> Even though Run has no return type, Run will exit if it encounters an error primarily in the setup phase.
	if (!Bind())
		return;
	if (!Listen())
		return;
	DWORD threadId;
	CreateThread(NULL, 0, SendData, NULL, 0, &threadId);
	while (1) {
		try
		{
			acceptSocket = accept(serverSocket, NULL, NULL);
			if (acceptSocket == INVALID_SOCKET)
				throw (C_S_Exceptions(WSAGetLastError()));
			cout << "Server: Client Connected!" << endl;
			acceptedSockets.push_back(acceptSocket);			
			CreateThread(NULL, 0, ServerThread, (LPVOID)acceptSocket, 0, &threadId);

			//Data newstruct;																	///IGNORE (Testing to see if LPVOID could be casted to and from a struct type, which it can :D. This is to allow multiple values to be passed to LPVOID in one shot.)
			//newstruct.socket = acceptSocket;													///Potential for future work!
			//newstruct.id = 10;
			//CreateThread(NULL, 0, ServerThread, (LPVOID)&newstruct, 0, &threadId);
		}
		catch (C_S_Exceptions e) {
			cout << stderr << " accept failed: " << e.ID << endl;
			return;
		}
		catch (std::exception& e)
		{
			cout << e.what() << endl;
			return;
		}
	}
}
