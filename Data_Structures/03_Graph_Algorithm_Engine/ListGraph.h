#ifndef _LIST_H_
#define _LIST_H_

#include "Graph.h"

class ListGraph : public Graph{	
private:
	map < int, int >* m_List;
	vector<int>* kw_graph;
	
public:	
	ListGraph(bool type, int size);
	~ListGraph();
		
	// Fills m with adjacent edges of the given vertex in an undirected view
	void getAdjacentEdges(int vertex, map<int, int>* m);

	// Fills m with outgoing edges of the given vertex in a directed view
	void getAdjacentEdgesDirect(int vertex, map<int, int>* m);

	// Inserts a directed edge from from~to with the specified weight
	void insertEdge(int from, int to, int weight);

	// Prints the graph structure to the output stream 
	bool printGraph(ofstream *fout);

};

#endif