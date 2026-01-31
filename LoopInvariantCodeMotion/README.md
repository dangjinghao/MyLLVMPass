# Loop invariant code motion

This is a simple LLVM pass that performs loop invariant code motion (LICM) optimization.

This optimization is very conservative and only hoists instructions that are declared, only used in loop and guaranteed to be safe to move out of loops.

## Required passes

- mem2reg

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone LoopInvariantCodeMotion/test.c -o build/LoopInvariantCodeMotion/test.ll
```

## Test

```bash
opt -load-pass-plugin=./build/LoopInvariantCodeMotion/LoopInvariantCodeMotionPass.so -passes="mem2reg,LoopInvariantCodeMotion" build/LoopInvariantCodeMotion/test.ll | llvm-dis
```
