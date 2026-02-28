# Networked Proxy Server — Socket Programming

## Overview

A network proxy server with Client-Server structure using TCP sockets. When a client sends a URL to the server, the server looks up the cache and returns the result.

## Architecture

```
Client ──TCP──► Server (fork per client)
                  │
                  ▼
             ~/cache/ (SHA1 hash based)
```

## Socket Configuration

- Server Port: `39999`
- Socket Type: TCP (SOCK_STREAM)
- Multi-client: Based on fork()

## File Structure

```
04_Networked_Proxy_Server_Socket/
├── server.c             — Server (socket bind/listen/accept + fork)
├── client.c             — Client (connect + send URL)
├── Makefile
└── Report_Socket_Programming_Arch.pdf
```

## Build and Run

```bash
make
./server &
./client
```
