#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>


//global defines
enum class SocketType { CLIENT, SERVER };
enum class ConnectionType { TCP, UDP };
const int DEFAULT_SIZE = 1024;

class MySocket {
	char* Buffer;
	SOCKET welcomeSocket;
	SOCKET connectionSocket;
	struct sockaddr_in SvrAddr;
	SocketType mySocket;
	std::string IPAddr;
	int port;
	ConnectionType connectionType;
	bool bTCPConnect;
	int maxSize;

public:
	//constructor
	MySocket(SocketType socketType, std::string ipAddress, unsigned int portNumber, ConnectionType connectionType, unsigned int bufferSize = DEFAULT_SIZE);

	//destructor
	~MySocket();

	//connect TCP/IP connection
	void ConnectTCP();

	//disconnect TCP/IP
	void DisconnectTCP();

	//send data
	void SendData(const char* data, int bytes);

	//get data
	int GetData(char* data);

	//return ip address within mysocket obj
	std::string GetIPAddr();

	//set porty
	void SetPort(int port);

	//get port
	int GetPort();

	//get Sokcet Type
	SocketType GetType();

	//set socket type
	void SetType(SocketType socketType);


};