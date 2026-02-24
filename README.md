# 💻 Computer Engineering Portfolio

> **Archive of systems engineering projects focusing on OS, Architecture, and Algorithms.**
> *Key Stack: C/C++, Linux Kernel, Verilog, Python*

## 📂 Repository Structure

### [Operating System (Kernel & Simulation)](./Operating_System)
Deep dive into Linux Kernel internals and resource management.
- **Kernel Hooking**: Intercepted system calls using `ftrace` and implemented custom modules.
- **Scheduling**: Designed Priority & Round-Robin schedulers in C.
- **Virtual Memory**: Simulated LRU/FIFO page replacement policies.

### [Computer Architecture (CPU Design)](./Computer_Architecture)
RTL design and performance analysis of MIPS processors.
- **MIPS CPU**: Built Single-Cycle & Multi-Cycle CPUs from scratch using Verilog.
- **Optimization**: Analyzed **Pipeline Hazards** and implemented a **Forwarding Unit** to replace NOP stalls.
- **Cache Memory**: Benchmarked AMAT (Average Memory Access Time) using SimpleScalar.

### [Data Structures (DB & Engine)](./Data_Structures)
Implementation of high-performance data storage engines.
- **Employee DB**: Built an in-memory DB using **B+ Tree** indexing for rapid lookup.
- **Graph Engine**: Implemented Dijkstra, Kruskal, and Bellman-Ford algorithms for network analysis.

### [System Programming & Network](./System_Programming)
Low-level system interactions and protocol design.
- **Reliable Data Transfer**: Implemented Go-Back-N & Selective Repeat protocols (UDP).
- **Shell**: Developed a custom Linux shell handling pipes and redirection.

---
