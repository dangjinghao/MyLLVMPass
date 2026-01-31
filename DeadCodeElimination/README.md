# Dead code elimination motion pass

## Required passes

- mem2reg

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone DeadCodeElimination/test.c -o build/DeadCodeElimination/test.ll
```

## Test

```bash
opt -load-pass-plugin=./build/DeadCodeElimination/DeadCodeEliminationPass.so -passes="mem2reg,DeadCodeElimination" build/DeadCodeElimination/test.ll | llvm-dis
```
