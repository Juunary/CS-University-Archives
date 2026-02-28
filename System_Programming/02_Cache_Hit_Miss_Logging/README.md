# Cache Hit/Miss Logging

## Overview

A project that adds Hit/Miss logging functionality to the proxy cache server. If the requested URL exists in the cache, it logs a Hit; otherwise, it logs a Miss.

## How it Works

1. URL Input → SHA1 Hash Generation
2. Check existence of `~/cache/[xx]/[hash]` path
   - **Hit**: Return file from cache + Log Hit
   - **Miss**: Save new file + Log Miss
3. Logs are recorded in a separate file with timestamps

## File Structure

```
02_Cache_Hit_Miss_Logging/
├── src/
│   ├── proxy_cache.c    — Proxy cache with Hit/Miss logging
│   ├── homedir.c
│   └── Makefile
└── Report_Hit_Miss_Log.pdf
```

## Build and Run

```bash
cd src
make
./proxy_cache
```
