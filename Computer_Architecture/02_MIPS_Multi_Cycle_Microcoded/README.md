# MIPS Multi-Cycle Microcoded CPU

## Overview

A project implementing MIPS Multi-Cycle CPU using ROM-based Microcode. Each instruction is decomposed into multiple micro-operations (μ-operations) and executed.

## Microcode Structure

- **MICRO ROM** (`ROM_MICRO.txt`): Definitions of control signals for each micro step
- **DISPATCH ROM** (`ROM_DISP.txt`): Dispatching from instruction opcode to micro routine start address

## Advantages over Single-Cycle

| Item | Single-Cycle | Multi-Cycle |
|------|--------------|-------------|
| Clock Period | Based on longest instruction | Shorter on average |
| Hardware | Dedicated path per instruction | Shared ALU/Memory |
| Control Complexity | PLA | ROM Microcode |

## Directory Structure

```
02_MIPS_Multi_Cycle_Microcoded/
├── design_logic/
│   ├── ROM_MICRO.txt        — Microcode ROM
│   └── ROM_DISP.txt         — Dispatch ROM
├── memory_init/
│   ├── M_TEXT_SEG.txt       — Instruction Memory
│   ├── M_DATA_SEG.txt       — Data Memory
│   └── run.bat              — Execution Script
├── verification_logs/
│   ├── reg_dump.txt
│   ├── mem_dump.txt
│   └── tb_cycle_limit.txt
├── tb_MC.o
├── tb_MC.vcd
├── Report_Multi_Cycle_Microarchitecture.pdf
├── Control Signals for MCPU.pdf
└── MIPS Inst Datasheet.pdf
```
