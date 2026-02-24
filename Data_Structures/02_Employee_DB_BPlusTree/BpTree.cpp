#include "BpTree.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <typeinfo>

using std::string;

static inline bool isData(BpTreeNode* n)  { return dynamic_cast<BpTreeDataNode*>(n)  != nullptr; }
static inline bool isIndex(BpTreeNode* n) { return dynamic_cast<BpTreeIndexNode*>(n) != nullptr; }
// Insert new employee 
bool BpTree::Insert(EmployeeData* newData) {
    if (!newData) return false;

    string key = newData->getName();

    // 1) empty tree -> create first leaf
    if (!root) {
        BpTreeDataNode* lf = new BpTreeDataNode();
        lf->insertDataMap(key, newData);
        root = lf;
        return true;
    }

    // 2) find target leaf
    BpTreeDataNode* leaf = dynamic_cast<BpTreeDataNode*>(searchDataNode(key));

    // defensive: if search failed but we have an index root without children
    if (!leaf) {
        BpTreeDataNode* lf = new BpTreeDataNode();
        lf->insertDataMap(key, newData);

        if (dynamic_cast<BpTreeIndexNode*>(root) && root->getMostLeftChild() == nullptr) {
            root->setMostLeftChild(lf);
            lf->setParent(root);
        } else if (!dynamic_cast<BpTreeIndexNode*>(root)) {
            root = lf;
        }
        return true;
    }

    // 3) if name already exists, update only income (ADD_BP rule)
    auto* DM = leaf->getDataMap();
    auto  hit = DM->find(key);
    if (hit != DM->end()) {
        hit->second->setIncome(newData->getIncome());
        delete newData; // discard duplicate object
        return true;
    }

    // 4) insert new record then split if overflow
    leaf->insertDataMap(key, newData);
    if (excessDataNode(leaf)) splitDataNode(leaf);
    return true;
}

// check data(leaf) node overflow: |keys| > m-1
bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
    BpTreeDataNode* dn = dynamic_cast<BpTreeDataNode*>(pDataNode);
    if (!dn) return false;
    auto* M = dn->getDataMap();
    return M && M->size() > static_cast<size_t>(order - 1);
}

// check index node overflow: |keys| > m-1
bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
    BpTreeIndexNode* in = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
    if (!in) return false;
    auto* M = in->getIndexMap();
    return M && M->size() > static_cast<size_t>(order - 1);
}

// split a leaf node into (left,right) and push up the first key of right
void BpTree::splitDataNode(BpTreeNode* pDataNode) {
    BpTreeDataNode* left = dynamic_cast<BpTreeDataNode*>(pDataNode);
    if (!left) return;

    auto* L = left->getDataMap();
    const size_t N = L->size();
    const size_t cut = (N + 1) / 2; // make left a little heavier

    BpTreeDataNode* right = new BpTreeDataNode();

    // move upper half to 'right'
    auto it = L->begin();
    for (size_t i = 0; i < cut && it != L->end(); ++i) ++it;

    for (auto jt = it; jt != L->end(); ) {
        auto cur = jt++;
        const string k = cur->first;
        EmployeeData* v = cur->second;
        right->insertDataMap(k, v);
        left->deleteMap(k); // erase from 'left'
    }

    // stitch the leaf-level doubly linked list
    right->setNext(left->getNext());
    if (left->getNext()) left->getNext()->setPrev(right);
    left->setNext(right);
    right->setPrev(left);

    // promote the first key of the right leaf
    const string upKey = right->getDataMap()->begin()->first;

    // insert promoted key to parent (create root if needed)
    BpTreeIndexNode* parent = dynamic_cast<BpTreeIndexNode*>(left->getParent());
    if (!parent) {
        BpTreeIndexNode* newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(left);
        newRoot->insertIndexMap(upKey, right);
        left->setParent(newRoot);
        right->setParent(newRoot);
        root = newRoot;
    } else {
        parent->insertIndexMap(upKey, right);
        right->setParent(parent);
        if (excessIndexNode(parent)) splitIndexNode(parent);
    }
}

// split an index node; push middle key up; distribute children
void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
    BpTreeIndexNode* left = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
    if (!left) return;

    auto* L = left->getIndexMap();
    const size_t N = L->size();
    const size_t mid = N / 2;

    // locate middle key (to be pushed up)
    auto it = L->begin();
    for (size_t i = 0; i < mid && it != L->end(); ++i) ++it;

    const string upKey = it->first;
    BpTreeNode* rightFirstChild = it->second; // first child for right index

    BpTreeIndexNode* right = new BpTreeIndexNode();
    right->setMostLeftChild(rightFirstChild);
    if (rightFirstChild) rightFirstChild->setParent(right);

    // move keys strictly AFTER upKey to 'right'
    ++it;
    for (auto jt = it; jt != L->end(); ) {
        const string k = jt->first;
        BpTreeNode* ch = jt->second;
        ++jt;
        left->deleteMap(k);
        right->insertIndexMap(k, ch);
        if (ch) ch->setParent(right);
    }

    // remove the promoted key from 'left'
    left->deleteMap(upKey);

    // attach right to parent (create new root if needed)
    BpTreeIndexNode* parent = dynamic_cast<BpTreeIndexNode*>(left->getParent());
    if (!parent) {
        BpTreeIndexNode* newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(left);
        newRoot->insertIndexMap(upKey, right);
        left->setParent(newRoot);
        right->setParent(newRoot);
        root = newRoot;
    } else {
        parent->insertIndexMap(upKey, right);
        right->setParent(parent);
        if (excessIndexNode(parent)) splitIndexNode(parent);
    }
}

// descend the tree to find the leaf that should contain 'name'
BpTreeNode* BpTree::searchDataNode(string name) {
    if (!root) return nullptr;

    BpTreeNode* cur = root;
    while (dynamic_cast<BpTreeDataNode*>(cur) == nullptr) {
        BpTreeIndexNode* idx = dynamic_cast<BpTreeIndexNode*>(cur);
        if (!idx) return nullptr;

        auto* M = idx->getIndexMap();
        BpTreeNode* next = idx->getMostLeftChild();

        // choose the right child: the last edge with key <= name
        for (auto it = M->begin(); it != M->end(); ++it) {
            if (name < it->first) break;
            next = it->second;
        }

        cur = next;
        if (!cur) return nullptr; // defensive
    }
    return cur; // leaf reached
}

// inclusive range search [start, end]; print to this->fout
void BpTree::searchRange(string start, string end) {
    if (!fout) return;

    if (!root) {
        (*fout) << "========ERROR========\n300\n=====================\n\n";
        return;
    }

    BpTreeDataNode* leaf = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
    if (!leaf) {
        (*fout) << "========ERROR========\n300\n=====================\n\n";
        return;
    }

    bool any = false;
    (*fout) << "========SEARCH_BP========\n";

    for (BpTreeDataNode* p = leaf; p != nullptr; p = dynamic_cast<BpTreeDataNode*>(p->getNext())) {
        auto& M = *p->getDataMap();
        auto it = M.lower_bound(start); // first key >= start in this leaf

        for (; it != M.end(); ++it) {
            const string& k = it->first;
            if (k > end) {
                if (any) (*fout) << "=======================\n\n";
                else      (*fout) << "========ERROR========\n300\n=====================\n\n";
                return;
            }
            EmployeeData* e = it->second;
            (*fout) << e->getName() << "/" << e->getDeptNo() << "/"
                    << e->getID()   << "/" << e->getIncome() << "\n";
            any = true;
        }
    }

    if (any) (*fout) << "=======================\n\n";
    else     (*fout) << "========ERROR========\n300\n=====================\n\n";
}
// B+Tree Search and Print functions
bool BpTree::Search(const string& name, std::ofstream& flog) {
    if (!root) { flog << "========ERROR========\n300\n=====================\n\n"; return false; }
    auto* leaf = dynamic_cast<BpTreeDataNode*>(searchDataNode(name));
    if (!leaf) { flog << "========ERROR========\n300\n=====================\n\n"; return false; }
    auto* M = leaf->getDataMap();
    auto it = M->find(name);
    if (it == M->end()) { flog << "========ERROR========\n300\n=====================\n\n"; return false; }
    auto* e = it->second;
    flog << "========SEARCH_BP========\n";
    flog << e->getName() << "/" << e->getDeptNo() << "/" << e->getID() << "/" << e->getIncome() << "\n";
    flog << "=======================\n\n";
    return true;
}
// Range search
bool BpTree::Search(const string& start, const string& end, std::ofstream& flog) {
    if (!root) { flog << "========ERROR========\n300\n=====================\n\n"; return false; }
    auto* leaf = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
    if (!leaf) { flog << "========ERROR========\n300\n=====================\n\n"; return false; }
    bool printed = false;
    flog << "========SEARCH_BP========\n";
    for (auto* cur = leaf; cur != nullptr; cur = dynamic_cast<BpTreeDataNode*>(cur->getNext())) {
        auto& M = *cur->getDataMap();
        auto it = M.lower_bound(start);
        for (; it != M.end(); ++it) {
            const string& k = it->first;
            if (k > end) {
                if (printed) flog << "=======================\n\n";
                else         flog << "========ERROR========\n300\n=====================\n\n";
                return printed;
            }
            auto* e = it->second;
            flog << e->getName() << "/" << e->getDeptNo() << "/" << e->getID() << "/" << e->getIncome() << "\n";
            printed = true;
        }
    }
    if (printed) flog << "=======================\n\n";
    else         flog << "========ERROR========\n300\n=====================\n\n";
    return printed;
}
bool BpTree::Print(std::ofstream& flog) {
    if (!root) { flog << "========ERROR========\n400\n=====================\n\n"; return false; }
    // move to leftmost leaf
    BpTreeNode* cur = root;
    while (!isData(cur)) cur = cur->getMostLeftChild();                          
    auto* leaf = dynamic_cast<BpTreeDataNode*>(cur);
    if (!leaf) { flog << "========ERROR========\n400\n=====================\n\n"; return false; }

    bool printed = false;
    flog << "========PRINT_BP========\n";
    for (auto* p = leaf; p != nullptr; p = dynamic_cast<BpTreeDataNode*>(p->getNext())) {
        for (auto& kv : *p->getDataMap()) {
            auto* e = kv.second;
            flog << e->getName() << "/" << e->getDeptNo() << "/" << e->getID() << "/" << e->getIncome() << "\n";
            printed = true;
        }
    }
    if (printed) flog << "=======================\n\n";
    else         flog << "========ERROR========\n400\n=====================\n\n";
    return printed;
}