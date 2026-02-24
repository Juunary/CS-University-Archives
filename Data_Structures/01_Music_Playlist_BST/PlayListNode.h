// PlayListNode.h
#pragma once
#include <string>

using namespace std;

class PlayList; // Forward declaration to avoid circular dependency

class PlayListNode {
private:
    string artist;
    string title;
    int runtime_sec; // saved as total seconds for easier calculations

    PlayListNode* prev;
    PlayListNode* next;
    
    // allow PlayList class to access private members
    friend class PlayList; 
public:
    PlayListNode(const string& artist, const string& title, int runtime_sec) {
        this->artist = artist;
        this->title = title;
        this->runtime_sec = runtime_sec;
        this->prev = nullptr;
        this->next = nullptr;
    }
    ~PlayListNode() {}
};