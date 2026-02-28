# Full-Featured Web Proxy — DNS & Traffic Forwarding

## Overview

A full-featured web proxy that includes DNS resolution to convert domain names to IPs and fully forwards traffic to destination servers.

## Added Features

- DNS resolution using `getaddrinfo()` / `gethostbyname()`
- Direct connection to destination server after Domain → IP conversion
- Bidirectional forwarding of HTTP/HTTPS traffic
- Logging of server uptime and connection counts

## Architecture

```
Client ──► Proxy ──DNS Lookup──► IP ──TCP──► Web Server
              │                                    │
              └──────────── Cache ◄────────────────┘
```

## File Structure

```
07_Full_Featured_Web_Proxy_DNS/
├── full_proxy_server.c  — Full-featured proxy including DNS
├── Makefile
└── Report_DNS_and_Traffic_Forwarding.pdf
```

## Build and Run

```bash
make
./full_proxy_server
```
