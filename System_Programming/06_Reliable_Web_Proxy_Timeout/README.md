# Reliable Web Proxy — Timeout Handling

## Overview

A project that improves reliability by adding socket timeout handling to the HTTP web proxy. If the destination server does not respond, it disconnects after a certain time and returns an error.

## Added Features

- Setting receive/send timeouts using `SO_RCVTIMEO` / `SO_SNDTIMEO` socket options
- Returning appropriate error responses to the client upon timeout
- Preventing broken pipe using `SIGPIPE` signal handling

## File Structure

```
06_Reliable_Web_Proxy_Timeout/
├── reliable_web_proxy.c — Proxy server with timeout added
├── Makefile
└── Report_Proxy_Timeout_Handling.pdf
```

## Build and Run

```bash
make
./reliable_web_proxy
```
