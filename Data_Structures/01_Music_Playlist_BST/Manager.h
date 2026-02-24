// Manager.h
#pragma once
#include "MusicQueue.h"
#include "TitleBST.h"
#include "ArtistBST.h"
#include "PlayList.h"
#include <fstream>
#include <string>

using namespace std;

class Manager {
private:
	MusicQueue q;
	ArtistBST ab;
	TitleBST tb;
	PlayList pl;
	ifstream fcmd;
	ofstream flog;
public:
	Manager();
	~Manager();

	void run(const char* command);

	void LOAD();
    void ADD(const string& args);
    void QPOP();
    void SEARCH(const string& args);
    void MAKEPL(const string& args);
    void PRINT(const string& args);
    void DELETE(const string& args);
    void EXIT();

};