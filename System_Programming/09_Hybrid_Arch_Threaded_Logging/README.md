# Hybrid Architecture — Threaded Logging

## Overview

A hybrid architecture proxy server combining multi-process (fork) based request handling and multi-thread (pthread) based logging.

## Architecture

```
Main Process (accept loop)
    │
    ├──fork()──► Child Process (Request Handling)
    │                │
    │            pthread_create()──► Logging Thread (Async Logging)
    │                │
    │            Cache Lookup/Store (Semaphore Sync)
    │
    └──fork()──► Child Process ...
```

## Advantages of Hybrid Design

| Processing Method | Task | Reason |
|-------------------|------|--------|
| `fork()` Multi-process | HTTP Request Handling | Process isolation ensures stability |
| `pthread` Multi-thread | Log File Writing | Efficient logging via shared memory |

## Core Technologies

- Log thread creation using `pthread_create` / `pthread_join`
- Passing log parameters to threads via struct (`log_param`)
- Concurrent cache access control using System V Semaphores

## File Structure

```
09_Hybrid_Arch_Threaded_Logging/
├── threaded_proxy.c     — Hybrid Proxy Server
├── Makefile
└── Report_Hybrid_Threading_Design.pdf
```


## 빌드 및 실행

```bash
make
./threaded_proxy
```

## 의존성

- OpenSSL (`libssl-dev`)
- POSIX Threads (`-lpthread`)
- System V IPC
