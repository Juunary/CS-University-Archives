#pragma once
#include <fstream>
#include "SelectionTreeNode.h"
#include <fstream>
#include <functional>

// forward decl.
class BpTree;

class SelectionTree {
private:
    SelectionTreeNode* root;
    std::ofstream* fout;
    SelectionTreeNode* run[8]; 

public:
    SelectionTree(std::ofstream* fout) {
        this->root = nullptr;
        this->fout = fout;
        for (int i = 0; i < 8; ++i) run[i] = nullptr;
    }

    ~SelectionTree() {
        // simple post-order deletion
        std::function<void(SelectionTreeNode*)> gc = [&](SelectionTreeNode* n){
            if (!n) return;
            gc(n->getLeftChild());
            gc(n->getRightChild());
            delete n;
        };
        gc(root);
        root = nullptr;
    }

    void setRoot(SelectionTreeNode* pN) { this->root = pN; }
    SelectionTreeNode* getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);

    // helper wrappers (free to change behaviorally equivalent)
    bool AddByDeptFromBpTree(BpTree* bp, int dept_no);
    bool AddByNameFromBpTree(BpTree* bp, const std::string& name);
    bool PrintDept(std::ofstream& /*flog*/, int dept_no) { return printEmployeeData(dept_no); }
    bool DeleteTop() { return Delete(); }
};
