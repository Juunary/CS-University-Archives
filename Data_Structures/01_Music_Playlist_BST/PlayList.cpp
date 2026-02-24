// PlayList.cpp
#include "PlayList.h"
#include <iostream> 
#include <fstream>

PlayList::PlayList() {
    this->head = nullptr;
    this->count = 0;
    this->time = 0;
}

PlayList::~PlayList() {
    if (empty()) {
        return;
    }
    PlayListNode* current = head->next;
    while (current != head) {
        PlayListNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete head;
}

void PlayList::insert_node(const string& artist, const string& title, const string& run_time) {
    if (full()) { 
        return;
    }
    if (exist(artist, title)) {
        return;
    }
    
    int runtime_sec = stringToSec(run_time);
    PlayListNode* newNode = new PlayListNode(artist, title, runtime_sec);

    if (empty()) {
        // if list is empty, initialize circular doubly linked list
        head = newNode;
        head->next = head;
        head->prev = head;
    } else {
        // add new node to the end of the list (before head)
        PlayListNode* tail = head->prev; // find the current tail
        
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
        // head remains unchanged
    }
    count++;
    time += runtime_sec;
}

void PlayList::delete_node(const string& artist, const string& title, ofstream& flog) {
    if (empty()) { 
        return; 
    }
    // ignore if both artist and title are empty
    if (artist.empty() && title.empty()) {
        return;
    }

    PlayListNode* current = head;
    int initial_count = count; // loop control to avoid infinite loop

    for (int i = 0; i < initial_count; ++i) {
        // save next node before potential deletion
        PlayListNode* next_node = current->next; 

        // check if the current node is the one to delete
        bool artist_match = artist.empty() || (current->artist == artist);
        bool title_match = title.empty() || (current->title == title);

        if (artist_match && title_match) {
            // start of node deletion logic
            time -= current->runtime_sec;
            count--;

            if (count == 0) { // if this was the last node
                delete current;
                head = nullptr;
                // no need to continue the loop
                break; 
            } else {
                // normal case: remove current node from the list
                current->prev->next = current->next;
                current->next->prev = current->prev;

                // if the node to delete was head, move head to the next node
                if (current == head) {
                    head = next_node;
                }
                delete current;
            }
            
        }

        // save next node before potential deletion
        current = next_node;
    }
}

bool PlayList::empty() {
    return this->count == 0;
}

bool PlayList::full() {
    return this->count >= 10; // max size is 10
}

int PlayList::getCount() {
    return this->count;
}

bool PlayList::exist(const string& artist, const string& title) {
    if (empty()) return false;

    PlayListNode* current = head;
    do {
        // check if artist parameter is empty or matches current node's artist
        bool artist_match = artist.empty() || (current->artist == artist);
        
        // check if title parameter is empty or matches current node's title
        bool title_match = title.empty() || (current->title == title);

        // find a match
        if (artist_match && title_match) {
            return true;
        }
        
        current = current->next;
    } while(current != head);

    return false;
}

string PlayList::print(ofstream& flog) {
    if (empty()) {
        flog << "Error: 600" << endl;
        return "";
    }

    string result_str = "";
    PlayListNode* current = head; 
    do { // circular traversal
        result_str += current->artist + "/" + current->title + "/" + secToString(current->runtime_sec)+ "\n";
        current = current->next;
    } while (current != head);
    // append count and total time
    result_str += "Count: " + to_string(count) + "/10\n";
    result_str += "Time: " + to_string(time / 60) + "min " + to_string(time % 60) + "sec\n";
    return result_str;
}

int PlayList::run_time() {
    return this->time;
}


int PlayList::stringToSec(const string& run_time) {
    size_t colon_pos = run_time.find(':');
    if (colon_pos == string::npos) return 0;
    int minutes = stoi(run_time.substr(0, colon_pos));
    int seconds = stoi(run_time.substr(colon_pos + 1));
    return minutes * 60 + seconds;
}

string PlayList::secToString(int seconds) {
    string min = to_string(seconds / 60);
    string sec = to_string(seconds % 60);
    if (seconds % 60 < 10) {
        sec = "0" + sec; // if seconds is a single digit, prepend '0'
    }
    return min + ":" + sec;
}