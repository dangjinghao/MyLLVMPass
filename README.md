# MyLLVMPass

This repository contains some basic LLVM pass implementations for learning and experimentation with **AI assistance**:

- [Hello World Pass](HelloWorld/README.md)
- [Loop Invariant Code Motion Pass](LoopInvariantCodeMotion/README.md)
- [Constant Propagation Pass](ConstantPropagation/README.md)
- [Dead Code Elimination Pass](DeadCodeElimination/README.md)
- [Common Subexpression Elimination Pass](CommonSubexpressionElimination/README.md)

## Build

```bash
cmake -S . -B build
make -C build -j # or ninja, or something
```

## Reference

- <https://llvm.org/docs/WritingAnLLVMNewPMPass.html>
- <https://llvm.org/docs/CMake.html>
