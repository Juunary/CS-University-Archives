#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "GraphMethod.h"

class Manager{	
private:
	Graph* graph;	
	ofstream fout;	
	int load;

public:
	Manager();	
	~Manager();	

	void run(const char * command_txt);
	
	// Load graph from file
	bool LOAD(const char* filename);

	// Print graph
	bool PRINT();

	// Run BFS
	bool mBFS(char option, int vertex);

	// Run DFS
	bool mDFS(char option, int vertex);

	// Run Dijkstra
	bool mDIJKSTRA(char option, int vertex);

	// Run Kruskal
	bool mKRUSKAL();

	// Run Bellman-Ford
	bool mBELLMANFORD(char option, int s_vertex, int e_vertex);

	// Run Floyd–Warshall
	bool mFLOYD(char option);

	// Run Centrality
	bool mCentrality();

	// Print error code
	void printErrorCode(int n);

};

#endif
