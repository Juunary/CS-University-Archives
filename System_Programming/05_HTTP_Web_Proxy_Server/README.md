# HTTP Web Proxy Server

## Overview

A web proxy server that parses actual HTTP requests and forwards them to the destination server. Responses are stored in SHA1 hash-based cache and returned from cache upon re-request.

## How it Works

1. Receive HTTP GET request from client
2. Parse Host and URL from request headers
3. Check cache using SHA1 hash of URL
   - **Hit**: Return cache file
   - **Miss**: Connect to destination server → Receive response → Save to cache → Return to client
4. Independent handling per client using `fork()`

## HTTP Parsing

- Request Line: `GET http://host/path HTTP/1.0`
- Extract Host header
- Port number default: 80

## File Structure

```
05_HTTP_Web_Proxy_Server/
├── web_proxy.c          — HTTP proxy server implementation
├── Makefile
└── Report_HTTP_Proxy_Implementation.pdf
```

## Build and Run

```bash
make
./web_proxy
```

Browser Proxy Settings: `localhost:39999`
