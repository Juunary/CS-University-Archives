# Pipeline CPU — Hazard Analysis

## Overview

A project that resolves and compares Data Hazards in a 5-stage MIPS Pipeline CPU using two methods.
Uses **Shell Sort** MIPS assembly as the test program.

## Comparison of Hazard Resolution Methods

| Item | 01_Software_Resolution_NOPs | 02_Hardware_Resolution_Fwd |
|------|-----------------------------|---------------------------|
| Method | Inserting NOP instructions | Forwarding Unit |
| Added Hardware | None | Forwarding + Hazard Detection |
| Performance | Cycle waste due to NOPs | Mostly resolved without NOPs |
| Code Size | Increased (NOPs added) | Original code maintained |

## Directory Structure

```
03_Pipeline_CPU_Hazard_Analysis/
├── 01_Software_Resolution_NOPs/
│   ├── shell_sort.asm       — Shell Sort assembly with NOPs inserted
│   ├── input/               — Instruction/Data memory initialization
│   └── output/              — Execution results (Register/Memory dumps, VCD)
├── 02_Hardware_Resolution_Fwd/
│   ├── shell_sort.asm       — Original Shell Sort assembly (NOPs removed)
│   ├── input/               — Instruction/Data memory initialization
│   └── output/              — Execution results
└── report.pdf
```

## 5-Stage Pipeline Structure

```
IF → ID → EX → MEM → WB
```

Data hazards are resolved by forwarding results generated in EX/MEM/WB stages to the ID/EX stage.
