// Manager.cpp
#include "Manager.h"
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

Manager::Manager() {

}

Manager::~Manager() {

}

void Manager::run(const char* command) {
    flog.open("log.txt");
    

    ifstream fcmd(command);
    if (!fcmd.is_open()) {
        flog << "Error: 100" << endl;
        return;
    }
    string line;
    while (getline(fcmd, line)) {
        // distinguish command and arguments
        size_t spacePos = line.find(' ');
        string cmd = (spacePos == string::npos) ? line : line.substr(0, spacePos);
        string args = (spacePos == string::npos) ? "" : line.substr(spacePos + 1);
        // call the corresponding function based on the command
        if (cmd == "LOAD") {
            flog << "========LOAD========" << endl;
            LOAD();
        } else if (cmd == "ADD") {
            flog << "========ADD========" << endl;
            ADD(args);
        } else if (cmd == "QPOP") {
            flog << "========QPOP========" << endl;
            QPOP();
        } else if (cmd == "SEARCH") {
            flog << "========SEARCH========" << endl;
            SEARCH(args);
        } else if (cmd == "MAKEPL") {
            flog << "========MAKEPL========" << endl;
            MAKEPL(args);
        } else if (cmd == "PRINT") { 
            flog << "========PRINT========" << endl;
            PRINT(args);
        } else if (cmd == "DELETE") {
            flog << "========DELETE========" << endl;
            DELETE(args);
        } else if (cmd == "EXIT") {
            flog << "========EXIT========" << endl;
            EXIT();

            break; // EXIT command is issued, terminate
        } else {
            flog << "Error: 1000" ;
        }
        flog << "====================" <<"\n"<< endl;
    }
    fcmd.close();
    flog.close();
}
void Manager::LOAD() {
    if(!q.empty()){
        flog<<"Error: 100" << endl;
        return;
    }
    ifstream fin("Music_List.txt");
    if (!fin.is_open()) {
        flog << "Error: 100" << endl;
        return;
    }
    string line;
    while (getline(fin, line)) {
        // parse the line to extract artist, title, and duration
        size_t firstDelim = line.find('|');
        size_t secondDelim = line.find('|', firstDelim + 1);
        if (firstDelim == string::npos || secondDelim == string::npos) {
            flog << "Error: 100" << endl;
            continue;
        }
        string artist = line.substr(0, firstDelim);
        string title = line.substr(firstDelim + 1, secondDelim - firstDelim - 1);
        string duration = line.substr(secondDelim + 1);
        // add to queue and log
        flog << artist << " / " << title << " / " << duration<< endl;
        q.push(artist, title, duration, flog);
    }
    fin.close();
    flog << endl;
}

void Manager::ADD(const string& args) {
    // parse the args to extract music information
    size_t firstDelim = args.find('|');
    size_t secondDelim = args.find('|', firstDelim + 1);
    if (firstDelim == string::npos || secondDelim == string::npos) {
        flog << "Error: 200" << endl;
        return;
    }
    string artist = args.substr(0, firstDelim);
    string title = args.substr(firstDelim + 1, secondDelim - firstDelim - 1);
    string duration = args.substr(secondDelim + 1);
    // add to queue and log 
    if(q.exist(artist, title)){
        flog << "Error: 200" <<   endl; 
        return;
    }
    else{
        // add to queue and log
        q.push(artist, title, duration, flog);
        flog << artist << " / " << title << " / " << duration << endl;
        return;
    }
}

void Manager::QPOP() {
    if (q.empty()) {
        // if queue is empty return err
        flog << "Error: 300" << endl;  
        exit(0);
    }

    bool had_dup = false;

    // 큐가 빌 때까지 POP
    while (MusicQueueNode* node = q.pop()) {
        const string a = node->getArtist();
        const string t = node->getTitle();
        const string r = node->getRunTime();

        if (!ab.searchSong(a, t).empty()) {  // check the song already exist
            had_dup = true;
            delete node;          // memry lose 
            continue;             
        }
        // new song
        ab.insert(a, t, r);
        tb.insert(t, a, r);

        delete node;             
    }

    if (had_dup) {
        flog << "Error: 300" << endl;  
    } else {
        flog << "SUCCESS" << endl;   
    }
    return;
}

void Manager::SEARCH(const string& args) {
	size_t spacePos = args.find(' ');
    if (spacePos == string::npos) {
        flog << "Error: 400" <<   endl; 
        return;
    }
    string command = args.substr(0, spacePos);
    string query = args.substr(spacePos + 1);

    //SEARCH ARTIST
    if (command == "ARTIST") {
        if(ab.searchCount(query) == 0){
            flog << "Error: 400" <<   endl; 
        }
        else{
            ab.search(query, flog);
        }
        return;
    }
    //SEARCH TITLE
    if (command == "TITLE") {
        if(tb.searchCount(query)==0){
            flog << "Error: 400" << endl;
        }
        tb.search(query, flog);
        
        return;
    }
    //SEARCH SONG
    if (command == "SONG") {
        size_t delimPos = query.find('|');
        string artist = query.substr(0, delimPos);
        string title = query.substr(delimPos + 1);
        string run_time = ab.searchSong(artist, title);

        if (!run_time.empty()) { // if returned run time is not empty (success)
            flog << artist << "/" << title << "/" << run_time <<   endl;
        } else { // if empty (failure)
            flog << "Error: 400" ;
        }
        return;
    }
}

void Manager::MAKEPL(const string& args) {
    // parse the args to determine type and value
    size_t spacePos = args.find(' ');
    if (spacePos == string::npos) {
        flog << "ERROR: 500" << endl;
        return;
    }
    string type = args.substr(0, spacePos);
    
    if (type == "ARTIST") {
        // check if adding this artist's songs would exceed limit
        string artist = args.substr(spacePos + 1);
        if((pl.getCount() + ab.searchCount(artist)) > 10) {
            flog << "ERROR: 500 " << endl;
            return;
        }
        else {
            ab.MAKEPL(artist, this->pl); 
        }
    } 
    else if (type == "TITLE") {
        // check if adding this title's songs would exceed limit
        string title = args.substr(spacePos + 1);
        if((pl.getCount() + tb.searchCount(title)) > 10) {
            flog << "ERROR: 500" << endl ;
        }
        // check if the title already exists in the playlist
        else if(pl.exist("", title)) {
            flog << "ERROR: 500"  << endl;
            return;
        }
        // insert into playlist
        else {
            tb.MAKEPL(title, this->pl); 
        }
    } 
    else if (type == "SONG"){
        // distinguish artist and title
        string song = args.substr(spacePos + 1);
        string artist = song.substr(0, song.find('|'));
        string title = song.substr(song.find('|') + 1);
        // check if will exceed limit or already exists
        string run_time = ab.searchSong(artist, title);
        if((pl.getCount() + 1) > 10) {
            flog << "ERROR: 500"  << endl;
        }
        else if (!run_time.empty()) {
            pl.insert_node(artist, title, run_time);
        } else { 
            flog << "ERROR: 500"  << endl;
            return;
        }
    }
    else {
        flog << "ERROR: 500" << endl;
        return;
    }


    string pl_output = pl.print(flog);

    flog << pl_output ;
}
void Manager::PRINT(const string& args) {
    // print ARTIST, TITLE, or LIST
    size_t spacePos = args.find(' ');
    string type = args.substr(0, spacePos);
    if (type =="ARTIST") {
        flog << "ArtistBST" << endl;
        ab.print(flog);
        flog << endl;
    }
    else if (type == "TITLE") {
        flog << "TitleBST" << endl;
        tb.print(flog);
        flog << endl;
    }
    else if (type == "LIST") {
        string pl_output = pl.print(flog);
        flog << pl_output << endl;
    }
    else {
        flog << "Error: 600" << endl;
        return;
    }
}

void Manager::DELETE(const string& args) {
    //700
	size_t spacePos = args.find(' ');
    string type = args.substr(0, spacePos);
    if (type =="ARTIST") {
        string artist = args.substr(spacePos + 1);
        // check if artist does not exist in both BSTs and playlist
        if(ab.searchCount(artist) == 0 && !pl.exist(artist, "")) {
            flog << "Error: 700" << endl ;
            return;
        }
        else{

            ab.delete_node(artist, "", flog);
            tb.delete_node(artist, "", flog);
            pl.delete_node(artist, "", flog);

            flog << "SUCCESS" ;  
        }
    }
    else if (type == "TITLE") {
        // check if title does not exist in both BSTs and playlist
        string title = args.substr(spacePos + 1);
        if(tb.searchCount(title) == 0 && !pl.exist("", title)) {
            flog << "Error: 700" << endl;
            return;
        }
        else{

            ab.delete_node("", title, flog);
            tb.delete_node("", title, flog);
            pl.delete_node("", title, flog);
            flog << "SUCCESS"<< endl ;  
        }
    }
    else if (type == "LIST") {
        size_t delimPos = args.find('|', spacePos + 1);
        string artist = args.substr(spacePos + 1, delimPos - spacePos - 1);
        string title = args.substr(delimPos + 1);
        // check if the song exists in the playlist
        if(!pl.exist(artist, title)) {
            flog << "Error: 700" << endl;
            return;
        }
        else{
            pl.delete_node(artist, title, flog);
            flog << "SUCCESS" << endl;  
        }
        
    }
    else if (type == "SONG") {
        size_t delimPos = args.find('|', spacePos + 1);
        string artist = args.substr(spacePos + 1, delimPos - spacePos - 1);
        string title = args.substr(delimPos + 1);
        if (ab.searchSong(artist, title).empty() && !pl.exist(artist, title)) {
            flog << "Error: 700"<< endl;
            return;
        }
        else{

            ab.delete_node(artist, title, flog);
            tb.delete_node(artist, title, flog);
            pl.delete_node(artist, title, flog);
            flog << "SUCCESS"<< endl;
        }
    }
    else {
        flog << "Error: 700"<< endl;
        return;
    }
    
}

void Manager::EXIT() {
    flog << "Success"<<endl;
    flog << "====================" <<"\n"<< endl;
}