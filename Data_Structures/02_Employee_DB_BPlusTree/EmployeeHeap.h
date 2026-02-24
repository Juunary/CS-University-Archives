#pragma once
#include "EmployeeData.h"
#include <vector>

class EmployeeHeap {
private:
    int datanum;                 // number of elements in heap
    EmployeeData** heapArr;      // array for heap (1-based index)
    int maxCapacity = 10;       // max capacity

public:
    EmployeeHeap() {
        maxCapacity = 10;                    
        datanum = 0;
        heapArr = new EmployeeData*[maxCapacity + 1]; // 1..maxCapacity
        for (int i = 0; i <= maxCapacity; ++i) heapArr[i] = nullptr;
    }
    ~EmployeeHeap() {
        delete [] heapArr;                    // EmployeeData* are deleted elsewhere
    }
    
    void Insert(EmployeeData* data);
    EmployeeData* Top();
    void Delete();

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();

    void ToVector(std::vector<EmployeeData*>& out) const {
        out.clear();
        for (int i = 1; i <= datanum; ++i) if (heapArr[i]) out.push_back(heapArr[i]);
    }

    int Size() const { return datanum; }
};
