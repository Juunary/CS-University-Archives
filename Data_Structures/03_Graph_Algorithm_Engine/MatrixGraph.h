#ifndef _MATRIX_H_
#define _MATRIX_H_
#include <map>
#include "Graph.h"

class MatrixGraph : public Graph{	
private:
	int** m_Mat;

public:
	MatrixGraph(bool type, int size);
	~MatrixGraph();
		
	// Get undirected adjacent edges
	void getAdjacentEdges(int vertex, map<int, int>* m);

	// Get directed adjacent edges
	void getAdjacentEdgesDirect(int vertex, map<int, int>* m);

	// Insert a directed edge
	void insertEdge(int from, int to, int weight);

	// Print the graph
	bool printGraph(ofstream *fout);

};

#endif