#include "SelectionTree.h"
#include "EmployeeHeap.h"
#include "BpTree.h"
#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include <algorithm>
#include <vector>

using std::string;
using std::vector;

namespace {
    inline int bucketOf(int dept) {
        if (dept < 100 || dept > 800 || dept % 100 != 0) return -1;
        int idx = dept / 100 - 1;
        return (0 <= idx && idx < 8) ? idx : -1;
    }

    // Max-by (income desc, name asc)
    inline EmployeeData* pickBetter(EmployeeData* a, EmployeeData* b) {
        if (!a) return b;
        if (!b) return a;
        if (a->getIncome() != b->getIncome())
            return (a->getIncome() > b->getIncome()) ? a : b;
        return (a->getName() <= b->getName()) ? a : b;
    }

    // pull up winners from a given node
    inline void pullUpWinners(SelectionTreeNode* from) {
        for (auto* cur = from; cur; cur = cur->getParent()) {
            auto* L = cur->getLeftChild();
            auto* R = cur->getRightChild();
            EmployeeData* le = L ? L->getEmployeeData() : nullptr;
            EmployeeData* ri = R ? R->getEmployeeData() : nullptr;
            cur->setEmployeeData(pickBetter(le, ri));
        }
    }
} // anonymous namespace

// Build SelectionTree structure
void SelectionTree::setTree() {
    if (root) return; // build once

    // allocate nodes (1-based indexing for convenience)
    SelectionTreeNode* node[16] = {nullptr};
    for (int i = 1; i <= 15; ++i) node[i] = new SelectionTreeNode(); // leaf&internal

    // wire internal nodes (1..7) with left/right/parent
    for (int i = 1; i <= 7; ++i) {
        node[i]->setLeftChild(node[i * 2]);
        node[i]->setRightChild(node[i * 2 + 1]);
        node[i * 2]->setParent(node[i]);
        node[i * 2 + 1]->setParent(node[i]);
    }

    // initialize leaves (8..15) as runs with heaps
    for (int i = 8; i <= 15; ++i) {
        node[i]->HeapInit();              // each run owns a heap
        run[i - 8] = node[i];            // dept 100..800 -> run[0]..run[7]
    }

    root = node[1];
}

// Insert into SelectionTree
bool SelectionTree::Insert(EmployeeData* newData) {
    if (!newData) return false;
    if (!root) setTree();

    const int slot = bucketOf(newData->getDeptNo());
    if (slot < 0) return false;

    SelectionTreeNode* leaf = run[slot];
    if (!leaf) return false;

    EmployeeHeap* heap = leaf->getHeap();
    if (!heap) { leaf->HeapInit(); heap = leaf->getHeap(); }
    if (!heap) return false; // defensive

    heap->Insert(newData);
    leaf->setEmployeeData(heap->Top());
    pullUpWinners(leaf->getParent());
    return true;
}

// Delete top of SelectionTree
bool SelectionTree::Delete() {
    if (!root) return false;

    EmployeeData* top = root->getEmployeeData();
    if (!top) return false;

    const int slot = bucketOf(top->getDeptNo());
    if (slot < 0) return false;

    SelectionTreeNode* leaf = run[slot];
    if (!leaf) return false;

    EmployeeHeap* heap = leaf->getHeap();
    if (!heap || heap->IsEmpty()) return false;

    heap->Delete();                        // pop best from that run
    leaf->setEmployeeData(heap->Top());    // new representative
    pullUpWinners(leaf->getParent());
    return true;
}

bool SelectionTree::printEmployeeData(int dept_no) {
    if (!fout) return false;

    const int slot = bucketOf(dept_no);
    EmployeeHeap* heap = (slot >= 0 && run[slot]) ? run[slot]->getHeap() : nullptr;

    if (slot < 0 || !heap || heap->IsEmpty()) {
        (*fout) << "========ERROR========\n600\n=====================\n\n"; // PRINT_ST error
        return false;
    }

    vector<EmployeeData*> buf;
    heap->ToVector(buf);
    if (buf.empty()) {
        (*fout) << "========ERROR========\n600\n=====================\n\n";
        return false;
    }

    std::sort(buf.begin(), buf.end(),
              [](EmployeeData* a, EmployeeData* b) {
                  if (a == nullptr || b == nullptr) return a != nullptr;
                  if (a->getIncome() != b->getIncome())
                      return a->getIncome() > b->getIncome();     // income desc
                  return a->getName() < b->getName();             // tie-break: name asc
              });

    (*fout) << "========PRINT_ST========\n";
    for (auto* e : buf) {
        (*fout) << e->getName() << "/" << e->getDeptNo() << "/"
                << e->getID()   << "/" << e->getIncome() << "\n";
    }
    (*fout) << "=======================\n\n";
    return true;
}


bool SelectionTree::AddByDeptFromBpTree(BpTree* bp, int dept_no) {
    if (!bp) return false;
    if (!root) setTree();

    const int slot = bucketOf(dept_no);
    if (slot < 0) return false;

    // go to leftmost leaf of B+Tree and sweep via leaf chain
    BpTreeNode* t = bp->getRoot();
    if (!t) return false;

    while (dynamic_cast<BpTreeIndexNode*>(t)) t = t->getMostLeftChild();
    auto* leaf = dynamic_cast<BpTreeDataNode*>(t);
    if (!leaf) return false;
    // scan all leaf nodes
    bool touched = false;
    for (auto* cur = leaf; cur; cur = dynamic_cast<BpTreeDataNode*>(cur->getNext())) {
        auto* M = cur->getDataMap();
        if (!M) continue;
        for (auto& kv : *M) {
            EmployeeData* e = kv.second;
            if (e && e->getDeptNo() == dept_no) {
                Insert(e);
                touched = true;
            }
        }
    }
    return touched;
}
// Add SelectionTree by name
bool SelectionTree::AddByNameFromBpTree(BpTree* bp, const std::string& name) {
    if (!bp) return false;
    if (!root) setTree();

    auto* leaf = dynamic_cast<BpTreeDataNode*>(bp->searchDataNode(name));
    if (!leaf) return false;

    auto* M = leaf->getDataMap();
    if (!M) return false;

    auto it = M->find(name);
    if (it == M->end() || !it->second) return false;

    return Insert(it->second);
}
