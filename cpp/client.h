// client.h
// 4/1/2022
// Haowei Cao

#pragma once
#include<Winsock2.h>
#include<iostream>
#include<string>
#include<ctime>
#include<tchar.h>
#include<Windows.h>
#include<fstream>
#include<vector>
#include <sstream>
#include<list>
// #include "sender.h"
#include "common.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_PORT 5055
#define DATA_BUFFER 1024

class client
{
public:
	bool Startup();
	void SetServerAddress();
	int GeneratePort();               // randomly generate monitor port number
	bool Getonlinelist();             // get online username
	void work();                      // main function
	void GetLocation();				  // get geographical location
	void GetInterest();					// get client interest
	void GetUserIndex();				// get user index from whole user list
	bool SlashCommand(string);			// slash commands
	string locationHelper();
	string interestHelper();
	string cLocation;
	string cInterest;
	// slash commands:  
	// To Do
	// temporary parameters, may be undated later
	bool ListUsers();					// call Getonlinelist()
	bool SendGroupMessage(string groupName, string msm);
	bool sendPrivateMessage(string username, string msm);
	bool listGeo();
	bool joinGeo(int distance);
	bool joinInterest();
	bool status();

private:
	int iSend, iRecv;
	string geoHash;
	int geoScale;			// store current geoHash scale (the current length of the geohash)
	WSADATA wsaData;
	SOCKET sClient;                       // sockets for sending and receiving information
	struct sockaddr_in ser;               // address of the server
	int ser_length = sizeof(ser);
	struct sockaddr_in communication;
	int communication_length = sizeof(communication);
	char recv_buf[DATA_BUFFER];           // buffer for receiving information
	int receiver_port;                    // port number of the monitor
	vector<string> onlinelist;            // usernames of online users
	vector<group>groupList;					// client buffer for list of groups

};
