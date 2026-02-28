# Employee DB — B+ Tree

## Overview

A C++ project implementing an employee database using a B+ Tree as the core index structure. It supports sorting and aggregation features using a Selection Tree (tournament tree) and Max Heap.

## Data Structure Composition

```
Manager
├── BpTree          — B+ Tree (employee name indexing)
│   ├── BpTreeIndexNode
│   └── BpTreeDataNode
└── SelectionTree   — Tournament tree (department sorting/aggregation)
    └── EmployeeHeap — Max Heap (extract highest salary by department)
```

## Supported Commands

| Command | Description |
|---------|-------------|
| `LOAD` | Load employee data file |
| `ADD_BP` | Add employee to B+ Tree |
| `SEARCH_BP_NAME` | Search employee by name |
| `SEARCH_BP_RANGE` | Search employees by name range |
| `PRINT_BP` | Print entire B+ Tree |
| `ADD_ST_DEPTNO` | Add to Selection Tree by department number |
| `ADD_ST_NAME` | Add to Selection Tree by name |
| `PRINT_ST` | Print Selection Tree |
| `DELETE` | Delete employee |

## File Structure

| File | Description |
|------|-------------|
| `main.cpp` | Entry point |
| `Manager.h/.cpp` | Command parsing and flow control |
| `BpTree.h/.cpp` | B+ Tree implementation |
| `BpTreeNode.h` | Node base class |
| `BpTreeIndexNode.h` | Index node |
| `BpTreeDataNode.h` | Data node |
| `SelectionTree.h/.cpp` | Tournament tree |
| `EmployeeHeap.h/.cpp` | Max Heap |
| `EmployeeData.h` | Employee data structure |

