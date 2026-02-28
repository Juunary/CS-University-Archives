# MIPS Single-Cycle CPU

## Overview

A project implementing MIPS Single-Cycle CPU and simulating it with Verilog. Every instruction completes in 1 clock cycle.

## Implementation Details

- Control signal design based on PLA (Programmable Logic Array)
  - `PLA_AND.txt`: AND array (Input condition)
  - `PLA_OR.txt`: OR array (Output signal)
- MIPS-I Instruction support: R-type, I-type (LUI, ORI, SH, LW, etc.)

## Directory Structure

```
01_MIPS_Single_Cycle_CPU/
├── control_logic/
│   ├── PLA_AND.txt         — PLA AND array definition
│   └── PLA_OR.txt          — PLA OR array definition
├── simulation/
│   ├── M_TEXT_SEG.txt      — Instruction memory initialization (binary)
│   └── run.bat             — Simulation execution script
├── simulation_logs/
│   ├── reg_dump.txt        — Register state after execution
│   ├── mem_dump.txt        — Memory state after execution
│   └── tb_cycle_limit.txt  — Cycle limit log
├── tb_SC.o                 — Compiled testbench
├── tb_SC.vcd               — Waveform dump (View with GTKWave etc.)
└── Report_MIPS_Single_Cycle.pdf
```

## How to Run

```bash
cd simulation
run.bat
```
