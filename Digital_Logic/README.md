# Digital Logic Design

A collection of digital logic circuit projects implemented in Verilog HDL.

## Project List

### [01_16bit_Prefix_Adder](./01_16bit_Prefix_Adder/)
16-bit Parallel Adder Implementation

- Hierarchical design: Half Adder → Full Adder → 16-bit Adder
- Structural design using `generate` construct
- Carry propagation and signed overflow detection

**Key File:** `adder_16bit.v`

---

### [02_12bit_Majority_Voter](./02_12bit_Majority_Voter/)
12-bit Majority Voter Implementation

- Compares counts of 0s and 1s in 12-bit input to determine majority
- Output: `3'b100` (Majority 0) / `3'b010` (Tie) / `3'b001` (Majority 1)

**Key File:** `majority_voter.v`

---

### [03_Sequence_Detector_FSM](./03_Sequence_Detector_FSM/)
Sequence Detector using Finite State Machine (FSM)

- Detecting `101101` bit pattern using Moore Machine
- Defining 7 states (S0~S6) and implementing state transition logic
- Output Y=1 on `101` detection, Y=1, Y1=1 on `101101` detection

**Key File:** `sequence_detector.v`

---

## Languages and Environment

- Language: Verilog HDL
- Tool: Logisim / ModelSim / Icarus Verilog
