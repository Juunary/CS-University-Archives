# 16-bit Parallel Adder

## Overview

A Verilog project implementing a 16-bit parallel adder by hierarchically composing Half Adders and Full Adders.

## Structure

```
half_adder
    └── full_adder (HA1 + HA2)
            └── adder_16bit (FA x16, generate block)
```

## Module Description

| Module | Input | Output | Description |
|--------|-------|--------|-------------|
| `half_adder` | a, b | sum, carry | Implemented with XOR/AND gates |
| `full_adder` | a, b, cin | sum, cout | Composed of two HAs |
| `adder_16bit` | A[15:0], B[15:0], Cin | Sum[15:0], Cout, Overflow | Connects 16 FAs using generate |

## Features

- `Overflow` output: Detecting overflow in signed arithmetic (`Cout XOR c[15]`)
- Implemented using Structural design method

## Files

- `adder_16bit.v` — Main Verilog Source
