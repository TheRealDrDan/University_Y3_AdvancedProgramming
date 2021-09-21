#include "_Client.h"

_Client::_Client(int port)
{
	///<summary> The constructor initialises "clientSocket", and "port"
	///<remark> The constructor also defines the client's socket and requests a Username from the user.
	clientSocket = INVALID_SOCKET;
	this->port = port;		//Usually 55555	

	Comms::DefineASocket(clientSocket);

	//Asks user for a Username (Can be left blank)
	cout << "Enter a username (20 characters)...";
	cin.getline(username, 20);
	char tab[4] = { " : " };
	strncat_s(username, tab, 20);
}

_Client::~_Client()
{
	///<summary> The deconstructor allows for a safe closure of the client's code.
	///<remark> The client socket is closed and wiped, while variables are set to "NULL" or cleared.
	WSACleanup();
	clientSocket = INVALID_SOCKET;
	shutdown(clientSocket, NULL);
	closesocket(clientSocket);
}

bool _Client::ConnectToServer()
{
	///<summary> ConnectToServer() is repsonsible for creating the connection between Client and Server.
	///<return> Returns true is the connection is successful.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);
	int byteCount;
	try
	{
		byteCount = connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (byteCount < 0)
			throw(C_S_Exceptions(WSAGetLastError()));
		cout << "Client Connected Successfully!" << endl;
		cout << "Client: Can start sending and receiving data..." << endl;
	}
	catch (C_S_Exceptions e)
	{
		switch (e.ID) {
		case 10061:
			cout << "Connection to Server Failed." << endl;
			break;
		default:
			e.what();
		}
		return false;
	}
	catch (std::exception & e) {
		cout << e.what() << endl;
		return false;
	}
	return true;
}

DWORD WINAPI _Client::RecvDat(LPVOID lp) {
	///<summary> RecvDat is a threading-function used to listen for any incoming messages from the server.
	///<param> "lp" is the socket that this threading-function is listening on.
	///<return> Returns a 0 upon successful execution and a -1 upon unsuccessful execution.
	SOCKET socket = (SOCKET)lp;
	char buffer[200] = "";
	while (1) {
		int	byteCount;
		try
		{
			byteCount = Comms::RecvData(socket, *buffer);
			if (byteCount < 0)
				throw(C_S_Exceptions(WSAGetLastError()));
			cout << "Received :  " << buffer << endl;
		}
		catch (C_S_Exceptions e)
		{
			switch (e.ID) {
			case 10053:
				cout << "Connection closed..." << endl;
				return 0;
				break;
			case 10054:
				cout << "(10054) Connection to Server Lost..." << endl;
				shutdown(socket, SD_SEND);
				closesocket(socket);
				socket = INVALID_SOCKET;
				return -1;
				break;
			case 10093:
				return -1;
				break;
			default:
				e.what();
			}
		}
		catch (std::exception & e) {
			cout << e.what() << endl;
		}
	}
	return 0;
}

bool _Client::EstablishConnection() {
	///<summary> EstablishConnection() is responsible for the invoking ConnectToServer() whilst allowing for multiple retry's if ever unsuccessful.
	///<remark> EstablishConnection() attempts up to 50 times to connect to the server. This is useful in the event the Client was launched before the Server, and avoids unnecessary restarts.
	///<return> Returns true upon successful connection to the Server. Returns false after failing 50 times.
	int connectionTries = 0;
	bool connectionStatus = false;
	while (!connectionStatus && connectionTries < 50) {
		connectionTries++;
		cout << "Attempting to Connect to Server (Try " << connectionTries << " of 50)" << endl;
		connectionStatus = ConnectToServer();
		if (!connectionStatus) {
			cout << "Retrying in 5 seconds...." << endl;
			Sleep(5000);
		}
	}
	return connectionStatus;
}

void _Client::Run()
{
	///<summary> Run() executes the Client code.
	///<remarks> Run() is responsible for executing the Client code in the correct order of execution.
	///<remarks> Run() also continously waits for User input to send to the server.
	///<return> Even though Run has no return type, Run will exit if it encounters an error primarily in the setup phase.
	if (!EstablishConnection())
	{
		cout << "Max connection tries reached..." << endl;
		WSACleanup();
		return;
	}

	DWORD threadId;
	CreateThread(NULL, 0, RecvDat, (LPVOID)clientSocket, 0, &threadId);
	char buffer[200];
	while (1) {
		char clientMessage[200];
		strcpy_s(clientMessage, 200, username);
		cin.getline(buffer, 200);
		strncat_s(clientMessage, buffer, 200);
		if (strcmp(buffer, "QUIT") != 0) {
			try
			{
				int byteCount = Comms::SendData(clientSocket, clientMessage);
				if (byteCount < 0)
					throw(C_S_Exceptions(WSAGetLastError()));
			}
			catch (C_S_Exceptions e) {
				switch (e.ID) {
				case 10038:
					cout << "(10038) Failed to send message to server." << endl;
					break;
				default:
					e.what();
				}
				return;
			}
			catch (std::exception & e)
			{
				cout << e.what() << endl;
				return;
			}
		}
		else {
			return;
		}
	}
}


