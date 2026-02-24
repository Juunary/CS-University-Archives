#include "ListGraph.h"
#include <iostream>
#include <utility>

using namespace std;

// Constructor : allocate adjacency list
ListGraph::ListGraph(bool type, int size) : Graph(type, size)
{
    // adjacency list for each vertex
    m_List = new map<int,int>[m_Size];

    // not used in this project, but keep pointer safe
    kw_graph = nullptr;
}

// Destructor : release allocated memory
ListGraph::~ListGraph()	
{
    if (m_List)
    {
        delete [] m_List;
        m_List = nullptr;
    }

    if (kw_graph)
    {
        delete [] kw_graph;
        kw_graph = nullptr;
    }
}

// Undirected adjacency : outgoing + incoming edges
void ListGraph::getAdjacentEdges(int vertex, map<int, int>* m)	 
{
    m->clear();
    if (vertex < 0 || vertex >= m_Size) return;

    // outgoing edges from "vertex"
    for (auto &p : m_List[vertex])
    {
        int to = p.first;
        int w  = p.second;

        auto it = m->find(to);
        if (it == m->end() || w < it->second)
            (*m)[to] = w;
    }

    // incoming edges to "vertex"
    for (int from = 0; from < m_Size; ++from)
    {
        if (from == vertex) continue;

        auto itEdge = m_List[from].find(vertex);
        if (itEdge != m_List[from].end())
        {
            int to = from;
            int w  = itEdge->second;

            auto it = m->find(to);
            if (it == m->end() || w < it->second)
                (*m)[to] = w;
        }
    }
}

// Directed adjacency : only outgoing edges

void ListGraph::getAdjacentEdgesDirect(int vertex, map<int, int>* m)
{
    m->clear();
    if (vertex < 0 || vertex >= m_Size) return;

    for (auto &p : m_List[vertex])
    {
        (*m)[p.first] = p.second;
    }
}

// Insert directed edge
void ListGraph::insertEdge(int from, int to, int weight)
{
    if (from < 0 || from >= m_Size) return;
    if (to   < 0 || to   >= m_Size) return;

    m_List[from][to] = weight;
}

// PRINT part
bool ListGraph::printGraph(ofstream *fout)
{
    if (fout == nullptr || !(*fout))
        return false;

    ofstream& out = *fout;

    out << "========PRINT========" << endl;
    for (int i = 0; i < m_Size; ++i)
    {
        out << "[" << i << "]";

        if (!m_List[i].empty())
            out << " -> ";

        bool first = true;
        for (auto it = m_List[i].begin(); it != m_List[i].end(); ++it)
        {
            if (!first)
                out << " -> ";
            out << "(" << it->first << "," << it->second << ")";
            first = false;
        }
        out << endl;
    }
    out << "=======================" << endl << endl;

    return true;
}
