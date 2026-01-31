# Common subexpression elimination pass

## Required passes

- mem2reg

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone CommonSubexpressionElimination/test.c -o build/CommonSubexpressionElimination/test.ll
```

## Test

```bash
opt -load-pass-plugin=./build/CommonSubexpressionElimination/CommonSubexpressionEliminationPass.so -passes="mem2reg,CommonSubexpressionElimination" build/CommonSubexpressionElimination/test.ll | llvm-dis
```
