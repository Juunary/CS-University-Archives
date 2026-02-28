# Computer Architecture

A collection of CPU design and performance analysis projects using Logisim / Verilog HDL.
Starting from MU0 processor design, it covers MIPS single/multi-cycle, pipeline, and cache memory analysis step by step.

## Project List

### [00_MU0_Processor_Design](./00_MU0_Processor_Design/)
Educational MU0 Processor Design

- Implementation of MU0 architecture with Logisim
- Includes RAM initialization code and architecture analysis report

---

### [01_MIPS_Single_Cycle_CPU](./01_MIPS_Single_Cycle_CPU/)
MIPS Single-Cycle CPU Implementation

- Control signal design using PLA (AND/OR array)
- MIPS instruction simulation (LUI, ORI, SH, etc.)
- Includes register/memory dumps and VCD waveform logs

---

### [02_MIPS_Multi_Cycle_Microcoded](./02_MIPS_Multi_Cycle_Microcoded/)
MIPS Multi-Cycle CPU Implementation based on Microcode

- ROM-based microcode design (MICRO ROM, DISPATCH ROM)
- Execution by breaking down each instruction into multiple cycles
- Includes verification logs (register/memory dumps)

---

### [03_Pipeline_CPU_Hazard_Analysis](./03_Pipeline_CPU_Hazard_Analysis/)
Pipeline CPU Hazard Analysis and Resolution

- **01_Software_Resolution_NOPs**: Solving data hazards in software by inserting NOPs
- **02_Hardware_Resolution_Fwd**: Solving hazards with Forwarding hardware
- Comparing both methods using Shell Sort MIPS assembly code

---

### [04_Cache_Memory_Performance_Analysis](./04_Cache_Memory_Performance_Analysis/)
Cache Memory Performance Analysis

- Connecting cache to MIPS CPU simulator to measure Hit/Miss
- Running SPEC CPU95 benchmark with SimpleScalar (SS) simulator
- Comparing Sequential (Sort) and Random access patterns

---

## 언어 및 도구

- Tool: Logisim (schematic), Verilog HDL (RTL), SimpleScalar
- MIPS 어셈블리 (MIPS-I ISA)
