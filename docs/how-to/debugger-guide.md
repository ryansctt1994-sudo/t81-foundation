# HanoiVM Debugger Guide

The `t81 debug` command provides a built-in interactive debugger for HanoiVM.

## Usage

```bash
t81 debug my_program.t81
# or
t81 debug my_program.tisc
```

## Commands

- `s`: **Step** one instruction.
- `c`: **Continue** execution until the next breakpoint or program halt.
- `b <pc>`: Set a **breakpoint** at the specified program counter (PC).
- `r`: Print current **registers** (R0-R8, PC, SP).
- `k`: Print the **stack** (top 10 elements).
- `m <addr>`: Print **memory** at the specified address.
- `h`: Show **help**.
- `q`: **Quit** the debugger.

## Example Session

```text
dbg> b 10
Breakpoint set at PC=10
dbg> c
Breakpoint hit at PC=10
dbg> r
Registers:
  R0: 81
  ...
  PC: 10
  SP: 5
dbg> s
[  11]
dbg> q
```
