// TitleBST.h
#pragma once
#include "TitleBSTNode.h"
#include "PlayList.h"
#include <string>
#include <fstream>

class PlayList; 

using namespace std;
class TitleBST {
private:
	TitleBSTNode* root;
	TitleBSTNode* parent;
	string data;
	TitleBSTNode* target;
	PlayList pl;
	

public:
	TitleBST();
	~TitleBST();
	
	void insert(const string& title, const string& artist, const string& run_time);
    void search(const string& title, ofstream& flog);
    int searchCount(const string& title);
	void MAKEPL(const string& title, PlayList& pl);
	void print(ofstream& flog);
	void print_recursive(TitleBSTNode* node, ofstream& flog);
    void delete_node(const string& artist, const string& title, ofstream& flog);
	void delete_title(const string& title, ofstream& flog);
	void delete_artist(const string& artist, ofstream& flog);
	void delete_artist_recursive(TitleBSTNode* node, const string& artist, ofstream& flog);
	void delete_song(const string& artist, const string& title, ofstream& flog);
	void destroy_recursive(TitleBSTNode* node);
};