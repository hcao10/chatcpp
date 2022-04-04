// Server.h
// 3/31/2022
// Haowei Cao

#pragma once
#include<WinSock2.h>	// https://docs.microsoft.com/en-us/windows/win32/networking
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cstdlib>
#include <ws2tcpip.h>
#include <stdio.h>
#include <ctime>
#include <list>
#include <map>
#include "common.h"

#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define DEFAULT_PORT 5055
#define BUFFER_LENGTH 1024
class user
{
public:
	user(string username, string ip, int sender_port, int receiver_port, string uGeohash, string uInterest)
	{
		this->username = username;
		this->ip = ip;
		this->sender_port = sender_port;
		this->receiver_port = receiver_port;
		this->uGeohash = uGeohash;
		this->uInterest = uInterest;
		// Set address:
		receiver.sin_family = AF_INET;
		receiver.sin_port = htons(receiver_port);
		char* addr = new char[ip.length() + 1];
		strcpy(addr, ip.c_str());
		receiver.sin_addr.s_addr = inet_addr(addr);
	}
	string uGeohash;				// geographical info in Geohash
	string uInterest;				// user interest
	string username;				// user name
	string ip;						// client ip
	int sender_port;
	int receiver_port;
	struct sockaddr_in receiver;   // store address

	// TO DO
	// void joinGroup(string groupID);

private:
	vector<string> groupList;		// list for groups joined
};
class server
{
public:
	string tempGeo;													// helper for temp geo
	string tempInterest;											// helper for temp interest
	bool Startup();                                                   // check the environment
	bool SetServerSocket();                                           // sets the socket to listen
	bool Checktxt();                                                  // check if the storage file exists, if not, create one
	void work();                                                      // main function for server
	void SendMessage(string message, struct sockaddr_in x);           // send helper
	void Sendonlinelist();                                            // report the onine user list
	bool TestUsernameAndPassword(string username, string password, int& flag);   // authentication  
	bool TestDuplicateLogin(string username);                         // username uniqueness detection
	bool TestDuplicateRigister(string username);                      // uniqueness detection
	string Getusername(string ip, int port);                           // username helper by ip
	int  Getuserindex(string username);                               // online user helper by username	
	void extractLoginuserinfor(string userinfor, string& username, string& password, string& receiverport, string& location, string& interest); // extract username password, monitor port number, location and interest from login request
	void extractRegisteruserinfor(string userinfor, string& username, string& password);                       // extract username password and monitor port number from register request
	void extactPersonalMessageReceivername(string& message, string& receivername);                           // extract recipient's name from private chat message
	string GetCurrentTime();								  // current time
	void groupInit(string username, string groupNmae, char groupType); 	// create groups
private:
	WSADATA wsaData;
	SOCKET sSocket;                            // socket to receive messages
	struct sockaddr_in ser;                    // server address
	struct sockaddr_in cli;                    // client address
	int cli_length = sizeof(cli);              //length of client address
	char recv_buf[BUFFER_LENGTH];              // buffer to receive data
	vector<user> usertable;                    // online User List
	string sendmessage;							// string  to forward 
	string printmessage;						// string to print
	int iSend, iRecv;                          // length of the string sent and received by the server

	// TO DO
	// Potential optimization
	// implement better data structure
	list<group> allGroups;
	map<string, string > geoGroupMap;					// geographical  group map< username, groupname>
	 map<std::string , string > intGrouplist;										// interest group map<username, groupname>
};

