#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <cstring>
#include <string.h>
#include <fstream>
#include <map>
#include <set>
#include <math.h>
#include <vector>
#include <algorithm>
#include <deque>
#include <queue>
#include <stack>

using namespace std;

// Abstract base class for graph representations
// type : true  -> matrix graph
//         false -> list graph
class Graph
{
protected:
    bool m_Type;  // graph storage type
    int  m_Size;  // number of vertices

public:
    Graph(bool type, int size);
    virtual ~Graph();

    bool getType();
    int  getSize();

    // For algorithms that assume an undirected graph
    virtual void getAdjacentEdges(int vertex, map<int, int>* m) = 0;
    // For algorithms that use original directed edges
    virtual void getAdjacentEdgesDirect(int vertex, map<int, int>* m) = 0;
    // Insert a directed edge (from -> to) with given weight
    virtual void insertEdge(int from, int to, int weight) = 0;
    // Print graph structure
    virtual	bool printGraph(ofstream *fout) = 0;
};

#endif