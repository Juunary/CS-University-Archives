# 12-bit Majority Voter

## Overview

A combinational logic circuit that determines the majority by comparing the counts of 0s and 1s in a 12-bit input.

## How it Works

It counts 0s and 1s by iterating through each bit of the 12-bit input `A[11:0]` and returns the comparison result as a 3-bit output `Y`.

| Condition | Output Y |
|-----------|----------|
| Count of 0 > Count of 1 | `3'b100` |
| Count of 0 == Count of 1 | `3'b010` |
| Count of 0 < Count of 1 | `3'b001` |

## Module

```verilog
module majority_12bit(
    input  [11:0] A,
    output reg [2:0] Y
);
```

## Files

- `majority_voter.v` — Main Verilog Source
