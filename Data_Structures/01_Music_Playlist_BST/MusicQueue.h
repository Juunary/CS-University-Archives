// MusicQueue.h
#pragma once
#include "MusicQueueNode.h"
#include <fstream>
class MusicQueue {
private:
	MusicQueueNode* head;
	MusicQueueNode* tail;
	int size = 0;
	const int max_size = 100;
public:
	MusicQueue();
	~MusicQueue();

	bool empty();
	bool exist(string artist, string title);
	bool push(string artist, string title, string run_time, ofstream& flog);
	MusicQueueNode* pop();
	MusicQueueNode* front();
};