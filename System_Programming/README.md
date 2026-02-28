# System Programming

A collection of system programming projects implemented in C.
The structure evolves step-by-step from a filesystem-based local proxy cache to network sockets, HTTP proxy, and concurrency control.

## Project Evolution Flow

```
Local Cache → Cache Logging → Multi-Process → Socket Communication → HTTP Proxy
         → Timeout → DNS Forwarding → Semaphore Synchronization → Threaded Logging
```

## Project List

| # | Project | Key Technologies |
|---|----------|-----------|
| 01 | [Proxy Cache Server](./01_Proxy_Cache_Server/) | SHA1 Hashing, Filesystem Cache |
| 02 | [Cache Hit/Miss Logging](./02_Cache_Hit_Miss_Logging/) | Cache Hit/Miss Log Recording |
| 03 | [Multi-Process Server](./03_Multi_Process_Server_Arch/) | fork(), Multi-Process Architecture |
| 04 | [Networked Proxy (Socket)](./04_Networked_Proxy_Server_Socket/) | TCP Sockets, Client-Server |
| 05 | [HTTP Web Proxy](./05_HTTP_Web_Proxy_Server/) | HTTP Parsing, Proxy Forwarding |
| 06 | [Reliable Proxy (Timeout)](./06_Reliable_Web_Proxy_Timeout/) | Socket Timeout Handling |
| 07 | [Full-Featured Proxy (DNS)](./07_Full_Featured_Web_Proxy_DNS/) | DNS Lookup, Traffic Forwarding |
| 08 | [Concurrent Proxy (Semaphore)](./08_Concurrent_Proxy_Semaphore_Sync/) | System V Semaphore Synchronization |
| 09 | [Hybrid Proxy (Threaded Logging)](./09_Hybrid_Arch_Threaded_Logging/) | pthread, Thread-based Logging |

## Common Tech Stack

- Language: C (Linux/POSIX)
- Libraries: OpenSSL (SHA1), POSIX sockets, pthreads, System V IPC
- Build: Makefile
- OS: Ubuntu Linux
