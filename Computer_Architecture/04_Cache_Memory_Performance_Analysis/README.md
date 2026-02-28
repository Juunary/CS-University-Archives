# Cache Memory Performance Analysis

## Overview

A project measuring and analyzing cache memory performance in two simulator environments.

## Simulator Configuration

### prj4_CACHE (MIPS CPU based Cache Simulation)

Measuring performance by connecting cache to a custom-implemented MIPS CPU simulator.

- **Sort Workload** (`sort_text.txt` / `sort_data.dat`): Sequential access pattern
- **Random Access Workload** (`random_access.txt` / `random_access.dat`): Random access pattern
- Result: Hit/Miss count, Hit Rate

```
prj4_CACHE_-_2025/
├── M_TEXT_SEG.txt       — Instruction Memory
├── M_DATA_SEG.txt       — Data Memory
├── sort_text.txt / sort_data.dat
├── random_access.txt / random_access.dat
├── run_SRT.batx         — Sort Execution Script
├── run_RND.batx         — Random Execution Script
└── mem_dump.txt / reg_dump.txt
```

### prj4_SS_CACHE (SimpleScalar based SPEC CPU95 Benchmark)

Analyzing cache performance by running actual SPEC CPU95 programs using SimpleScalar simulator.

- Benchmarks: `go`, `m88ksim`, `swim`
- Cache Config: `cache.cfg`
- Analysis based on memory access traces

```
prj4_SS_CACHE_-_2025/
├── bin/                 — SPEC CPU95 Binaries
├── inputs/              — Benchmark Input Files
├── traces/              — Memory Access Traces
├── results/             — Simulation Results
├── cache.cfg            — Cache Config File
└── sim_spec95.txt       — Execution Command Script
```

## Analysis Points

- Direct Mapped vs Set Associative vs Fully Associative
- Write-Through vs Write-Back
- Comparison of spatial/temporal locality by workload

**리포트:** `report.pdf`
