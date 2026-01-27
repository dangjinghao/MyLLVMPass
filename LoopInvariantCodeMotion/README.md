# Loop invariant code motion

This is a simple LLVM pass that performs loop invariant code motion (LICM) optimization.

This optimization is very conservative and only hoists instructions that are declared, only used in loop and guaranteed to be safe to move out of loops.
