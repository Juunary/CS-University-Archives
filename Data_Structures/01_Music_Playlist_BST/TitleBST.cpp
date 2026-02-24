// TitleBST.cpp

#include "TitleBST.h"
#include "PlayList.h"
#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

TitleBST::TitleBST()  {
    this->root = nullptr;
}
TitleBST::~TitleBST() {
    destroy_recursive(this->root);
}

void TitleBST::destroy_recursive(TitleBSTNode* node) {
    if (node == nullptr) return;
    destroy_recursive(node->left);
    destroy_recursive(node->right);
    delete node;
}

void TitleBST::insert(const string& title, const string& artist, const string& run_time) {
    TitleBSTNode* newNode = new TitleBSTNode();
    newNode->set(title, artist, run_time);

    if (this->root == nullptr) {
        this->root = newNode;
        return;
    }

    TitleBSTNode* current = this->root;
    while (current) {
        // compare titles for BST property
        if (title == current->title) {
            current->set(title, artist, run_time);
            delete newNode;
            return;
        }
        // move left or right based on comparison
        if (title < current->title) {
            if (current->left == nullptr) {
                current->left = newNode;
                return;
            }
            current = current->left;
        } else { // title > current->title
            if (current->right == nullptr) {
                current->right = newNode;
                return;
            }
            current = current->right;
        }
    }
}



void TitleBST::search(const string& title, ofstream& flog) {
    TitleBSTNode* current = this->root;
    while (current) {
        if (title == current->title) {
            // print all artist info for the found title
            string all_runtimes = current->run_time;
            size_t search_start_pos = 0;
            // iterate through all artists for this title
            for (const auto& artist_name : current->artist) {
                size_t comma_pos = all_runtimes.find(',', search_start_pos);
                string time_part;
                if (comma_pos != string::npos) {
                    time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
                    search_start_pos = comma_pos + 1;
                } else {
                    time_part = all_runtimes.substr(search_start_pos);
                }
                flog << artist_name << "/" << current->title << "/" << time_part << endl;
            }
            return;
        } 
        else if (title < current->title) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return; //if title not found, do nothing
}

int TitleBST::searchCount(const string& title) {
    TitleBSTNode* current = this->root;
    while (current) {
        if (title == current->title) {
            return current->getSongCount();
        } 
        else if (title < current->title) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0; // if title not found
}


void TitleBST::MAKEPL(const string& title, PlayList& pl) {
    TitleBSTNode* current = this->root;
    while (current) {
        if (title == current->title) {
            // add all songs with this title to the provided PlayList
            string all_runtimes = current->run_time;
            size_t search_start_pos = 0;
            // loop through all artists for this title
            for (const auto& artist_name : current->artist) {
                size_t comma_pos = all_runtimes.find(',', search_start_pos);
                string time_part;
                if (comma_pos != string::npos) {
                    time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
                    search_start_pos = comma_pos + 1; 
                } else {
                    time_part = all_runtimes.substr(search_start_pos);
                }
                
                if (!time_part.empty()) {
                    // only insert if the song is not already in the playlist
                    if(!pl.exist(artist_name, title)) {
                        pl.insert_node(artist_name, title, time_part);
                    }
                    
                }
            }
            return; 
        } 
        else if (title < current->title) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return;
}

void TitleBST::print_recursive(TitleBSTNode* node, ofstream& flog) {
    if (node == nullptr) return;
    print_recursive(node->left, flog);
    
    // find and print all artist info for this title
    string all_runtimes = node->run_time;
    size_t search_start_pos = 0;

    for(const auto& artist_name : node->artist) {
        size_t comma_pos = all_runtimes.find(',', search_start_pos);
        string time_part;

        if (comma_pos != string::npos) {
            time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
            search_start_pos = comma_pos + 1; 
        } else {
            time_part = all_runtimes.substr(search_start_pos);
        }
        flog << artist_name << "/" << node->title << "/" << time_part << endl;
    }
    print_recursive(node->right, flog); // in-order traversal
}

void TitleBST::print(ofstream& flog) {
    print_recursive(this->root, flog);
}
void TitleBST::delete_node(const string& artist,const string& title, ofstream& flog) {
    if (title == "") {
        delete_artist(artist, flog);
    }
    else if (artist == "") {
        delete_title(title, flog);
    }
    else {
        delete_song(artist, title, flog);
    }
}
void TitleBST::delete_title(const string& title, ofstream& flog){
    // if tree is empty or title is empty, do nothing
    if (this->root == nullptr || title.empty()) {
        return;
    }

    //find the title node (current) and its parent (parent)
    TitleBSTNode* current = this->root;
    TitleBSTNode* parent = nullptr;

    // compare titles to find the node
    while (current != nullptr && current->title != title) {
        parent = current;
        if (title < current->title) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // if title node not found
    if (current == nullptr) {
        flog << "Error: 700"<<endl;
        return;
    }

    // if found, delete the node from the tree
    if (current->left == nullptr && current->right == nullptr) {
        if (current == this->root) this->root = nullptr;
        else if (parent->left == current) parent->left = nullptr;
        else parent->right = nullptr;
        delete current;
    }
    // if node has one child
    else if (current->left == nullptr || current->right == nullptr) {
        TitleBSTNode* child = (current->left != nullptr) ? current->left : current->right;
        if (current == this->root) this->root = child;
        else if (parent->left == current) parent->left = child;
        else parent->right = child;
        delete current;
    }
    // if node has two children
    else {
        TitleBSTNode* successorParent = current;
        TitleBSTNode* successor = current->right;
        while (successor->left != nullptr) {
            successorParent = successor;
            successor = successor->left;
        }

        // copy successor node's data to current node
        current->title = successor->title;
        current->artist = successor->artist;
        current->run_time = successor->run_time;
        current->count = successor->count;
        
        // delete the successor node
        if (successorParent->left == successor) {
            successorParent->left = successor->right;
        } else {
            successorParent->right = successor->right;
        }
        delete successor;
    }
}


void TitleBST::delete_artist(const string& artist, ofstream& flog){
    delete_artist_recursive(this->root, artist, flog);
}
void TitleBST::delete_artist_recursive(TitleBSTNode* node, const string& artist, ofstream& flog){
    if (node == nullptr) return;

    // search left subtree
    delete_artist_recursive(node->left, artist, flog);

    // delete artist from current node
    node->delete_artist(artist);

    // search right subtree
    delete_artist_recursive(node->right, artist, flog);
}

void TitleBST::delete_song(const string& artist, const string& title, ofstream& flog) {
    if (this->root == nullptr || artist.empty() || title.empty()) {
        flog << "Error: 700 "<<endl;
        return;
    }

    // step 1: find the title node (current) and its parent (parent)
    TitleBSTNode* current = this->root;
    TitleBSTNode* parent = nullptr;

    while (current != nullptr && current->title != title) {
        parent = current;
        if (title < current->title) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // if title node not found
    if (current == nullptr) {
        flog << "Error: 700 "<<endl;
        return;
    }

    //  find the index of the specific artist within the found node
    int artist_index = -1;
    for (int i = 0; i < current->artist.size(); ++i) {
        if (current->artist[i] == artist) {
            artist_index = i;
            break;
        }
    }

    // if artist not found within the title node
    if (artist_index == -1) {
        flog << "Error: 700 "<<endl;
        return;
    }

    // delete the artist from the node
    if (current->count > 1) {
        current->artist.erase(current->artist.begin() + artist_index);

        // remove the corresponding run_time from the comma-separated string
        string all_runtimes = current->run_time;
        string new_runtimes = "";
        stringstream ss(all_runtimes);
        string time_part;
        int current_index = 0;
        
        while(getline(ss, time_part, ',')) {
            if (current_index != artist_index) {
                if (!new_runtimes.empty()) {
                    new_runtimes += ",";
                }
                new_runtimes += time_part;
            }
            current_index++;
        }
        current->run_time = new_runtimes;
        current->count--;
    } 
    // deleting the only artist for this title, so remove the entire title node
    else {
        // if node is a leaf
        if (current->left == nullptr && current->right == nullptr) {
             if (current == this->root) this->root = nullptr;
             else if (parent->left == current) parent->left = nullptr;
             else parent->right = nullptr;
             delete current;
        }
        // if node has one child
        else if (current->left == nullptr || current->right == nullptr) {
            TitleBSTNode* child = (current->left != nullptr) ? current->left : current->right;
            if (current == this->root) this->root = child;
            else if (parent->left == current) parent->left = child;
            else parent->right = child;
            delete current;
        }
        // if node has two children
        else {
            TitleBSTNode* successorParent = current;
            TitleBSTNode* successor = current->right;
            while (successor->left != nullptr) {
                successorParent = successor;
                successor = successor->left;
            }
            // copy successor node's data to current node
            current->title = successor->title;
            current->artist = successor->artist;
            current->run_time = successor->run_time;
            current->count = successor->count;
            // delete the successor node
            if (successorParent->left == successor) {
                successorParent->left = successor->right;
            } else {
                successorParent->right = successor->right;
            }
            delete successor;
        }
    }
}