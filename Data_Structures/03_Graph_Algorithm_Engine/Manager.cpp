#include "Manager.h"
#include "GraphMethod.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <limits>

using namespace std;

// log stream pointer defined in GraphMethod.cpp
extern ofstream* logOutput;

// Limit to prevent memory exhaustion
namespace {
	const int MAX_VERTICES = 10000;
}

Manager::Manager()	
{
	graph = nullptr;

	// Create log.txt and truncate existing content
	fout.open("log.txt", ios::trunc);
	load = 0;	// no data loaded

	// Connect shared log stream used by GraphMethod
	logOutput = &fout;
}

Manager::~Manager()
{
	if (graph != nullptr)
	{
		delete graph;
		graph = nullptr;
	}

	if (fout.is_open())
		fout.close();

	logOutput = nullptr;
}

void Manager::run(const char * command_txt)
{
	ifstream fin(command_txt);
	if (!fin.is_open())
		return; // exit silently if command file cannot be opened

	string line;

	while (getline(fin, line))
	{
		if (line.empty())
			continue;

		stringstream ss(line);
		string cmd;
		ss >> cmd;

		if (cmd.empty())
			continue;

		if (cmd == "LOAD")
		{
			string filename, extra;
			if (!(ss >> filename) || (ss >> extra))
			{
				printErrorCode(100);
				continue;
			}

			if (LOAD(filename.c_str()))
			{
				fout << "========LOAD========" << endl;
				fout << "Success" << endl;
				fout << "====================" << endl << endl;
			}
			else
			{
				printErrorCode(100);
			}
		}
		else if (cmd == "PRINT")
		{
			string extra;
			if (ss >> extra)
			{
				printErrorCode(200);
				continue;
			}

			if (!graph)
			{
				printErrorCode(200);
				continue;
			}

			if (!PRINT())
				printErrorCode(200);
		}
		else if (cmd == "BFS")
		{
			char option;
			long long vertexLL;   // temporary to prevent overflow
			string extra;

			if (!(ss >> option >> vertexLL) || (ss >> extra))
			{
				printErrorCode(300);
				continue;
			}

			if (!graph)
			{
				printErrorCode(300);
				continue;
			}

			option = static_cast<char>(toupper(static_cast<unsigned char>(option)));
			if (option != 'O' && option != 'X')
			{
				printErrorCode(300);
				continue;
			}

			// check vertex and type range
			if (vertexLL < 0 || 
				vertexLL > numeric_limits<int>::max())
			{
				printErrorCode(300);
				continue;
			}

			int vertex = static_cast<int>(vertexLL);

			if (vertex < 0 || vertex >= graph->getSize())
			{
				printErrorCode(300);
				continue;
			}

			if (!mBFS(option, vertex))
				printErrorCode(300);
		}
		else if (cmd == "DFS")
		{
			char option;
			long long vertexLL;
			string extra;

			if (!(ss >> option >> vertexLL) || (ss >> extra))
			{
				printErrorCode(400);
				continue;
			}

			if (!graph)
			{
				printErrorCode(400);
				continue;
			}

			option = static_cast<char>(toupper(static_cast<unsigned char>(option)));
			if (option != 'O' && option != 'X')
			{
				printErrorCode(400);
				continue;
			}

			if (vertexLL < 0 || 
				vertexLL > numeric_limits<int>::max())
			{
				printErrorCode(400);
				continue;
			}

			int vertex = static_cast<int>(vertexLL);

			if (vertex < 0 || vertex >= graph->getSize())
			{
				printErrorCode(400);
				continue;
			}

			if (!mDFS(option, vertex))
				printErrorCode(400);
		}
		else if (cmd == "KRUSKAL")
		{
			string extra;
			if (ss >> extra)
			{
				printErrorCode(500);
				continue;
			}

			if (!graph)
			{
				printErrorCode(500);
				continue;
			}

			if (!mKRUSKAL())
				printErrorCode(500);
		}
		else if (cmd == "DIJKSTRA")
		{
			char option;
			long long vertexLL;
			string extra;

			if (!(ss >> option >> vertexLL) || (ss >> extra))
			{
				printErrorCode(600);
				continue;
			}

			if (!graph)
			{
				printErrorCode(600);
				continue;
			}

			option = static_cast<char>(toupper(static_cast<unsigned char>(option)));
			if (option != 'O' && option != 'X')
			{
				printErrorCode(600);
				continue;
			}

			if (vertexLL < 0 || 
				vertexLL > numeric_limits<int>::max())
			{
				printErrorCode(600);
				continue;
			}

			int vertex = static_cast<int>(vertexLL);

			if (vertex < 0 || vertex >= graph->getSize())
			{
				printErrorCode(600);
				continue;
			}

			if (!mDIJKSTRA(option, vertex))
				printErrorCode(600);
		}
		else if (cmd == "BELLMANFORD")
		{
			char option;
			long long sLL, eLL;
			string extra;

			if (!(ss >> option >> sLL >> eLL) || (ss >> extra))
			{
				printErrorCode(700);
				continue;
			}

			if (!graph)
			{
				printErrorCode(700);
				continue;
			}

			option = static_cast<char>(toupper(static_cast<unsigned char>(option)));
			if (option != 'O' && option != 'X')
			{
				printErrorCode(700);
				continue;
			}

			// check start and end vertex ranges and types
			if (sLL < 0 || sLL > numeric_limits<int>::max() ||
				eLL < 0 || eLL > numeric_limits<int>::max())
			{
				printErrorCode(700);
				continue;
			}

			int s_vertex = static_cast<int>(sLL);
			int e_vertex = static_cast<int>(eLL);

			if (s_vertex < 0 || s_vertex >= graph->getSize() ||
				e_vertex < 0 || e_vertex >= graph->getSize())
			{
				printErrorCode(700);
				continue;
			}

			if (!mBELLMANFORD(option, s_vertex, e_vertex))
				printErrorCode(700);
		}
		else if (cmd == "FLOYD")
		{
			char option;
			string extra;

			if (!(ss >> option) || (ss >> extra))
			{
				printErrorCode(800);
				continue;
			}

			if (!graph)
			{
				printErrorCode(800);
				continue;
			}

			option = static_cast<char>(toupper(static_cast<unsigned char>(option)));
			if (option != 'O' && option != 'X')
			{
				printErrorCode(800);
				continue;
			}

			if (!mFLOYD(option))
				printErrorCode(800);
		}
		else if (cmd == "CENTRALITY")
		{
			string extra;
			if (ss >> extra)
			{
				printErrorCode(900);
				continue;
			}

			if (!graph)
			{
				printErrorCode(900);
				continue;
			}

			if (!mCentrality())
				printErrorCode(900);
		}
		else if (cmd == "EXIT")
		{
			// EXIT should have no arguments ignore lines with extra arguments
			string extra;
			if (ss >> extra)
			{
				// No error code specified for EXIT skip line and continue
				continue;
			}

			fout << "========EXIT========" << endl;
			fout << "Success" << endl;
			fout << "====================" << endl << endl;
			return; // terminate program
		}
		// ignore other commands outside spec
	}
}

// LOAD : construct graph from graph_L or graph_M
bool Manager::LOAD(const char* filename)	
{
	ifstream in(filename);
	if (!in.is_open())
		return false;

	char typeChar;
	long long sizeLL;

	// Read graph type and vertex count
	if (!(in >> typeChar >> sizeLL))
		return false;

	typeChar = static_cast<char>(toupper(static_cast<unsigned char>(typeChar)));

	// Validate vertex count
	if (sizeLL <= 0 || sizeLL > MAX_VERTICES)
		return false;

	int size = static_cast<int>(sizeLL);

	// Delete existing graph
	if (graph != nullptr)
	{
		delete graph;
		graph = nullptr;
	}

	if (typeChar == 'L')   // adjacency list
	{
		graph = new ListGraph(false, size);

		// Clear remaining newline after vertex count
		string dummy;
		getline(in, dummy);

		int from = -1;
		string line;

		while (getline(in, line))
		{
			if (line.empty())
				continue;

			stringstream ss(line);
			long long aLL, bLL;

			// If first number fails to read skip line
			if (!(ss >> aLL))
				continue;

			// If second number exists it's an edge line otherwise it's a vertex line
			if (ss >> bLL)
			{
				// edge line current from to aLL with weight bLL
				if (from < 0 || from >= size)
					continue; // skip if from is invalid

				if (aLL < 0 || aLL >= size)
					continue; // skip if to is out of range

				// check weight fits in int
				if (bLL < std::numeric_limits<int>::min() ||
					bLL > std::numeric_limits<int>::max())
					continue; // skip absurd weights

				int to = static_cast<int>(aLL);
				int w  = static_cast<int>(bLL);

				graph->insertEdge(from, to, w);
			}
			else
			{
				// vertex line set from
				if (aLL < 0 || aLL >= size)
				{
					// invalid vertex number ignore following edges
					from = -1;
				}
				else
				{
					from = static_cast<int>(aLL);
				}
			}
		}
	}
	else if (typeChar == 'M')  // adjacency matrix
	{
		graph = new MatrixGraph(true, size);

		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				long long wLL;
				if (!(in >> wLL))
				{
					// fail if matrix cannot be read fully
					return false;
				}

				if (wLL < std::numeric_limits<int>::min() ||
					wLL > std::numeric_limits<int>::max())
				{
					// fail if weight out of int range
					return false;
				}

				int w = static_cast<int>(wLL);

				if (w != 0)
					graph->insertEdge(i, j, w);
			}
		}
	}
	else
	{
		// type character is not L or M
		return false;
	}

	load = 1;
	return true;
}

// PRINT
bool Manager::PRINT()
{
	if (!graph) return false;
	return graph->printGraph(&fout);
}

// Wrapper functions for algorithm calls
bool Manager::mBFS(char option, int vertex)
{
	return BFS(graph, option, vertex);
}

bool Manager::mDFS(char option, int vertex)
{
	return DFS(graph, option, vertex);
}

bool Manager::mDIJKSTRA(char option, int vertex)
{
	return Dijkstra(graph, option, vertex);
}

bool Manager::mKRUSKAL()
{
	return Kruskal(graph);
}

bool Manager::mBELLMANFORD(char option, int s_vertex, int e_vertex)
{
	return Bellmanford(graph, option, s_vertex, e_vertex);
}

bool Manager::mFLOYD(char option)
{
	return FLOYD(graph, option);
}

bool Manager::mCentrality()
{
	return Centrality(graph);
}

// Print error code
void Manager::printErrorCode(int n)
{
	fout << "========ERROR=======" << endl;
	fout << n << endl;
	fout << "====================" << endl << endl;
}
