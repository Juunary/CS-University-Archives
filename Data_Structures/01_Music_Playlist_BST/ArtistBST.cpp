// ArtistBST.cpp
#include "ArtistBST.h"
#include "PlayList.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
ArtistBST::ArtistBST() {
    this->root = nullptr;

}
ArtistBST::~ArtistBST() {
    destroy_recursive(this->root);
}

void ArtistBST::destroy_recursive(ArtistBSTNode* node) {
    if (node == nullptr) {
        return; // if null node, just return
    }
    // delete left and right subtrees first
    destroy_recursive(node->left);
    destroy_recursive(node->right);
    // lastly, delete the current node
    delete node;
}

void ArtistBST::insert(const string& artist, const string& title, const string& run_time) {
    ArtistBSTNode* newNode = new ArtistBSTNode();
    newNode->set(artist, title, run_time);
    if (this->root == nullptr) {
        this->root = newNode;
        return;
    }
    ArtistBSTNode* current = this->root;
    while (current) {
        // compare artist names for BST property
        if (artist == current->artist) {
            current->set(artist, title, run_time);
            delete newNode;
            return;
        }
        // move left or right based on comparison
        if (artist < current->artist) {
            if (current->left == nullptr) {
                current->left = newNode;
                return;
            }
            current = current->left;
        } else { // artist > current->artist
            if (current->right == nullptr) {
                current->right = newNode;
                return;
            }
            current = current->right;
        }
    }
}

void ArtistBST::search(const string& artist, ofstream& flog) {
    ArtistBSTNode* current = this->root;
    while (current) {
        if (artist == current->artist) {
            
            
            string all_runtimes = current->run_time;
            size_t search_start_pos = 0;
            // write each song of the artist
            for (const auto& song_title : current->title) {
                size_t comma_pos = all_runtimes.find(',', search_start_pos);
                string time_part;
                if (comma_pos != string::npos) {
                    time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
                    search_start_pos = comma_pos + 1;
                } else {
                    time_part = all_runtimes.substr(search_start_pos);
                }
                flog << current->artist << "/" << song_title << "/" << time_part << endl;
            }
            return;
        } 
        else if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return; // if artist not found, do nothing
}

int ArtistBST::searchCount(const string& artist) {
    ArtistBSTNode* current = this->root;
    while (current) {
        if (artist == current->artist) {
            return current->getSongCount();
        } 
        else if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0; // if artist not found
}



// using PlayList& pl parameter to add songs directly to the provided PlayList
void ArtistBST::MAKEPL(const string& artist, PlayList& pl) {
    ArtistBSTNode* current = this->root;
    while (current) {
        if (artist == current->artist) {
            // if artist found, add all their songs to the provided PlayList
            string all_runtimes = current->run_time;
            size_t search_start_pos = 0;

            for (const auto& song_title : current->title) {
                size_t comma_pos = all_runtimes.find(',', search_start_pos);
                string time_part;
                if (comma_pos != string::npos) {
                    time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
                    search_start_pos = comma_pos + 1;
                } else {
                    time_part = all_runtimes.substr(search_start_pos);
                }
                
                // using the provided PlayList reference to insert songs
                pl.insert_node(current->artist, song_title, time_part);
            }
            return; 
        } 
        else if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return;
}

void ArtistBST::print_recursive(ArtistBSTNode* node, ofstream& flog) {
    if (node == nullptr) return;
    print_recursive(node->left, flog);
    
    // using find and substr to parse run_time string
    string all_runtimes = node->run_time;
    size_t search_start_pos = 0;

    for(const auto& song_title : node->title) {
        size_t comma_pos = all_runtimes.find(',', search_start_pos);
        string time_part;

        if (comma_pos != string::npos) {
            time_part = all_runtimes.substr(search_start_pos, comma_pos - search_start_pos);
            search_start_pos = comma_pos + 1;
        } else {
            time_part = all_runtimes.substr(search_start_pos);
        }
        flog << node->artist << "/" << song_title << "/" << time_part << endl;
    }

    print_recursive(node->right, flog);
}


void ArtistBST::print(ofstream& flog) {
    print_recursive(this->root, flog);
}

void ArtistBST::delete_node(const string& artist, const string& title, ofstream& flog){
    if (artist == "") {
        // if only title is given, delete all songs with that title across all artists
        delete_title(title, flog);
    }
    else if (title == "") {
        delete_artist(artist, flog);
        // if only artist is given, delete the entire artist node
        
    } else {
        delete_song(artist, title, flog);
    }

}
void ArtistBST::delete_title(const string& title, ofstream& flog) {
    delete_title_recursive(this->root, title, flog);
}

void ArtistBST::delete_title_recursive(ArtistBSTNode* node, const string& title, ofstream& flog) {
    // base case : if node is null, just return
    if (node == nullptr) {
        return;
    }

    node->delete_title(title);


    delete_title_recursive(node->left, title, flog);
    delete_title_recursive(node->right, title, flog);

}

void ArtistBST::delete_artist(const string& artist, ofstream& flog) {
    if (this->root == nullptr || artist.empty()) {
        flog << "Error: 700" ;
        return;
    }

    // find the artist node (current) and its parent (parent)
    ArtistBSTNode* current = this->root;
    ArtistBSTNode* parent = nullptr;

    while (current != nullptr && current->artist != artist) {
        parent = current;
        if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // if artist node not found
    if (current == nullptr) {
        flog << "Error: 700";
        return;
    }
    // actual deletion logic
    if (current->left == nullptr && current->right == nullptr) {
         if (current == this->root) this->root = nullptr;
         else if (parent->left == current) parent->left = nullptr;
         else parent->right = nullptr;
         delete current;
    }
    // if node has one child
    else if (current->left == nullptr || current->right == nullptr) {
        ArtistBSTNode* child = (current->left != nullptr) ? current->left : current->right;
        if (current == this->root) this->root = child;
        else if (parent->left == current) parent->left = child;
        else parent->right = child;
        delete current;
    }
    // if node has two children
    else {
        ArtistBSTNode* successorParent = current;
        ArtistBSTNode* successor = current->right;
        while (successor->left != nullptr) {
            successorParent = successor;
            successor = successor->left;
        }
        // copy successor node's data
        current->artist = successor->artist;
        current->title = successor->title;
        current->run_time = successor->run_time;
        current->count = successor->count;

        if (successorParent->left == successor) {
            successorParent->left = successor->right;
        } else {
            successorParent->right = successor->right;
        }
        delete successor;
    }

}


void ArtistBST::delete_song(const string& artist, const string& title, ofstream& flog) {
    if (this->root == nullptr || artist.empty() || title.empty()) {
        flog << "Error: 700" ;
        return;
    }

    // find the artist node (current) and its parent (parent)
    ArtistBSTNode* current = this->root;
    ArtistBSTNode* parent = nullptr;

    while (current != nullptr && current->artist != artist) {
        parent = current;
        if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // if artist node not found
    if (current == nullptr) {
        flog << "Error: 700";
        return;
    }

    // find the specific song title index within the artist node
    int song_index = -1;
    for (int i = 0; i < current->title.size(); ++i) {
        if (current->title[i] == title) {
            song_index = i;
            break;
        }
    }

    // if song title not found within the artist node
    if (song_index == -1) {
        flog << "Error: 700";
        return;
    }

    // deleting one song from the artist node
    if (current->count > 1) {
        // deleating one song, so just remove from vectors and strings
        current->title.erase(current->title.begin() + song_index);
        
        // run_time string needs to be reconstructed without the deleted song's time
        string all_runtimes = current->run_time;
        string new_runtimes = "";
        stringstream ss(all_runtimes);
        string time_part;
        int current_index = 0;
        while(getline(ss, time_part, ',')) {
            
            if (current_index != song_index) {
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
    // deleting the only song of the artist, so remove the entire artist node
    else {

        // 1. if node is a leaf
        if (current->left == nullptr && current->right == nullptr) {
             if (current == this->root) this->root = nullptr;
             else if (parent->left == current) parent->left = nullptr;
             else parent->right = nullptr;
             delete current;
        }
        // 2. if node has one child
        else if (current->left == nullptr || current->right == nullptr) {
            ArtistBSTNode* child = (current->left != nullptr) ? current->left : current->right;
            if (current == this->root) this->root = child;
            else if (parent->left == current) parent->left = child;
            else parent->right = child;
            delete current;
        }
        // 3. if node has two children
        else {
            ArtistBSTNode* successorParent = current;
            ArtistBSTNode* successor = current->right;
            while (successor->left != nullptr) {
                successorParent = successor;
                successor = successor->left;
            }
            // copy only the 'data' of the successor node
            current->artist = successor->artist;
            current->title = successor->title;
            current->run_time = successor->run_time;
            current->count = successor->count;
            
            // remove the successor node from the tree
            if (successorParent->left == successor) {
                successorParent->left = successor->right;
            } else {
                successorParent->right = successor->right;
            }
            delete successor;
        }
    }
}


string ArtistBST::searchSong(const string& artist, const string& title) {
    ArtistBSTNode* current = this->root;

    // find the artist node first
    while (current) {
        if (artist == current->artist) {
            // if artist found, look for the specific song title
            for (int i = 0; i < current->title.size(); ++i) {
                if (current->title[i] == title) {
                    // if song found, extract and return its run_time
                    stringstream ss(current->run_time);
                    string time_part;
                    for (int j = 0; j <= i; ++j) {
                        getline(ss, time_part, ',');
                    }
                    return time_part;
                }
            }
            // artist was found but song was not found
            return ""; 
        } 
        else if (artist < current->artist) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    // artist was not found
    return "";
}