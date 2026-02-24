// MusicQueueNode.h

#pragma once
#include <string>
#include <cstring>
#include <sstream>

using namespace std;

class MusicQueueNode {
private:
	string artist;
	string title;
	string run_time;

	MusicQueueNode* next;
	MusicQueueNode* prev;

public:
	MusicQueueNode() {
		this->artist = "";
		this->title = "";
		this->run_time = "";
		
		this->next = nullptr;
		this->prev = nullptr;
	}
	~MusicQueueNode() {

	}

	void insert(string artist, string title, string run_time) {
		// insert new music data into the node
		this->artist = artist;
		this->title = title;
		this->run_time = run_time;

	}
	

	bool exist(string artist, string title) {
		// check if the given artist and title match this node's data
		if (strcmp(this->artist.c_str(), artist.c_str()) == 0 && strcmp(this->title.c_str(), title.c_str()) == 0) {
			return true;
		}
		else {
			return false;
		}

	}
	// getter 선언
	string getArtist() { return this->artist; }
    string getTitle() { return this->title; }
	string getRunTime() { return this->run_time; }
    MusicQueueNode* getNext() { return this->next; }
    MusicQueueNode* getPrev() { return this->prev; }

	// setter 선언
    void setNext(MusicQueueNode* node) { this->next = node; }
    void setPrev(MusicQueueNode* node) { this->prev = node; }

};