#include "mySocket.h"
#include <iostream>
#include <cstring>

using namespace std;

// init winsock
#pragma comment(lib, "ws2_32.lib")

MySocket::MySocket(SocketType socketType, string ipAddress, unsigned int portNumber,
    ConnectionType connType, unsigned int bufferSize) {

    mySocket = socketType;
    IPAddr = ipAddress;
    port = portNumber;
    connectionType = connType;
    bTCPConnect = false;
    maxSize = (bufferSize > 0) ? bufferSize : DEFAULT_SIZE;

    // buffer
    Buffer = new char[maxSize];

    //init winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return;
    }

    // create socket based on connection type
    if (connectionType == ConnectionType::TCP) {
        welcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (welcomeSocket == INVALID_SOCKET) {
            cout << "TCP socket creation failed" << endl;
            WSACleanup();
            return;
        }
    }
    else {
        welcomeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (welcomeSocket == INVALID_SOCKET) {
            cout << "UDP socket creation failed" << endl;
            WSACleanup();
            return;
        }
    }

    // setup server address
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);

    // bind socket iof server
    if (mySocket == SocketType::SERVER) {
        if (bind(welcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            cout << "socket binding failed" << endl;
            closesocket(welcomeSocket);
            WSACleanup();
            return;
        }

        // listen mode for tcp server
        if (connectionType == ConnectionType::TCP) {
            if (listen(welcomeSocket, 1) == SOCKET_ERROR) {
                cout << "socket listening failed" << endl;
                closesocket(welcomeSocket);
                WSACleanup();
                return;
            }
        }
    }
}

MySocket::~MySocket() {
    if (bTCPConnect) {
        DisconnectTCP();
    }
    closesocket(welcomeSocket);
    delete[] Buffer;
    WSACleanup();
}

void MySocket::ConnectTCP() {
    if (connectionType != ConnectionType::TCP) {
        cout << "cannot initiate connection-oriented scenario using UDP socket" << endl;
        return;
    }

    if (mySocket == SocketType::CLIENT) {
        if (connect(welcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            cout << "TCP connection failed" << endl;
            return;
        }
        connectionSocket = welcomeSocket;
    }
    else {
        // server accepts connection
        int addrLen = sizeof(SvrAddr);
        connectionSocket = accept(welcomeSocket, (struct sockaddr*)&SvrAddr, &addrLen);
        if (connectionSocket == INVALID_SOCKET) {
            cout << "TCP accept failed" << endl;
            return;
        }
    }
    bTCPConnect = true;
}

void MySocket::DisconnectTCP() {
    if (connectionType != ConnectionType::TCP) {
        cout << "Ccnnot disconnect UDP socket" << endl;
        return;
    }

    if (bTCPConnect) {
        if (mySocket == SocketType::SERVER) {
            closesocket(connectionSocket);
        }
        bTCPConnect = false;
    }
}

void MySocket::SendData(const char* data, int bytes) {
    if (bytes > maxSize) {
        cout << "buffer is too small for data" << endl;
        return;
    }

    int bytesSent;
    if (connectionType == ConnectionType::TCP) {
        if (!bTCPConnect) {
            cout << "TCP connection not established" << endl;
            return;
        }
        bytesSent = send(connectionSocket, data, bytes, 0);
    }
    else {
        bytesSent = sendto(welcomeSocket, data, bytes, 0,
            (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
    }

    if (bytesSent == SOCKET_ERROR) {
        cout << "send failed" << endl;
        return;
    }
}

int MySocket::GetData(char* data) {
    int bytesReceived;
    if (connectionType == ConnectionType::TCP) {
        if (!bTCPConnect) {
            cout << "TCP connection not established" << endl;
            return -1;
        }
        bytesReceived = recv(connectionSocket, Buffer, maxSize, 0);
    }
    else {
        int addrLen = sizeof(SvrAddr);
        bytesReceived = recvfrom(welcomeSocket, Buffer, maxSize, 0,
            (struct sockaddr*)&SvrAddr, &addrLen);
    }

    if (bytesReceived == SOCKET_ERROR) {
        cout << "Receive failed" << endl;
        return -1;
    }

    if (bytesReceived > 0) {
        memcpy(data, Buffer, bytesReceived);
    }

    return bytesReceived;
}

string MySocket::GetIPAddr() {
    return IPAddr;
}

void MySocket::SetPort(int portNumber) {
    if (bTCPConnect) {
        cout << "cannot change port while connected" << endl;
        return;
    }
    port = portNumber;
    SvrAddr.sin_port = htons(port);
}

int MySocket::GetPort() {
    return port;
}

SocketType MySocket::GetType() {
    return mySocket;
}

void MySocket::SetType(SocketType socketType) {
    if (bTCPConnect || (mySocket == SocketType::SERVER && welcomeSocket != INVALID_SOCKET)) {
        cout << "cannot change socket type while connected" << endl;
        return;
    }
    mySocket = socketType;
}
