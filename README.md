
# bob16++
this page is writen by ai because i do not know any markdown !!!

an better version of bob16 removed of all of the useless code and writen in a simple way instead of weird things this is purely writen and 
most instruction are tested for 1:1 binary compatibility

emulator runs arround 80 mips on a ryzen 5 2600x with -O0
and runs arround 130-200 mips with -O3

quick note
bin loading uses little endian

## base instructions

same instruction set as bob16 (NOP, ADD, AND, NOT, LD, LDI, LDR, ST, STI, STR, BR, JMP, JSR, LEA, RET, TRAP), fully binary compatible. see original bob16 docs for the base instruction semantics.

## extended instructions (trap 4)

extra instructions are accessed through `trap 4` (ARITHMETIC_EXPANSION), instead of adding new top level opcodes. this keeps bob16pp binary compatible with normal bob16 programs, since a `trap 4` in a program that doesn't use it just goes unused.

when a `trap 4` is hit, the emulator fetches the *next* word in memory as a second instruction word and decodes it separately:

```
 15            12 11    9 8      5 4      1 0
+----------------+-------+--------+--------+--+
|   sub-opcode   |  dst  |  src2  |  src1  |  |
+----------------+-------+--------+--------+--+
```

- bits 12-15: sub-opcode (which extended instruction)
- bits 9-11: dst register
- bits 4-6: src2 register
- bits 1-3: src1 register
- bit 0: unused

### sub-opcodes

| value | mnemonic | operation |
|-------|----------|-----------|
| 0 | SUB | `r[dst] = r[src2] - r[src1]` |
| 1 | MUL | `r[dst] = r[src2] * r[src1]` |
| 2 | IDIV | `r[dst] = r[src2] / r[src1]` (integer division) |
| 3 | MOD | `r[dst] = r[src2] % r[src1]` |
| 4 | INC | `r[dst] = ++r[dst]` |
| 5 | DEC | `r[dst] = --r[dst]` |
| 6 | MIN | `r[dst] = (r[src1] < r[src2]) ? r[src1] : r[src2]` |
| 7 | MAX | `r[dst] = (r[src1] > r[src2]) ? r[src1] : r[src2]` |
| 8 | CMP | condition codes set from `r[src1] - r[src2]`, no register written |

all extended instructions update the condition codes (n/z/p) same as base ALU ops, based on the resulting value in `r[dst]` (except CMP, which only sets cc from the subtraction result and doesn't touch a register).

### notes on extended instructions

- IDIV and MOD do not check for divide-by-zero — dividing by 0 is undefined behavior (crashes/UB same as C), so make sure `r[src1]` isn't 0 before using IDIV/MOD.
- INC/DEC only use `dst`, `src1`/`src2` fields are ignored (should be left as 0 by convention but currently unchecked).
- there is currently no assembler support for these — you'll need to hand-encode the trap 4 + sub-opcode word pair, or write your own assembler.

## on trap expansion

`trap 4` is used for arithmetic expansion (see above). more expansions will not use trap 4 instead they will use incremental trap 5 6 7 8 9 
