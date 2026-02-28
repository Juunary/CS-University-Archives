# Kernel Compilation Intro

## Overview

A practice project for building Linux kernel source and adding a custom system call to be called from user space.

## Practice Content

1. Download and configure Linux kernel source (`.config`)
2. Compile and install kernel using `make`
3. Register custom call number in the system call table
4. Implement custom system call (kernel space)
5. Verify call with user space test program

## File Structure

| File | Description |
|------|-------------|
| `custom_syscall_test.c` | Test program for calling custom system call |
| `Report_Linux_Kernel_Compilation.pdf` | Report on kernel compilation process and results |

## Execution Environment

- OS: Ubuntu Linux
- Kernel: Linux (Built from source)
