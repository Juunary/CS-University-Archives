// ArtistBST.h
#pragma once
#include "ArtistBSTNode.h"
#include <string>
#include <fstream>
#include "PlayList.h"

using namespace std;
class PlayList;
class ArtistBST {
private:
	ArtistBSTNode* root;
	ArtistBSTNode* parent;
	string data;
	ArtistBSTNode* target;
	PlayList pl;

public:
	ArtistBST();
	~ArtistBST();
	void destroy_tree(ArtistBSTNode* node);
	void insert(const string& artist, const string& title, const string& run_time);
    void search(const string& artist, ofstream& flog);
	int searchCount(const string& artist);
	void MAKEPL(const string& artist, PlayList& pl);
    void print(ofstream& flog); 
    void print_recursive(ArtistBSTNode* node, ofstream& flog);
	void delete_node(const string& artist, const string& title, ofstream& flog);
	void delete_artist(const string& artist, ofstream& flog);
	void delete_title(const string& title, ofstream& flog);
	void delete_title_recursive(ArtistBSTNode* node, const string& title, ofstream& flog);
	void delete_song(const string& artist, const string& title, ofstream& flog);
    void destroy_recursive(ArtistBSTNode* node);
	string searchSong(const string& artist, const string& title);
};