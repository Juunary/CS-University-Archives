#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include <fstream>
#include <string>

class BpTree {
private:
    BpTreeNode* root;
    int order;                 
    std::ofstream* fout;       

public:
    BpTree(std::ofstream* f, int m = 3)
        : root(nullptr), order(m), fout(f) {}
    ~BpTree() {}

    /* essential services */
    bool        Insert(EmployeeData* newData);
    bool        excessDataNode(BpTreeNode* pDataNode);
    bool        excessIndexNode(BpTreeNode* pIndexNode);
    void        splitDataNode(BpTreeNode* pDataNode);
    void        splitIndexNode(BpTreeNode* pIndexNode);
    BpTreeNode* getRoot() { return root; }
    BpTreeNode* searchDataNode(std::string name);
    void        searchRange(std::string start, std::string end);

    /* convenience for Manager */
    bool Search(const std::string& name, std::ofstream& flog);                 // single key
    bool Search(const std::string& start, const std::string& end, std::ofstream& flog); // range   
    bool Print(std::ofstream& flog);                                           // print
};

#endif
