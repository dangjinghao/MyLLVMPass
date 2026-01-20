# A LLVM Out-of-tree Hello World Pass

## Build

```bash
cmake -S . -B build
make -C build -j
```

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone HelloWorld/test.c -o build/HelloWorld/test.ll
```

`-O0 -Xclang -disable-O0-optnone` means generating unoptimized IR without the `optnone` attribute, so custom passes can run on all functions. If you omit `-Xclang -disable-O0-optnone`, Clang will add the `optnone` attribute to each function at `-O0`, and most LLVM passes (including your own) will be skipped for those functions.

## test

```bash
opt -load-pass-plugin=./build/HelloWorld/HelloWorldPass.so -passes="hello-world" -disable-output build/HelloWorld/test.ll
```

## Reference

- <https://llvm.org/docs/WritingAnLLVMNewPMPass.html>
- <https://llvm.org/docs/CMake.html>
