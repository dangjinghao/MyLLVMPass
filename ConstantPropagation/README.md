# A Simple Constant Propagation Pass

## Supported Features

- [x] binary operation propagation with int/fp type constant operands
- [x] special operand propagation
- [x] constant variable propagation

## Required passes

- mem2reg

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone -Wno-tautological-compare ConstantPropagation/test.c -o build/ConstantPropagation/test.ll
```

## Test

```bash
opt -load-pass-plugin=./build/ConstantPropagation/ConstantPropagationPass.so -passes="mem2reg,ConstantPropagation" build/ConstantPropagation/test.ll | llvm-dis
```

In fact we can use lli to execute the optimized LLVM-IR and see the result:

```bash
opt -load-pass-plugin=./build/ConstantPropagation/ConstantPropagationPass.so -passes="mem2reg,ConstantPropagation" build/ConstantPropagation/test.ll | lli; echo $?
```
