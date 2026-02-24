#include "Graph.h"

Graph::Graph(bool type, int size)
{
	m_Type = type;
	m_Size = size;
}

Graph::~Graph()	
{

}

// get type
bool Graph::getType(){return m_Type;}	
// get size
int Graph::getSize(){return m_Size;}