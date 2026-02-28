# Sequence Detector FSM

## Overview

Implementation of a Finite State Machine (FSM) using Moore Machine to detect the `101101` bit pattern.

## State Diagram

```
S0 --1--> S1 --0--> S2 --1--> S3(Y=1) --1--> S4 --0--> S5 --1--> S6(Y=1,Y1=1)
                         |                                              |
                         +--(0)---> S0 <--(0)-------------------------+
```

## State Encoding

| State | Encoding | Meaning | Output |
|-------|----------|---------|--------|
| S0 | 000 | Initial State | Y=0, Y1=0 |
| S1 | 001 | '1' Detected | Y=0, Y1=0 |
| S2 | 010 | '10' Detected | Y=0, Y1=0 |
| S3 | 011 | '101' Detected | Y=1, Y1=0 |
| S4 | 100 | '1011' Detected | Y=0, Y1=0 |
| S5 | 101 | '10110' Detected | Y=0, Y1=0 |
| S6 | 110 | '101101' Detected | Y=1, Y1=1 |

## Module Interface

```verilog
module sequence_detector(
    input  X, CLK, RST,
    output reg Y,           // 1 when '101' or '101101' is detected
    output reg Y1,          // 1 when '101101' is detected
    output reg [2:0] current_state
);
```

## Files

- `sequence_detector.v` — Main Verilog Source
