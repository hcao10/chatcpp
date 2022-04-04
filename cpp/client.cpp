// client.cpp
// 4/1/2022
// Haowei Cao

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
#include "client.h"
#include "common.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_PORT 5055
#define DATA_BUFFER 1024


int main()
{
	client aClient;
	if (aClient.Startup() == false)
		return 0;
	aClient.GetLocation();
	aClient.GetInterest();
	aClient.SetServerAddress();
	aClient.work();
}

bool client::Startup()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed to load Winsock." << endl;
		return false;
	}
	sClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
void client::SetServerAddress()
{
	cout << "Enter the server ip address " << endl;
	cout << "For localhost type 'local' or 'l'" << endl;
	string iptemp;
	cin >> iptemp;
	if (iptemp == "l" || iptemp == "local" || iptemp == "localhost" || iptemp == "1") {
		iptemp.clear();
		iptemp = "127.0.0.1";
	}
	char* ip = new char[iptemp.length() + 1];
	strcpy(ip, iptemp.c_str());
	// server address
	ser.sin_family = AF_INET;
	ser.sin_port = htons(DEFAULT_PORT);
	ser.sin_addr.s_addr = inet_addr(ip);
}
int client::GeneratePort()
{
	srand((unsigned)time(NULL));
	int x = 1024 + rand() % (5000 - 1024);
	return x;
}
bool client::Getonlinelist()            // get online list from server
{
	if (onlinelist.size() > 0)
		onlinelist.clear();
	char getonlinelist[3] = "PL";
	iSend = sendto(sClient, getonlinelist, 3, 0, (struct sockaddr*)&ser, ser_length);
	if (iSend == SOCKET_ERROR)
	{
		cout << "sendto()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	memset(recv_buf, 0, sizeof(recv_buf));
	iRecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&communication, &communication_length);   ///////////////////////////////////////////////////
	if (iRecv == SOCKET_ERROR)
	{
		cout << "recvfrom() Failed" << WSAGetLastError() << endl;
		return false;
	}
	string list(recv_buf);
	string friendname;
	for (int i = 0; i < list.length(); i++)
	{
		if (list[i] == '$')  break;
		else if (list[i] == '#')
		{
			onlinelist.push_back(friendname);
			friendname = "";
		}
		else
			friendname = friendname + list[i];
	}
	cout << "----------------------------" << endl;
	cout << "Online Users: " << endl;
	for (int i = 0; i < onlinelist.size(); i++)
		cout << i << ":  " << onlinelist[i] << endl;
	cout << "----------------------------" << endl;
	return true;
}

void client::work()
{
	while (true)
	{
		memset(recv_buf, 0, sizeof(recv_buf));
		system("cls");
		cout << "Menu options:" << endl;
		cout << "1 - Log on" << endl;
		cout << "2 - Sign up" << endl;
		cout << "3 - Log out" << endl;

		string choice;
		getline(cin, choice);
		if (choice == "1")
		{
			system("cls");
			cout << "Username: ";
			string username;
			getline(cin, username);
			cout << "Password: ";
			string password;
			getline(cin, password);
			// generate monitor port number
			receiver_port = GeneratePort();
			// store the port number to a file for the monitor
			ofstream out("port.txt");
			out << receiver_port << "\n" << username;
			out.close();

			string init_infortemp = "L" + username + "#" + password + "#" + to_string(receiver_port) +"#"+ cLocation +"#"+ cInterest +"#";
			char* init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			// authenticate
			iSend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			// receive message
			iRecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&communication, &communication_length);
			if (iRecv == SOCKET_ERROR)
			{
				cout << "recvfrom() Failed:" << GetLastError() << endl;
				cout << "Login failed" << endl << "No response from the server" << "Please enter Y to return to the home page: " << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "Login failed" << endl << "No response from the server" << "Please enter Y to return to the home page: " << endl;
				}
				continue;
			}
			if (recv_buf[0] == 'Y')   //login successful
			{
				system("cls");
				ShellExecute(NULL, _T("open"), _T("newterminal.exe"), NULL, NULL, SW_SHOW);     // call a new terminal
			}
			else if (recv_buf[0] == '0')
			{
				cout << "Login failed" << endl << "Unregistered user name" << endl << "please enter Y to return to the home page: " << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "Login failed" << endl << "Unregistered user name" << endl << "please enter Y to return to the home page: " << endl;
				}
				continue;
			}
			else if (recv_buf[0] == '1')
			{
				cout << "Login failed" << endl << "Unregistered user name" << endl << "please enter Y to return to the home page: " << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "Login failed" << endl << "Unregistered user name" << endl << "please enter Y to return to the home page: " << endl;
				}
				continue;
			}
			else if (recv_buf[0] == '2')
			{
				cout << "Login failed" << endl << "Signed in elsewhere" << endl << "Please enter Y to return to the home page: " << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "Login failed" << endl << "Signed in elsewhere" << endl << "Please enter Y to return to the home page: " << endl;
				}
				continue;
			}

			// Temp Options
			while (true)
			{
				system("cls");
				cout << "Menu options for " << username << endl;
				cout << "User: " << username << endl << endl;
				cout << "1 - Private chat" << endl;
				cout << "2 - Group chat" << endl;
				cout << "3 - Log out" << endl;
				cout << "Other commands starts with '/'" << endl;
				cout << "Type '/' to show all commands" << endl << endl;
				string mode;
				getline(cin, mode);
				if (mode == "1")    // private chat
				{
					system("cls");
					cout << "In private chat mode, enter return to the previous level" << endl << endl;
					if (!Getonlinelist())    continue;                           //  Failed to get friend online list
					cout << "Please select the serial number of the private chat partner" << endl;
					string choose;
					getline(cin, choose);
					if (choose == "return") continue;
					int i = 0;
					for (i = 0; i < choose.size(); i++)
						if (choose[i] > '9' || choose[i] < '0')break;
					if (i < choose.size()) continue;
					stringstream stream(choose);
					int index = 0;
					stream >> index;
					if (index < 0 || index >= onlinelist.size()) continue;
					while (true)                     // send messages to the user in a loop until return is entered to exit
					{
						system("cls");
						cout << "Chatting with " << onlinelist[index] << " in private chat mode" << "£¬enter return to the previous level" << endl << endl;
						cout << "To " << onlinelist[index] << " : ";
						string message;
						getline(cin, message);
						if (message == "return")
						{
							system("cls");
							break;
						}
						message = "PM" + onlinelist[index] + "#" + message;
						char* buf = new char[message.length() + 1];
						strcpy(buf, message.c_str());
						iSend = sendto(sClient, buf, message.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
						if (iSend == SOCKET_ERROR)
						{
							cout << "sendto()Failed:" << WSAGetLastError() << endl;
							break;
						}
						delete[]buf;
						iRecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&communication, &communication_length);
						if (recv_buf[0] == 'Y') continue;
						else
						{
							cout << onlinelist[index] << " offline " << "Please enter Y to return to the home page: " << endl;
							string ret;
							while (getline(cin, ret))
							{
								if (ret == "Y") break;
								cout << onlinelist[index] << " offline " << "Please enter Y to return to the home page: " << endl;
							}
							break;
						}
					}
				}
				else if (mode == "2")    // public group chat for testing
				{
					system("cls");
					while (true)
					{
						system("cls");
						cout << "In Group Chat. Please enter 'return' to the home page: " << endl << endl;
						cout << "To all: ";
						string message;
						getline(cin, message);
						if (message == "return")
						{
							system("cls");
							break;
						}

						message = "G" + message;
						char* buf = new char[message.length() + 1];
						strcpy(buf, message.c_str());
						iSend = sendto(sClient, buf, message.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
						delete[]buf;
						if (iSend == SOCKET_ERROR)
						{
							cout << "sendto()Failed:" << WSAGetLastError() << endl;
							break;
						}
					}
					continue;
				}
				else if (mode == "3")                             // log out
				{
					char buf[] = "exit";
					iSend = sendto(sClient, buf, sizeof(buf), 0, (struct sockaddr*)&ser, ser_length);
					break;
				}
				else if (mode == "/") 
				{
					system("cls");
					cout << "Other commands for " << username << endl;
					cout << "User: " << username << endl << endl;
					cout << "- /list-users" << endl;
					cout << "- /send-group-message <msg>" << endl;
					cout << "- /send-private-message <userid> <msg>" << endl;
					cout << "- /list-geo                // displays the current geo location" << endl;
					cout << "- /join-geo <distance>     // joins geo based chatting within the specified distance" << endl;
					cout << "- /join-interest           // joins interest based group" << endl;
					cout << "- /status                  // shows whether a user has joined an interest based or geo based chat and a list of users in the current group" << endl << endl;
					string message;
					getline(cin, message);
					if (SlashCommand(message) == true) {
						continue;
					}
					else {
						break;
					}

				}
				else if (mode.at(0) == '/') 
				{
					SlashCommand(mode);
				}
				else
					continue;
			}
		}
		else if (choice == "2")
		{
			system("cls");
			cout << "Enter username: ";
			string username;
			getline(cin, username);
			cout << "Password: ";
			string password;
			getline(cin, password);
			string init_infortemp = "R" + username + "#" + password;
			char* init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			// send user info to server
			iSend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			// receive user info from server
			iRecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&communication, &communication_length);
			if (recv_buf[0] == 'Y')
			{
				cout << "Registered successfully. " << endl;
				continue;
			}
			else
			{
				cout << "Login failed" << endl << "Username already exists" << endl << "Please enter Y to return to the home page: " << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "Login failed" << endl << "Username already exists" << endl << "Please enter Y to return to the home page: " << endl;
				}
				continue;
			}
		}
		else if (choice == "3")
		{
			closesocket(sClient);
			WSACleanup;
			return;
		}
		else
			continue;
	}
}

void client::GetLocation() 
{
	// TO DO
	// https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.Geolocation?view=winrt-22000
	cout << " Obtaining geographical location" << endl << endl;
	// TEMP:
	cout << "Working on Geographical location portion" << endl << endl;
	cout << "Enter an address, (latitude, longitude) or a Geohash" << endl;	// http://geohash.co/
	cout << "e.g. Gehash '9vk1n6mgm0qt' stands for '29.719951, -95.342222'" << endl;
	cout << "or enter 'example' using example location: " << endl;
	getline(cin, cLocation);

	if (cLocation == "example") {
		cLocation.clear();
		cLocation = "9vk1n6mgm0qt";
		// Test
		// double latitude, longitude;
		//Geohash2L(latitude, longitude, gLocation);
	}
	// if user enter a latitude and longtitude
	else if (cLocation.find(',') == string::npos || cLocation.find('.') == string::npos || cLocation.find('(') == string::npos || cLocation.find(')') == string::npos || cLocation.find('-') == string::npos) {
		cLocation = L2Geohash(cLocation);		// get Geohash
	}
	system("cls");
	cout << "Your current location is " << cLocation << " (in Geohash) " << endl;
	cout << "After you have successfully logged in, you will automatically be added to the location-based group." << endl << endl;


}


void client::GetInterest() 
{
	cout << "Enter your interest: ";
	getline(cin, cInterest);
	// fetching group
	system("cls");
	cout << "Your current location is " << cLocation << "( in Geohash) " << endl << endl;
	cout << "Your interest is " << cInterest << endl;
	cout << "After you have successfully logged in, you will join the new interest group." << endl << endl;

}



string client::locationHelper() 
{
	return cLocation;
}
string client::interestHelper() 
{
	return cInterest;
}
bool client::ListUsers() 
{
	system("cls");
	cout << "All online users" << endl << endl;
	if (!Getonlinelist())    return true;                           // Failed to get friend online list
	cout << "Please select the serial number of the private chat partner" << endl;
	cout << "or enter return to the previous level" << endl << endl;
	string choose;
	getline(cin, choose);
	if (choose == "return") return true;
	int i = 0;
	for (i = 0; i < choose.size(); i++)
		if (choose[i] > '9' || choose[i] < '0')break;
	if (i < choose.size()) return true;
	stringstream stream(choose);
	int index = 0;
	stream >> index;
	if (index < 0 || index >= onlinelist.size()) return true;
	while (true)                     //  send messages to the user in a loop until return is entered to exit
	{
		system("cls");
		cout << "Chatting with " << onlinelist[index] << " in private chat mode" << "£¬enter return to the previous level" << endl << endl;
		cout << "To " << onlinelist[index] << " : ";
		string message;
		getline(cin, message);
		if (message == "return")
		{
			system("cls");
			break;
		}
		message = "PM" + onlinelist[index] + "#" + message;
		char* buf = new char[message.length() + 1];
		strcpy(buf, message.c_str());
		iSend = sendto(sClient, buf, message.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
		if (iSend == SOCKET_ERROR)
		{
			cout << "sendto()Failed:" << WSAGetLastError() << endl;
			break;
		}
		delete[]buf;
		iRecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&communication, &communication_length);
		if (recv_buf[0] == 'Y') continue;
		else
		{
			cout << onlinelist[index] << " offline " << "Please enter Y to return to the home page: " << endl;
			string message;
			string ret;
			while (getline(cin, ret))
			{
				if (ret == "Y") continue;
				cout << onlinelist[index] << " offline " << "Please enter Y to return to the home page: " << endl;
				string message;
			}
			return true;
		}
	}
}

//TO DO

bool client::SlashCommand(string sCommand) 
{
	/*
	- /list-users
	- /send-group-message <msg>
	- /send-private-message <userid> <msg>
	- /list-geo // displays the current geo location
	- /join-geo <distance>  // joins geo based chatting within the specified distance
	- /join-interest // joins interest based group
	- /status // shows whether a user has joined an interest based or geo based chat and a list of users in the current group
	*/

	/*

	*/
	if ((sCommand == "/list-users") || ((sCommand.find("list") != string::npos) && (sCommand.find("users") != string::npos))) {
		if (ListUsers() == true) {
			return true;
		}
		else return false;
	}

}
void client::GetUserIndex() 
{

}
bool client::SendGroupMessage(string groupName, string msm) 
{
	return true;
}
bool client::sendPrivateMessage(string username, string msm) 
{
	return true;
}
bool client::listGeo() 
{
	return true;
}
bool client::joinGeo(int distance) 
{
	return true;
}
bool client::joinInterest() 
{
	return true;
}
bool client::status() 
{
	return true;
}
