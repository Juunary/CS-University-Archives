// PlayList.h
#pragma once
#include "PlayListNode.h"
#include <string>

using namespace std;

class PlayList {
private:
	PlayListNode* head; 
	PlayListNode* cursor;
	int count;
	int time; 
	string data;

    int stringToSec(const string& run_time);
    string secToString(int seconds);

public:
	PlayList();
	~PlayList();


	void insert_node(const string& artist, const string& title, const string& run_time);
	void delete_node(const string& artist, const string& title, ofstream& flog);
	bool empty();
	bool full();
	bool exist(const string& artist, const string& title);
	string print(ofstream& flog);
	int run_time(); 
	int getCount();
};