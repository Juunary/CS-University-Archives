#pragma once
#include "EmployeeHeap.h"
// Constructor
void EmployeeHeap::Insert(EmployeeData* data) {
    if (!data) return;

    // ensure capacity for 1-based heap
    if (datanum >= maxCapacity) ResizeArray();

    int pos = ++datanum;
    heapArr[pos] = data;
    UpHeap(pos);
}
// Get top element
EmployeeData* EmployeeHeap::Top() {
    return (datanum == 0) ? nullptr : heapArr[1];
}
// Delete top element
void EmployeeHeap::Delete() {
    if (datanum == 0) return;

    // move last to root and sink it
    heapArr[1] = heapArr[datanum];
    heapArr[datanum] = nullptr;
    --datanum;

    if (datanum > 0) DownHeap(1);
}
// Check if heap is empty
bool EmployeeHeap::IsEmpty() {
    return datanum == 0;
}

void EmployeeHeap::UpHeap(int index) {
    // local comparator: higher income first; if tie, smaller name first
    auto better = [](EmployeeData* a, EmployeeData* b) {
        if (!a) return false;
        if (!b) return true;
        if (a->getIncome() != b->getIncome()) return a->getIncome() > b->getIncome();
        return a->getName() < b->getName();
    };

    int i = index;
    while (i > 1) {
        int p = i / 2;
        if (!better(heapArr[i], heapArr[p])) break;

        // manual swap to avoid extra headers
        EmployeeData* tmp = heapArr[p];
        heapArr[p] = heapArr[i];
        heapArr[i] = tmp;

        i = p;
    }
}

void EmployeeHeap::DownHeap(int index) {
    auto better = [](EmployeeData* a, EmployeeData* b) {
        // local comparator: higher income first; if tie, smaller name first
        if (!a) return false;
        if (!b) return true;
        if (a->getIncome() != b->getIncome()) return a->getIncome() > b->getIncome();
        return a->getName() < b->getName();
    };

    int i = index;
    for (;;) {
        int left  = i * 2;
        if (left > datanum) break;           // no children

        int right = left + 1;
        int bestChild = left;
        if (right <= datanum && better(heapArr[right], heapArr[left])) bestChild = right;

        if (!better(heapArr[bestChild], heapArr[i])) break;

        EmployeeData* tmp = heapArr[i];
        heapArr[i] = heapArr[bestChild];
        heapArr[bestChild] = tmp;

        i = bestChild;
    }
}

void EmployeeHeap::ResizeArray() {
    // double capacity; handle edge-case when maxCapacity could be 0
    int newCap = (maxCapacity > 0) ? (maxCapacity * 2) : 2;

    EmployeeData** fresh = new EmployeeData*[newCap + 1];
    for (int i = 0; i <= newCap; ++i) fresh[i] = nullptr;

    // copy current heap (1-based)
    for (int i = 1; i <= datanum; ++i) fresh[i] = heapArr[i];

    delete[] heapArr;
    heapArr = fresh;
    maxCapacity = newCap;
}