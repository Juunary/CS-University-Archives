// ArtistBSTNode.h

#pragma once
#include "MusicQueueNode.h"
#include <vector>
#include <string>

class ArtistBST;

class ArtistBSTNode {
private:
	string artist;
	vector<string> title;
	string run_time;
	vector<int> rt; 
	int count; 
	ArtistBSTNode* left;
	ArtistBSTNode* right;

	friend class ArtistBST;

public:
	ArtistBSTNode() {
		this->artist = "";
		this->count = 0;
		this->left = nullptr;
		this->right = nullptr;
	}
	~ArtistBSTNode(){}

	void set(const string& artist, const string& title, const string& run_time) {
		if(this->artist.empty()) {
			this->artist = artist;
			this->run_time = run_time;
		} else {
			// add new song info to existing artist node 
			this->run_time += ("," + run_time);
		}
		this->title.push_back(title);
		this->count++;
	}

	void delete_title(const string& title) {
		int song_index = -1;
		// find the index of the song to be deleted
		for (int i = 0; i < this->title.size(); ++i) {
			if (this->title[i] == title) {
				song_index = i;
				break;
			}
		}

		// if song not found, do nothing
		if (song_index == -1) {
			return;
		}

		// delete the song title from the vector
		this->title.erase(this->title.begin() + song_index);
		
		// delete the corresponding run_time from the comma-separated string
		string all_runtimes = this->run_time;
		string new_runtimes = "";
		stringstream ss(all_runtimes);
		string time_part;
		int current_index = 0;
		while(getline(ss, time_part, ',')) {
			
			// if not the one to be deleted, keep it
			if (current_index != song_index) {
				if (!new_runtimes.empty()) {
					new_runtimes += ",";
				}
				new_runtimes += time_part;
			}
			current_index++;
		}
		this->run_time = new_runtimes;

		// decrease the song count
		this->count--;
	}
	int getSongCount() { return this->count; }
	
};