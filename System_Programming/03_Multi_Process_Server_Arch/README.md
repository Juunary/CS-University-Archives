# Multi-Process Server Architecture

## Overview

A multi-process server architecture that creates a child process for each client request using `fork()`. Extends proxy cache functionality to a multi-process structure.

## How it Works

1. Parent Process: Wait for connection requests (accept loop)
2. Call `fork()` on client connection
3. Child Process: Handle request (Proxy cache lookup/store)
4. Handle `SIGCHLD` on child process exit to prevent zombie processes

## Core Technologies

- `fork()` based multi-process
- `SIGCHLD` signal handling (`waitpid`)
- Child process count management

## File Structure

```
03_Multi_Process_Server_Arch/
├── proxy_cache.c        — Multi-process proxy server
├── homedir.c
├── Makefile
└── Report_Process_Fork_Management.pdf
```

## Build and Run

```bash
make
./proxy_cache
```
