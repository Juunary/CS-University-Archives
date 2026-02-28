# Operating System

A collection of Operating System practical projects focused on Linux kernel programming.
It covers everything step-by-step from kernel building to system call hooking, process scheduling, memory management, and file system implementation.

## Project List

### [01_Kernel_Compilation_Intro](./01_Kernel_Compilation_Intro/)
Linux kernel compilation and adding custom system calls

- Building locally from Linux source and booting
- Implementing custom system calls and calling them from user space

---

### [02_System_Call_Hooking](./02_System_Call_Hooking/)
Kernel module implementation via Ftrace and system call table hooking

- **01_Ftrace_Syscall_Add**: Adding system calls based on Ftrace
- **02_Ftrace_Hooking_Module**: Module for direct hooking of the system call table
- **03_IO_Trace_Hooking**: Tracking I/O related system calls (open/close/read/write/lseek) and gathering statistics

---

### [03_Process_Scheduling_Simulation](./03_Process_Scheduling_Simulation/)
Tracing process states and simulating CPU schedulers

- **01_Process_State_Tracer**: Kernel module to output process states (Running/Wait/Zombie, etc.) and sibling processes
- **02_Multi_Thread_Fork**: Parallel processing using fork/thread
- **03_CPU_Scheduler_Sim**: Simulation of CPU scheduling algorithms such as FCFS, SJF, RR (Gantt chart output)

---

### [04_Memory_Management](./04_Memory_Management/)
Querying Virtual Memory Areas (VMA) and simulating page replacement algorithms

- **01_VMA_Info_Inspector**: Kernel module that exposes process VMA information via the `/proc` filesystem
- **02_Page_Replacement_Sim**: Comparison of page replacement algorithms: Optimal, FIFO, LRU, LFU, MFU

---

### [05_File_System_Implementation](./05_File_System_Implementation/)
Implementation of proc filesystem and FAT filesystem

- **01_Proc_FS_Process_Info**: Exposing process information to the `/proc` virtual filesystem
- **02_FAT_File_System**: Filesystem implementing the FAT structure (directory + FAT table + data blocks) in memory (create/write/read/delete/list, auto-persist)

---

## Languages and Environment

- Language: C (Linux Kernel Module)
- OS: Ubuntu Linux (Kernel Module Build Environment)
- Build: Makefile (Kernel Module)
