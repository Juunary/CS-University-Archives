#include <iostream>
#include <vector>
#include "GraphMethod.h"
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <list>
#include <utility>
#include <limits>
#include <algorithm>
#include <cctype>

using namespace std;

// log for commu with Manager
ofstream* logOutput = nullptr;

// Large value (used to represent infinite distance)
static const long long INF = std::numeric_limits<long long>::max() / 4;


// Check for negative edges (used by Dijkstra)
static bool hasNegativeEdge(Graph* graph)
{
    if (!graph) return false;
    int size = graph->getSize();

    for (int i = 0; i < size; ++i)
    {
        map<int,int> adj;
        graph->getAdjacentEdgesDirect(i, &adj); // based on stored directed graph

        for (auto &p : adj)
        {
            if (p.second < 0) return true;
        }
    }
    return false;
}

// Check connectivity for undirected graph (used by Kruskal)
static bool isConnectedUndirected(Graph* graph)
{
    if (!graph) return false;
    int size = graph->getSize();
    if (size == 0) return false;

    vector<bool> visited(size, false);
    queue<int> q;

    visited[0] = true;
    q.push(0);

    while (!q.empty())
    {
        int v = q.front(); q.pop();
        map<int,int> adj;
        graph->getAdjacentEdges(v, &adj); // undirected

        for (auto &p : adj)
        {
            int nxt = p.first;
            if (nxt < 0 || nxt >= size) continue;
            if (!visited[nxt])
            {
                visited[nxt] = true;
                q.push(nxt);
            }
        }
    }

    for (int i = 0; i < size; ++i)
    {
        if (!visited[i]) return false;
    }
    return true;
}

// Floyd-Warshall (build distance matrix without printing)
// option == 'O': directed, 'X': undirected
static bool buildFloydMatrix(Graph* graph, char option, vector< vector<long long> >& dist)
{
    if (!graph) return false;
    int size = graph->getSize();

    dist.assign(size, vector<long long>(size, INF));

    for (int i = 0; i < size; ++i)
        dist[i][i] = 0;

    for (int i = 0; i < size; ++i)
    {
        map<int,int> adj;

        if (option == 'O')          // directed
            graph->getAdjacentEdgesDirect(i, &adj);
        else                        // undirected
            graph->getAdjacentEdges(i, &adj);

        for (auto &p : adj)
        {
            int j = p.first;
            int w = p.second;
            if (j < 0 || j >= size) continue;

            if (dist[i][j] > (long long)w)
                dist[i][j] = (long long)w;
        }
    }

    // Floyd core
    for (int k = 0; k < size; ++k)
    {
        for (int i = 0; i < size; ++i)
        {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < size; ++j)
            {
                if (dist[k][j] == INF) continue;
                long long cand = dist[i][k] + dist[k][j];
                if (cand < dist[i][j])
                    dist[i][j] = cand;
            }
        }
    }

    // Check for negative cycles
    for (int i = 0; i < size; ++i)
    {
        if (dist[i][i] < 0) return false;
    }
    return true;
}

// DFS recursive function
static void dfsRecursive(Graph* graph, char option, int vertex,
                         vector<bool>& visited, vector<int>& order)
{
    int size = graph->getSize();
    if (vertex < 0 || vertex >= size) return;

    visited[vertex] = true;
    order.push_back(vertex);

    map<int,int> adj;
    if (option == 'O')
        graph->getAdjacentEdgesDirect(vertex, &adj);
    else
        graph->getAdjacentEdges(vertex, &adj);

    for (auto &p : adj)
    {
        int nxt = p.first;
        if (nxt < 0 || nxt >= size) continue;
        if (!visited[nxt])
            dfsRecursive(graph, option, nxt, visited, order);
    }
}


bool BFS(Graph* graph, char option, int vertex)
{
    if (!graph || !logOutput) return false;

    int size = graph->getSize();
    if (vertex < 0 || vertex >= size) return false;

    option = std::toupper(option);
    if (option != 'O' && option != 'X') return false;

    vector<bool> visited(size, false);
    queue<int> q;

    visited[vertex] = true;
    q.push(vertex);

    ofstream& fout = *logOutput;

    fout << "========BFS========" << endl;
    if (option == 'O')
        fout << "Directed Graph BFS" << endl;
    else
        fout << "Undirected Graph BFS" << endl;
    fout << "Start: " << vertex << endl;

    bool first = true;

    while (!q.empty())
    {
        int v = q.front(); q.pop();

        if (!first) fout << " -> ";
        first = false;
        fout << v;

        map<int,int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(v, &adj);
        else
            graph->getAdjacentEdges(v, &adj);

        for (auto &p : adj)
        {
            int nxt = p.first;
            if (nxt < 0 || nxt >= size) continue;
            if (!visited[nxt])
            {
                visited[nxt] = true;
                q.push(nxt);
            }
        }
    }

    fout << endl;
    fout << "=======================" << endl << endl;
    return true;
}


bool DFS(Graph* graph, char option, int vertex)
{
    if (!graph || !logOutput) return false;

    int size = graph->getSize();
    if (vertex < 0 || vertex >= size) return false;

    option = std::toupper(option);
    if (option != 'O' && option != 'X') return false;

    vector<bool> visited(size, false);
    vector<int> order;

    dfsRecursive(graph, option, vertex, visited, order);

    ofstream& fout = *logOutput;

    fout << "========DFS========" << endl;
    if (option == 'O')
        fout << "Directed Graph DFS" << endl;
    else
        fout << "Undirected Graph DFS" << endl;
    fout << "Start: " << vertex << endl;

    for (size_t i = 0; i < order.size(); ++i)
    {
        if (i > 0) fout << " -> ";
        fout << order[i];
    }
    fout << endl;
    fout << "=======================" << endl << endl;

    return true;
}


struct EdgeK
{
    int u;
    int v;
    int w;
};

static int findParent(vector<int>& parent, int x)
{
    if (parent[x] == x) return x;
    return parent[x] = findParent(parent, parent[x]);
}

static void unionParent(vector<int>& parent, int a, int b)
{
    a = findParent(parent, a);
    b = findParent(parent, b);
    if (a == b) return;
    if (a < b) parent[b] = a;
    else parent[a] = b;
}

bool Kruskal(Graph* graph)
{
    if (!graph || !logOutput) return false;
    int size = graph->getSize();
    if (size <= 0) return false;

    ofstream& fout = *logOutput;

    // Error if there are unreachable vertices
    if (!isConnectedUndirected(graph))
        return false;

    // Collect edges (undirected, remove duplicates)
    map< pair<int,int>, int > edgeMap;

    for (int i = 0; i < size; ++i)
    {
        map<int,int> adj;
        graph->getAdjacentEdges(i, &adj); // undirected

        for (auto &p : adj)
        {
            int j = p.first;
            int w = p.second;
            if (j < 0 || j >= size) continue;

            int a = std::min(i, j);
            int b = std::max(i, j);
            pair<int,int> key = make_pair(a, b);

            auto it = edgeMap.find(key);
            if (it == edgeMap.end() || w < it->second)
                edgeMap[key] = w;
        }
    }

    vector<EdgeK> edges;
    edges.reserve(edgeMap.size());

    for (auto &e : edgeMap)
    {
        EdgeK ed;
        ed.u = e.first.first;
        ed.v = e.first.second;
        ed.w = e.second;
        edges.push_back(ed);
    }

    std::sort(edges.begin(), edges.end(),
              [](const EdgeK& a, const EdgeK& b){ return a.w < b.w; });

    vector<int> parent(size);
    for (int i = 0; i < size; ++i) parent[i] = i;

    vector< vector< pair<int,int> > > mstAdj(size);
    long long totalCost = 0;
    int edgeCount = 0;

    for (size_t i = 0; i < edges.size() && edgeCount < size - 1; ++i)
    {
        int u = edges[i].u;
        int v = edges[i].v;
        int w = edges[i].w;

        int pu = findParent(parent, u);
        int pv = findParent(parent, v);
        if (pu == pv) continue;

        unionParent(parent, pu, pv);

        mstAdj[u].push_back(make_pair(v, w));
        mstAdj[v].push_back(make_pair(u, w));
        totalCost += w;
        ++edgeCount;
    }

    // Output
    fout << "========KRUSKAL========" << endl;
    for (int i = 0; i < size; ++i)
    {
        std::sort(mstAdj[i].begin(), mstAdj[i].end(),
                  [](const pair<int,int>& a, const pair<int,int>& b)
                  {
                      return a.first < b.first;
                  });

        fout << "[" << i << "]";

        if (!mstAdj[i].empty())
            fout << " ";

        for (size_t j = 0; j < mstAdj[i].size(); ++j)
        {
            fout << mstAdj[i][j].first << "(" << mstAdj[i][j].second << ")";
            if (j + 1 < mstAdj[i].size())
                fout << " ";
        }
        fout << endl;
    }
    fout << "Cost: " << totalCost << endl;
    fout << "=======================" << endl << endl;

    return true;
}


bool Dijkstra(Graph* graph, char option, int vertex)
{
    if (!graph || !logOutput) return false;
    int size = graph->getSize();
    if (vertex < 0 || vertex >= size) return false;

    option = std::toupper(option);
    if (option != 'O' && option != 'X') return false;

    ofstream& fout = *logOutput;

    // Error if negative weights exist
    if (hasNegativeEdge(graph))
        return false;

    vector<long long> dist(size, INF);
    vector<int> prev(size, -1);
    vector<bool> selected(size, false);

    dist[vertex] = 0;

    for (int i = 0; i < size; ++i)
    {
        long long minVal = INF;
        int u = -1;

        for (int j = 0; j < size; ++j)
        {
            if (!selected[j] && dist[j] < minVal)
            {
                minVal = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        selected[u] = true;

        map<int,int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(u, &adj);
        else
            graph->getAdjacentEdges(u, &adj);

        for (auto &p : adj)
        {
            int v = p.first;
            int w = p.second;
            if (v < 0 || v >= size) continue;
            if (dist[u] == INF) continue;

            long long cand = dist[u] + w;
            if (cand < dist[v])
            {
                dist[v] = cand;
                prev[v] = u;
            }
        }
    }

    fout << "========DIJKSTRA========" << endl;
    if (option == 'O')
        fout << "Directed Graph Dijkstra" << endl;
    else
        fout << "Undirected Graph Dijkstra" << endl;
    fout << "Start: " << vertex << endl;

    for (int i = 0; i < size; ++i)
    {
        fout << "[" << i << "] ";

        if (i == vertex)
        {
            fout << vertex << " (0)" << endl;
            continue;
        }

        if (dist[i] == INF)
        {
            fout << "x" << endl;
            continue;
        }

        vector<int> path;
        int cur = i;
        while (cur != -1)
        {
            path.push_back(cur);
            cur = prev[cur];
        }
        std::reverse(path.begin(), path.end());

        for (size_t k = 0; k < path.size(); ++k)
        {
            if (k > 0) fout << " -> ";
            fout << path[k];
        }
        fout << " (" << dist[i] << ")" << endl;
    }

    fout << "=======================" << endl << endl;

    return true;
}


struct EdgeBF
{
    int u;
    int v;
    int w;
};

bool Bellmanford(Graph* graph, char option, int s_vertex, int e_vertex) 
{
    if (!graph || !logOutput) return false;

    int size = graph->getSize();
    if (s_vertex < 0 || s_vertex >= size) return false;
    if (e_vertex < 0 || e_vertex >= size) return false;

    option = std::toupper(option);
    if (option != 'O' && option != 'X') return false;

    ofstream& fout = *logOutput;

    vector<EdgeBF> edges;
    edges.reserve(size * size);

    // Build edge list
    for (int i = 0; i < size; ++i)
    {
        map<int,int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(i, &adj);
        else
            graph->getAdjacentEdges(i, &adj);

        for (auto &p : adj)
        {
            EdgeBF e;
            e.u = i;
            e.v = p.first;
            e.w = p.second;
            edges.push_back(e);
        }
    }

    vector<long long> dist(size, INF);
    vector<int> prev(size, -1);
    dist[s_vertex] = 0;

    // Relax |V|-1 times
    for (int i = 0; i < size - 1; ++i)
    {
        bool updated = false;
        for (const auto& e : edges)
        {
            if (dist[e.u] == INF) continue;
            long long cand = dist[e.u] + e.w;
            if (cand < dist[e.v])
            {
                dist[e.v] = cand;
                prev[e.v] = e.u;
                updated = true;
            }
        }
        if (!updated) break;
    }

    // Check for negative cycles
    for (const auto& e : edges)
    {
        if (dist[e.u] == INF) continue;
        long long cand = dist[e.u] + e.w;
        if (cand < dist[e.v])
        {
            return false; // negative cycle
        }
    }

    fout << "========BELLMANFORD========" << endl;
    if (option == 'O')
        fout << "Directed Graph Bellman-Ford" << endl;
    else
        fout << "Undirected Graph Bellman-Ford" << endl;

    if (dist[e_vertex] == INF)
    {
        // Unreachable: print only 'x'
        fout << "x" << endl;
        fout << "=======================" << endl << endl;
        return true;
    }

    vector<int> path;
    int cur = e_vertex;
    while (cur != -1)
    {
        path.push_back(cur);
        cur = prev[cur];
    }
    std::reverse(path.begin(), path.end());

    if (path.front() != s_vertex)
    {
        fout << "x" << endl;
        fout << "=======================" << endl << endl;
        return true;
    }

    for (size_t i = 0; i < path.size(); ++i)
    {
        if (i > 0) fout << " -> ";
        fout << path[i];
    }
    fout << endl;
    fout << "Cost: " << dist[e_vertex] << endl;
    fout << "=======================" << endl << endl;

    return true;
}


bool FLOYD(Graph* graph, char option)
{
    if (!graph || !logOutput) return false;

    option = std::toupper(option);
    if (option != 'O' && option != 'X') return false;

    ofstream& fout = *logOutput;

    vector< vector<long long> > dist;
    if (!buildFloydMatrix(graph, option, dist))
    {
        // negative cycle
        return false;
    }

    int size = graph->getSize();

    fout << "========FLOYD========" << endl;
    if (option == 'O')
        fout << "Directed Graph Floyd" << endl;
    else
        fout << "Undirected Graph Floyd" << endl;

    // header
    fout << "    ";
    for (int j = 0; j < size; ++j)
    {
        fout << "[" << j << "]";
        if (j < size - 1) fout << " ";
    }
    fout << endl;

    // matrix
    for (int i = 0; i < size; ++i)
    {
        fout << "[" << i << "] ";
        for (int j = 0; j < size; ++j)
        {
            if (dist[i][j] == INF)
                fout << "x";
            else
                fout << dist[i][j];

            if (j < size - 1) fout << " ";
        }
        fout << endl;
    }

    fout << "=======================" << endl << endl;
    return true;
}


bool Centrality(Graph* graph) 
{
    if (!graph || !logOutput) return false;

    ofstream& fout = *logOutput;

    int size = graph->getSize();
    if (size <= 0) return false;

    // Use undirected Floyd results
    vector< vector<long long> > dist;
    if (!buildFloydMatrix(graph, 'X', dist))
    {
        // negative cycle
        return false;
    }

    vector<bool> valid(size, false);
    vector<long long> denom(size, 0);

    long long bestDenom = 0;
    bool hasBest = false;

    for (int v = 0; v < size; ++v)
    {
        bool unreachable = false;
        long long sum = 0;

        for (int u = 0; u < size; ++u)
        {
            if (u == v) continue;
            // from all other vertices to v
            if (dist[u][v] == INF)
            {
                unreachable = true;
                break;
            }
            sum += dist[u][v];
        }

        if (unreachable || sum == 0)
        {
            valid[v] = false;
        }
        else
        {
            valid[v] = true;
            denom[v] = sum;

            if (!hasBest || sum < bestDenom)
            {
                hasBest = true;
                bestDenom = sum;
            }
        }
    }

    fout << "========CENTRALITY========" << endl;

    int Nminus1 = size - 1;
    for (int v = 0; v < size; ++v)
    {
        fout << "[" << v << "] ";
        if (!valid[v])
        {
            fout << "x";
        }
        else
        {
            fout << Nminus1 << "/" << denom[v];
            if (hasBest && denom[v] == bestDenom)
                fout << " <- Most Central";
        }
        fout << endl;
    }

    fout << "=======================" << endl << endl;

    return true;
}
