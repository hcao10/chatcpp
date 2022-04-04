// newterminal.cpp
// 3/31/2022
// Haowei Cao

#include<WinSock2.h>
#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#define DEFAULT_SPORT 5055
#define DEFAULT_CPORT 5056
#define BUFFER_LENGTH 1024

void main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed to load Winsock." << endl;
		return;
	}
	// create terminal for monitoring
	struct sockaddr_in receiver;
	// get port number for 
	ifstream in("port.txt");
	string receiver_port;
	string username;
	getline(in, receiver_port);
	getline(in, username);
	in.close();
	remove("port.txt");
	int receiver_port_int = atoi(receiver_port.c_str());

	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(receiver_port_int);    // converts a u_short from host to TCP/IP network byte order (which is big-endian)
	receiver.sin_addr.s_addr = htonl(INADDR_ANY);    // converts a u_long from host to TCP/IP network byte order (which is big-endian)  
	SOCKET rSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (rSocket == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return;
	}
	if (bind(rSocket, (LPSOCKADDR)&receiver, sizeof(receiver)) == SOCKET_ERROR)
	{
		cout << "bind()Failed:" << WSAGetLastError() << endl;
		return;
	}

	char recv_buf[BUFFER_LENGTH];                  // buffer to receive data
	memset(recv_buf, 0, sizeof(recv_buf));         // initialize receive-buffer

	struct sockaddr_in ser;                        // client address
	int ser_length = sizeof(ser);                  // client address length


	cout << "----------------------------------------" << endl << endl;
	cout << "Current User: " << username << endl << endl << endl;

	while (true)                                   // data reception and transmission
	{
		int iRecv = recvfrom(rSocket, recv_buf, BUFFER_LENGTH, 0, (SOCKADDR*)&ser, &ser_length);
		string transmessage(recv_buf);
		if (iRecv == SOCKET_ERROR)
		{
			cout << "recvfrom()Failed:" << WSAGetLastError() << endl;
			break;
		}
		else if (transmessage == "exit") break;
		else
			cout << transmessage << endl;
	}
	closesocket(rSocket);
	WSACleanup();
}