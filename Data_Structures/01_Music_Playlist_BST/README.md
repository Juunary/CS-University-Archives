# Music Playlist BST

## Overview

A C++ project that manages a music library and playlists using Binary Search Tree (BST) as the core data structure.

## Data Structure Composition

```
Manager
├── MusicQueue      — Input queue (music loading)
├── TitleBST        — BST by title (search/delete)
├── ArtistBST       — BST by artist (search/delete)
└── PlayList        — Linked list-based playlist
```

## Supported Commands

| Command | Description |
|---------|-------------|
| `LOAD` | Load music data from file |
| `ADD` | Dequeue song and add to BST |
| `QPOP` | Remove song from queue |
| `SEARCH` | Search by title or artist |
| `MAKEPL` | Create playlist |
| `PRINT` | Print BST or playlist |
| `DELETE` | Delete song |

## File Structure

| File | Description |
|------|-------------|
| `main.cpp` | Entry point, Manager execution |
| `Manager.h/.cpp` | Command parsing and overall control flow |
| `TitleBST.h/.cpp` | BST by title |
| `ArtistBST.h/.cpp` | BST by artist |
| `MusicQueue.h/.cpp` | Music input queue |
| `PlayList.h/.cpp` | Playlist linked list |

