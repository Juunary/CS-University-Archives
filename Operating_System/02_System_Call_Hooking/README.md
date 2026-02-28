# System Call Hooking

## Overview

A project to hook the system call table using Linux kernel modules and intercept I/O-related system calls for tracing.

## Sub-projects

### 01_Ftrace_Syscall_Add
Adding a custom system call using Ftrace

- Registering a function to a new system call number (336) using kernel Ftrace mechanism

### 02_Ftrace_Hooking_Module
Direct system call table hooking module

- Retrieving `sys_call_table` address using `kallsyms_lookup_name`
- Modifying page protection bits (RW conversion) to replace function pointers

### 03_IO_Trace_Hooking
I/O system call tracing module

- Counting calls and bytes for `open`, `close`, `read`, `write`, `lseek`
- Outputting I/O activity of a specific PID to kernel log (`dmesg`) when registered
- Restoring original system calls upon module unload

## Core Technologies

- `sys_call_table` hooking (Bypassing Read-Only page protection)
- Linux Kernel Module (`module_init` / `module_exit`)
- Global variable sharing between modules via `EXPORT_SYMBOL`

## File Structure

```
02_System_Call_Hooking/
├── 01_Ftrace_Syscall_Add/
│   └── os_ftrace.c
├── 02_Ftrace_Hooking_Module/
│   └── os_ftracehooking.c
├── 03_IO_Trace_Hooking/
│   ├── ftracehooking.c      — sys_call_table hooking module
│   ├── ftracehooking.h
│   ├── iotracehooking.c     — I/O system call tracing module
│   └── Makefile
└── Report_System_Call_Hooking_Analysis.pdf
```
