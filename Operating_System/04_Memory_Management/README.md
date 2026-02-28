# Memory Management

## Overview

A collection of projects exploring the Linux kernel's virtual memory management structure and simulating page replacement algorithms.

## Sub-projects

### 01_VMA_Info_Inspector
Kernel module for inspecting Virtual Memory Area (VMA) information

- Traversing `vm_area_struct` list from `task_struct`'s `mm_struct`
- Outputting start/end addresses, access permissions (read/write/execute), and mapped file information for each VMA
- Verifying results via `/proc` interface or `dmesg`

### 02_Page_Replacement_Sim
Page Replacement Algorithm Simulator

Supported Algorithms:

| Algorithm | Description |
|-----------|-------------|
| Optimal | Optimal replacement based on future references |
| FIFO | Replace the oldest page |
| LRU | Replace the Least Recently Used page |
| LFU | Replace the Least Frequently Used page |
| MFU | Replace the Most Frequently Used page |

- Comparing page fault counts given a page reference string and number of frames

## File Structure

```
04_Memory_Management/
├── 01_VMA_Info_Inspector/
│   ├── file_varea.c        — VMA inspection kernel module
│   └── Makefile
├── 02_Page_Replacement_Sim/
│   ├── page_replacement_simulator.c
│   └── Makefile
└── Report_Page_Replacement_Simulation.pdf
```
