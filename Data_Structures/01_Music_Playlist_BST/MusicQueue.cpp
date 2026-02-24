// MusicQueue.cpp
#include "MusicQueue.h"
#include <iostream>
#include <string>
using namespace std;

MusicQueue::MusicQueue() {
    this->head = nullptr;
    this->tail = nullptr;
    this->size = 0;
}

MusicQueue::~MusicQueue() {
    while (!this->empty()) {
        MusicQueueNode* temp = this->pop();
        delete temp;
    }
}

bool MusicQueue::empty() {
    if (this->size == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool MusicQueue::exist(string artist, string title) {
    MusicQueueNode* current = this->head;
    while (current != nullptr) {
        if (current->exist(artist, title)) {
            return true;
        }
        current = current->getNext();
    }
    return false;
}

bool MusicQueue::push(string artist, string title, string run_time, ofstream& flog) {
    // size is 100
    if (this->size == this -> max_size) {
        flog << "Error: 100" << endl;
        exit(0);
        return false;
    }
    // size is less than 100
    else {
        MusicQueueNode* newNode = new MusicQueueNode();
        newNode->insert(artist, title, run_time);
        // check if queue is empty
        if (this->empty()) {
            this->head = newNode;
            this->tail = newNode;
        }
        // if queue is not empty
        else {
            
            this->tail->setNext(newNode);
            newNode->setPrev(this->tail);
            this->tail = newNode;
        }
        this->size++;
        return true;
    }
}

MusicQueueNode* MusicQueue::pop(){
    if (this->empty()) {
        return nullptr;
    }
    // save the current head to return later
    MusicQueueNode* temp = this->head;
    // move head to the next node
    this->head = this->head->getNext();
    this->size--;

    if (!this->empty()){
        this->head->setPrev(nullptr);
    }
    else {
        this->tail = nullptr; 
    }

    temp->setNext(nullptr);
    temp->setPrev(nullptr);
    return temp; // return the popped node
}

MusicQueueNode* MusicQueue::front() {
    return this->head;
}