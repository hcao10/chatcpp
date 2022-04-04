// Server.cpp
// 3/31/2022
// Haowei Cao

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
#include "server.h"
#include "common.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define DEFAULT_PORT 5055
#define BUFFER_LENGTH 1024

int main()
{
	server myServer;
	if (myServer.Startup() == false)
		return 0;
	if (myServer.SetServerSocket() == false)
		return 0;
	myServer.work();
	return 0;
}


// check the environment
bool server::Startup()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed to load Winsock." << endl;
		return false;
	}
	return true;
}

// sets the socket to listen
bool server::SetServerSocket()
{
	// socket for server
	sSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sSocket == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	// server address
	ser.sin_family = AF_INET;
	ser.sin_port = htons(DEFAULT_PORT);               // converts a u_short from host to TCP/IP network byte order (which is big-endian)
	ser.sin_addr.s_addr = htonl(INADDR_ANY);          // converts a u_long from host to TCP/IP network byte order (which is big-endian)   
	if (bind(sSocket, (LPSOCKADDR)&ser, sizeof(ser)) == SOCKET_ERROR)
	{
		cout << "bind()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
void server::SendMessage(string message, struct sockaddr_in x)
{
	char* send_buf = new char[message.length() + 1];
	strcpy(send_buf, message.c_str());
	SOCKET rSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (rSocket == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return;
	}
	iSend = sendto(rSocket, send_buf, message.length() + 1, 0, (SOCKADDR*)&(x), sizeof(x));
	if (iSend == SOCKET_ERROR)
	{
		cout << "sendto failed:" << WSAGetLastError() << endl;
		closesocket(rSocket);
		return;
	}
	closesocket(rSocket);
}
void server::Sendonlinelist()
{
	string onlinelist;
	for (int i = 0; i < usertable.size(); i++)
		onlinelist = onlinelist + usertable[i].username + "#";
	onlinelist = onlinelist + "$";                // ending
	SendMessage(onlinelist, cli);
}
bool server::TestUsernameAndPassword(string username, string password, int& flag)
{
	if (!Checktxt())
	{
		cout << "Cannot retrieve data file error 001" << endl << endl;
		flag = 0;                                 // set flag: username not found 
		return false;
	}
	fstream in("C:\\userform\\userform.txt");
	string line;
	string username_txt, password_txt;
	while (getline(in, line))
	{
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == '#')
			{
				username_txt = line.substr(0, i);
				password_txt = line.substr(i + 1);
				break;
			}
		}
		if (username_txt == username)         // find username
		{
			if (password == password_txt)     // check password
			{
				in.close();
				return true;                  // verification success
			}

			//Time:
			cout << GetCurrentTime();
			cout << "Login failed flag" << endl;
			cout << "Password incorrect" << endl;
			cout << "User id: " << username << endl << endl;			
			flag = 1;                        //  set flag: password incorrect
			return false;
		}
	}
	in.close();
	cout << GetCurrentTime();
	cout << "Login failed flag" << endl;
	cout << "Not in the databas" << endl;
	cout << "User id: " << username << endl << endl;
	flag = 0;                                 //set flag: username not found
	return false;
}
bool server::TestDuplicateLogin(string username)
{
	int i;
	for (i = 0; i < usertable.size(); i++)
		if (usertable[i].username == username) break;
	if (i == usertable.size())     // user has not logged in
		return false;
	else
	{
		cout << GetCurrentTime() << endl;
		cout << "Login failed flag" << endl;
		cout << "User status error: logged in" << endl;
		cout << "User id: " << username << endl << endl;
		return true;
	}
}
bool server::TestDuplicateRigister(string username)
{
	if (!Checktxt())
	{
		cout << "Cannot retrieve data file error 002" << endl << endl;
		return true;
	}
	fstream in("C:\\userform\\userform.txt");
	string line;
	while (getline(in, line))
	{
		string username_txt;
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == '#')
			{
				username_txt = line.substr(0, i);              // get username
				if (username_txt == username)                  //uniqueness detection,check if it has been registered
				{
					in.close();
					cout << GetCurrentTime();
					cout << "Registration failed flag" << endl;
					cout << "Username already exists" << endl;
					cout << "User id: " << username << endl << endl;
					return true;
				}
				break;                                         // exit loop
			}
		}
	}
	in.close();
	return false;                                               // user has not been registered
}
string server::Getusername(string ip, int port)
{
	for (int i = 0; i < usertable.size(); i++)
		if (usertable[i].ip == ip && usertable[i].sender_port == port)
			return usertable[i].username;
	cout << GetCurrentTime();
	cout << "!! Illegal user connected to the server !!"<< endl;
	cout << "ip: " << ip << " : " << port << endl<<endl;
	return "";
}

// return a string of current time
string server::GetCurrentTime() {
	time_t now = time(0);			// get time from system
	char* dt = ctime(&now);
	string currentTime = dt;
	return currentTime;
}
int server::Getuserindex(string username)
{
	int i = 0;
	for (i = 0; i < usertable.size(); i++)
		if (usertable[i].username == username) break;
	return i;
}
void server::extractLoginuserinfor(string userinfor, string& username, string& password, string& receiverport , string& location, string& interest)
{
	unsigned int i;
	vector<string> initUser;
	for (i = 0; i < userinfor.length(); i++)           // get user name
	{
		if (userinfor[i] == '#')
		{
			username = userinfor.substr(0, i);
			break;
		}
	}
	for (unsigned int j = i + 1; j < userinfor.length(); j++)  // get other user info
	{
		if (userinfor[j] == '#')
		{

			initUser.push_back(userinfor.substr(i + 1, j - i - 1));
			i = j;
			
		}
		


	}
	if (initUser.size() >= 3) {
		password = initUser.at(0);
		receiverport = initUser.at(1);
		location = initUser.at(2);
		interest = initUser.at(3);
	}
	
}
void server::extractRegisteruserinfor(string userinfor, string& username, string& password)
{
	for (int i = 0; i < userinfor.size(); i++)
	{
		if (userinfor[i] == '#')
		{
			username = userinfor.substr(0, i);
			password = userinfor.substr(i + 1);
			break;
		}
	}
}
void server::extactPersonalMessageReceivername(string& message, string& receivername)
{
	for (int i = 0; i < message.size(); i++)
	{
		if (message[i] == '#')
		{
			receivername = message.substr(0, i);
			message = message.substr(i + 1);
			break;
		}
	}
}
bool server::Checktxt()
{
	FILE* fp = fopen("C:\\userform\\userform.txt", "r");
	if (fp == NULL)
	{
		system("md C:\\userform");
		ofstream out("C:\\userform\\userform.txt");
		if (!out)
			return false;
		out.close();
		return true;
	}
	return true;
}

// TO Do
void server::groupInit(string username, string groupid, char groupType) 
{
	if (allGroups.size() == 0){
		group aGroup( groupid,  groupType, username);
		allGroups.push_back(aGroup);
	}
	// look for the group in the allGroups

} 	// create groups
void server::work()
{
	cout << "     Welcome   " << endl;
	cout << "Server is running..." << endl << GetCurrentTime() << endl << endl;
		;
	while (true)                                       // data transmission
	{
		memset(recv_buf, 0, sizeof(recv_buf));         // initialize receive-buffer
		iRecv = recvfrom(sSocket, recv_buf, BUFFER_LENGTH, 0, (struct sockaddr*)&cli, &cli_length);
		if (iRecv == SOCKET_ERROR)
		{
			cout << "recvfrom()Failed:" << WSAGetLastError() << endl;
			continue;
		}

		// sender's ip & port
		char* x = inet_ntoa(cli.sin_addr); string address(x);         // client address
		int userport = ntohs(cli.sin_port);                           // client port

		string infortype = string(recv_buf);                            // determine the type of the message according to informtype[0]       
		if (infortype[0] == 'L')                                      // login request
		{
			string userinfor = infortype.substr(1);                   // remove message type
			string username, password, receiver_port;
			string clientLocation, clientInterest;
			extractLoginuserinfor(userinfor, username, password, receiver_port, clientLocation, clientInterest);  //  get user info

			cout << endl;
			cout << "Location: " << clientLocation << endl << "Interest: " << clientInterest << endl;
																				  // login failure flag to illegal user
			int flag = 0;
			if (!TestUsernameAndPassword(username, password, flag))
			{
				if (flag == 0)
					SendMessage("0", cli);
				if (flag == 1)
					SendMessage("1", cli);
				continue;
			}
			// check the user status
			if (TestDuplicateLogin(username))
			{
				SendMessage("2", cli);
				continue;
			}
			// add online users to the list
			int receiver_port_int = atoi(receiver_port.c_str());
			user newuser(username, address, userport, receiver_port_int, clientLocation, clientInterest);
			usertable.push_back(newuser);

			printmessage = GetCurrentTime() + "[Online notification] Userid: [ " + newuser.username + " ] is online now";               // message to be printed
			sendmessage = printmessage;                                           // message to be forwarded
			SendMessage("Y", cli);                                                // send the client a successful login response
		}
		else if (infortype[0] == 'R')                 // registration info
		{
			string userinfor = infortype.substr(1);                  // remove message type		
			string username, password;
			extractRegisteruserinfor(userinfor, username, password); // extract username and password

																	 //check if the username is already registered
			if (TestDuplicateRigister(username))
			{
				SendMessage("N", cli);
				continue;
			}
			// store the new username and password to the file
			if (!Checktxt())
			{
				SendMessage("N", cli);
				continue;
			}
			fstream out("C:\\userform\\userform.txt", ios::app);
			out << userinfor << endl;
			out.close();
			// send a successful registration response
			SendMessage("Y", cli);
			cout << "New User Registered Successfully" << endl;
			cout << "User name is: " << username << endl << endl;
			continue;
		}
		else if (infortype[0] == 'G')                                          // public group msm
		{
			string message = infortype.substr(1);
			string sendername = Getusername(address, userport);                  // get the sender name
			if (sendername == "")   continue;
			printmessage = GetCurrentTime() + sendername + " [group placeholder] " + ":" + message;
			sendmessage = printmessage;
			//sendmessage = "G#"+sendername + ":" + message;                       
		}
		else if (infortype[0] == 'P')                     // private message
		{
			if (infortype[1] == 'L')                      // Request online friends list
			{
				Sendonlinelist();
				continue;
			}
			if (infortype[1] == 'M')                      // private message
			{
				string message = infortype.substr(2);
				string sendername = Getusername(address, userport);  // get the sender name
				if (sendername == "")  continue;
				//get the receiver name
				string receivername;
				extactPersonalMessageReceivername(message, receivername);
				// check if the receiver is offline
				int i = Getuserindex(receivername);
				if (i == usertable.size())                              // receiver is offline
				{
					SendMessage("N", cli);
					continue;
				}
				SendMessage("Y", cli);                                  // Send a successful response to the sender
				printmessage = GetCurrentTime() + sendername + " -> " + receivername + " : " + message;               // set the message to be printed
				cout << printmessage << endl;
				cout << "User ip:" << address << " :" << userport << endl;
				cout << "Current online No: " << usertable.size() << endl << endl;
				sendmessage = printmessage;                                                                 
				SendMessage(sendmessage, usertable[i].receiver);
				if (sendername != receivername)
				{
					int j = Getuserindex(sendername);
					SendMessage(sendmessage, usertable[j].receiver);
				}
				continue;
			}
		}
		// TO DO
		// add more commands
		else if (infortype == "exit")
		{
			string sendername = Getusername(address, userport);
			if (sendername == "") continue;
			int i = Getuserindex(sendername);
			if (i >= usertable.size() || i < 0) continue;
			SendMessage("exit", usertable[i].receiver);                                 // Send an exit command to this user terminal
			usertable.erase(usertable.begin() + i);
			printmessage = GetCurrentTime() + "[Offline notification] Userid:[ " + sendername + " ] is logged out ";                         // set the message to be printed
			sendmessage = printmessage;                                                  // set the message to be forwarded

		}
		// print on server		
		cout << printmessage << endl;
		cout << "User ip:" << address << " : " << userport << endl;
		cout << "Total number of people currently online: " << usertable.size() << endl << endl;
		// send msm to client
		for (int i = 0; i < usertable.size(); i++)
			SendMessage(sendmessage, usertable[i].receiver);
	}
}

