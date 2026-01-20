# A LLVM Out-of-tree Hello World Pass

## Build

```bash
cmake -S . -B build
make -C build -j
```

## LLVM-IR Generation

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone test.c -o build/test.ll
```

`-O0 -Xclang -disable-O0-optnone` means generating unoptimized IR without the `optnone` attribute, so custom passes can run on all functions. If you omit `-Xclang -disable-O0-optnone`, Clang will add the `optnone` attribute to each function at `-O0`, and most LLVM passes (including your own) will be skipped for those functions.

## test

```bash
opt -load-pass-plugin=./build/HelloWorld.so -passes="hello-world" -disable-output build/test.ll
```
