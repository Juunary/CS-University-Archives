# Concurrent Proxy — Semaphore Synchronization

## Overview

A project that synchronizes concurrent access to shared resources (cache files, logs) in a multi-process proxy server using System V semaphores.

## Solving Concurrency Issues

Using semaphores to resolve Race Conditions that occur when multiple child processes access the same cache file simultaneously.

### Synchronization Flow

```
Child Process A      Child Process B
     │                    │
  semop(-1) ──────────────┤ (Wait)
  Write Cache             │
  semop(+1) ─────────────►│
                        semop(-1)
                        Write Cache
                        semop(+1)
```

## Core Technologies

- System V Semaphores (`semget`, `semop`, `semctl`)
- Definition and initialization of `union semun`
- Critical section protection using Mutex pattern

## File Structure

```
08_Concurrent_Proxy_Semaphore_Sync/
├── synchronized_proxy.c — Semaphore Synchronized Proxy
├── Makefile
└── Report_Semaphore_Synchronization.pdf
```

## Build and Run

```bash
make
./synchronized_proxy
```
