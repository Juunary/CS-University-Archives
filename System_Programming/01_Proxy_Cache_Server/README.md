# Proxy Cache Server

## Overview

A local proxy cache server that takes a URL as input, converts it to a SHA1 hash, and stores it hierarchically under `~/cache/` in the home directory.

## How it Works

1. User enters URL
2. SHA1 hashing of URL → Generates 40-character hexadecimal string
3. Uses first 2 characters as subdirectory name, saving to `~/cache/[xx]/[hash]` path
4. Exits when `bye` is entered

## Cache Structure

```
~/cache/
├── ab/
│   └── abcdef1234...  (SHA1 hash file)
├── 3f/
│   └── 3f9821...
└── ...
```

## File Structure

```
01_Proxy_Cache_Server/
├── src/
│   ├── proxy_cache.c    — Main proxy cache implementation
│   ├── homedir.c        — Home directory utility
│   └── Makefile
├── practice_warmup/
│   └── mkdir_test.c     — Directory creation practice
└── Report_Proxy_Cache.pdf
```

## Build and Run

```bash
cd src
make
./proxy_cache
```

## Dependencies

- OpenSSL (`libssl-dev`): SHA1 hashing
