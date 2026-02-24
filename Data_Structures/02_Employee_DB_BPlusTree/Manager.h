#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

class Manager {
private:
    char* cmd;                 // hold the raw parameter tail (for legacy-style handlers)
    BpTree* bptree;
    SelectionTree* stree;

    int bpOrder = 3;
    bool dsEmpty = true;       // data structure empty flag
    std::string lastCmdTail;   // hold the last command tail

    // helpers
    static inline bool isValidDept(int d) {
        return d==100||d==200||d==300||d==400||d==500||d==600||d==700||d==800;
    }
    static bool toInt(const std::string& s, int& out) {
        if(s.empty()) return false;
        std::istringstream iss(s);
        long long v; char c;
        if(!(iss>>v) || (iss>>c)) return false;
        out = static_cast<int>(v);
        return true;
    }
    static std::vector<std::string> tokenize(const std::string& s) {
        std::istringstream iss(s);
        std::vector<std::string> v; std::string w;
        while(iss>>w) v.push_back(w);
        return v;
    }

public:
    Manager(int bpOrder) {      // constructor
        this->bpOrder = bpOrder;
        cmd = nullptr;
        bptree = nullptr;        
        stree  = nullptr;       
    }

    ~Manager() {                // destructor
        if (cmd) { delete[] cmd; cmd = nullptr; }
        if (bptree) { delete bptree; bptree = nullptr; }
        if (stree)  { delete stree;  stree  = nullptr; }
        if (fin.is_open())  fin.close();
        if (flog.is_open()) flog.close();
    }

    // streams 
    std::ifstream fin;
    std::ofstream flog;

    //  API from skeleton 
    void run(const char* command);
    void LOAD();
    void ADD_BP();
    void SEARCH_BP_NAME(std::string name);
    void SEARCH_BP_RANGE(std::string start, std::string end);
    void PRINT_BP();
    void ADD_ST_DEPTNO(int dept_no);
    void ADD_ST_NAME(std::string name);
    void PRINT_ST();
    void DELETE();

    void printErrorCode(int n);
    void printSuccessCode(std::string success);
};
