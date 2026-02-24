#include "MatrixGraph.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

MatrixGraph::MatrixGraph(bool type, int size) : Graph(type, size)
{
    m_Mat = new int*[m_Size];
    for (int i = 0; i < m_Size; ++i)
    {
        m_Mat[i] = new int[m_Size];
        for (int j = 0; j < m_Size; ++j)
            m_Mat[i][j] = 0;
    }
}

MatrixGraph::~MatrixGraph()
{
    if (m_Mat)
    {
        for (int i = 0; i < m_Size; ++i)
            delete [] m_Mat[i];
        delete [] m_Mat;
        m_Mat = nullptr;
    }
}

// Undirected adjacency: row + column
void MatrixGraph::getAdjacentEdges(int vertex, map<int, int>* m)
{
    m->clear();
    if (vertex < 0 || vertex >= m_Size) return;

    // outgoing edges
    for (int to = 0; to < m_Size; ++to)
    {
        int w = m_Mat[vertex][to];
        if (w != 0)
        {
            auto it = m->find(to);
            if (it == m->end() || w < it->second)
                (*m)[to] = w;
        }
    }

    // incoming edges
    for (int from = 0; from < m_Size; ++from)
    {
        int w = m_Mat[from][vertex];
        if (w != 0)
        {
            int to = from;
            auto it = m->find(to);
            if (it == m->end() || w < it->second)
                (*m)[to] = w;
        }
    }
}

// Directed adjacency: only row
void MatrixGraph::getAdjacentEdgesDirect(int vertex, map<int, int>* m)
{
    m->clear();
    if (vertex < 0 || vertex >= m_Size) return;

    for (int to = 0; to < m_Size; ++to)
    {
        int w = m_Mat[vertex][to];
        if (w != 0)
            (*m)[to] = w;
    }
}

// Insert edge (from -> to)
void MatrixGraph::insertEdge(int from, int to, int weight)	
{
    if (from < 0 || from >= m_Size) return;
    if (to   < 0 || to   >= m_Size) return;

    m_Mat[from][to] = weight;
}

// PRINT matrix
bool MatrixGraph::printGraph(ofstream *fout)	
{
    if (fout == nullptr || !(*fout))
        return false;

    ofstream& out = *fout;

    out << "========PRINT========" << endl;

    // header row
    out << "    ";
    for (int j = 0; j < m_Size; ++j)
    {
        out << "[" << j << "]";
        if (j < m_Size - 1) out << " ";
    }
    out << endl;

    // matrix rows
    for (int i = 0; i < m_Size; ++i)
    {
        out << "[" << i << "] ";
        for (int j = 0; j < m_Size; ++j)
        {
            out << m_Mat[i][j];
            if (j < m_Size - 1) out << " ";
        }
        out << endl;
    }

    out << "=======================" << endl << endl;
    return true;
}
