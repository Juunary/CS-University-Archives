# Graph Algorithm Engine

## Overview

A C++ project that supports both graph representation methods (adjacency matrix and adjacency list) and implements major graph algorithms.

## Supported Algorithms

| Algorithm | Command | Description |
|-----------|---------|-------------|
| BFS | `mBFS` | Breadth-First Search |
| DFS | `mDFS` | Depth-First Search |
| Dijkstra | `mDIJKSTRA` | Single-Source Shortest Path (non-negative weights) |
| Kruskal | `mKRUSKAL` | Minimum Spanning Tree (MST) |
| Bellman-Ford | `mBELLMANFORD` | Single-Source Shortest Path (negative weights allowed) |
| Floyd-Warshall | `mFLOYD` | All-Pairs Shortest Path |
| Centrality | `mCentrality` | Node Centrality Calculation |

## Graph Representation

- **MatrixGraph**: Adjacency matrix representation
- **ListGraph**: Adjacency list representation
- Selectable via command-line options

## File Structure

| File | Description |
|------|-------------|
| `main.cpp` | Entry point |
| `Manager.h/.cpp` | Command parsing and control flow |
| `Graph.h/.cpp` | Graph abstract base class |
| `MatrixGraph.h/.cpp` | Adjacency matrix graph |
| `ListGraph.h/.cpp` | Adjacency list graph |
| `GraphMethod.h/.cpp` | Algorithm implementations |

