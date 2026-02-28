# Process Scheduling Simulation

## Overview

A collection of projects tracing process states using kernel modules and simulating CPU scheduling algorithms.

## Sub-projects

### 01_Process_State_Tracer
Process state information output via kernel module

- Accessing `task_struct` information of a specific PID through system call hooking
- Converting process state to string (Running / Wait / Zombie / Dead, etc.)
- Outputting parent/sibling process list

### 02_Multi_Thread_Fork
Parallel processing using fork and pthread

- Creating child processes using `fork()`
- Multi-threaded programming using POSIX threads (`pthread`)
- Producer-Consumer structure separating number generator and printer

### 03_CPU_Scheduler_Sim
CPU Scheduling Algorithm Simulator

Supported Algorithms:

| Algorithm | Description |
|-----------|-------------|
| FCFS | First Come First Served |
| SJF (Non-Preemptive) | Shortest Job First |
| SRTF (Preemptive) | Shortest Remaining Time First |
| RR | Round Robin (Configurable time quantum) |
| Priority (Non-Preemptive) | Priority Scheduling |
| Priority (Preemptive) | Preemptive Priority Scheduling |

- Gantt Chart output
- Calculation of Average Waiting Time, Turnaround Time, Response Time

## File Structure

```
03_Process_Scheduling_Simulation/
├── 01_Process_State_Tracer/
│   ├── process_tracer.c    — Main Kernel Module
│   ├── fork.c
│   ├── init_task.c
│   ├── sched.h
│   └── Makefile
├── 02_Multi_Thread_Fork/
│   ├── fork.c
│   ├── thread.c
│   ├── numgen.c
│   └── Makefile
├── 03_CPU_Scheduler_Sim/
│   ├── cpu_scheduler.c     — 스케줄러 시뮬레이터
│   └── Makefile
└── Report_CPU_Scheduler_Performance.pdf
```
