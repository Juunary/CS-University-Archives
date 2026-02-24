// TitleBSTNode.h

#pragma once
#include "MusicQueueNode.h"
#include <vector>
#include <string>
class TitleBSTNode {
private:
	string title;
	vector<string> artist;
	string run_time;
	vector<int> rt;
	int count;
	TitleBSTNode* left;
	TitleBSTNode* right;

	friend class TitleBST;
public:
	TitleBSTNode() {
		this->title = "";
		this->count = 0;
		this->left = nullptr;
		this->right = nullptr;
	}
	~TitleBSTNode() {}

	void set(const string& title, const string& artist, const string& run_time) {
        if (this->title.empty()) { // if this is a new title node
            this->title = title;
            this->run_time = run_time;
        } else { // if adding a new artist to an existing song
            this->run_time += "," + run_time;
        }
        
        this->artist.push_back(artist);
        this->count++;
    }
	
	void delete_artist(const string& artist) {
		int song_index = -1;
		// find the index of the artist to be deleted
		for (int i = 0; i < this->artist.size(); ++i) {
			if (this->artist[i] == artist) {
				song_index = i;
				break;
			}
		}

		// if artist not found, do nothing
		if (song_index == -1) {
			return;
		}

		// delete the artist from the vector
		this->artist.erase(this->artist.begin() + song_index);

		// delete the corresponding run_time from the string
		string all_runtimes = this->run_time;
		string new_runtimes = "";
		stringstream ss(all_runtimes);
		string time_part;
		int current_index = 0;
		while(getline(ss, time_part, ',')) {
			if (time_part[0] == ' ') {
				time_part = time_part.substr(1);
			}
			if (current_index != song_index) {
				if (!new_runtimes.empty()) {
					new_runtimes += ", ";
				}
				new_runtimes += time_part;
			}
			current_index++;
		}
		this->run_time = new_runtimes;
		this->count--;
	}

	int getSongCount() { return this->count; } // return number of artists for this title	
	
};