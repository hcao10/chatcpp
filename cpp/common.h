// common.h
// 3/31/2022
// Haowei Cao

#pragma once
#include <string>
#include<iostream>
#include<list>
#include <math.h>
#include<vector>
//#include "refer/geohash.h"

using namespace std;
// TO DO
string L2Geohash(string alocation);
string GeohashMove(string geohash, int distance);


class group {
	group(string groupname, char grouptype, string userid) {
		gName = groupname;
		gType = grouptype;
		AddUser(userid);

	}
public:
	
	int gNumber;				// unique number for the position of group in the vector, a helper for using vector 
	string gName;				// group name
	char gType;					// group type i for interest; g for geographical group
	void ShowList();			// print the all users in the group
	void ShowOnlineList();		// print the all online users in the group
	void AddUser(string username);		// all user to the group
	void DelUser(string username);		// delete user from the group
	// Testing functions:
	//int FindUser(string username);			// look for a user return 0 for not in the group, 1 for exist
private:
	vector<string> userlist;		// store all users 


};

//TO DO
void group::ShowList() {

}
void group::ShowOnlineList() {

}
void group::AddUser(string username) {
	userlist.push_back(username);
}
void group::DelUser(string username) {

}

string L2Geohash(string alocation) {

	return alocation;
}

//TO DO
/*
 int Geohash2L(double &latitude, double &longitude, string aGeohash) {
	// call geohash_decode


}
*/

//TO DO
string GeohashMove(string geohash, int distance) {
	string ret;


	//TO DO
	ret = geohash;
	return ret;
}

