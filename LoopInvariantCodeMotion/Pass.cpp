#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class LoopInvariantCodeMotion : public PassInfoMixin<LoopInvariantCodeMotion> {
private:
  bool isLoopInvariant(Instruction *I, Loop *L, DominatorTree &DT) {
    // skip phi nodes
    if (isa<PHINode>(I))
      return false;

    // skip instructions with side effects
    if (I->mayHaveSideEffects())
      return false;

    // check whether all operands are loop invariant(out of loop or constants)
    for (Value *Op : I->operands()) {
      // if the operand is an instruction
      if (auto *OpI = dyn_cast<Instruction>(Op)) {
        // if the operand is inside the loop, check if it's loop invariant
        if (L->contains(OpI)) {
          if (!isLoopInvariant(OpI, L, DT))
            return false;
        }
      }
      // Constants or values defined outside the loop are acceptable
    }

    return true;
  }

  // Check whether the instruction can be safely hoisted
  bool isSafeToHoist(Instruction *I, Loop *L) {
    // Cannot hoist instructions with side effects
    if (I->mayHaveSideEffects())
      return false;

    // Cannot hoist memory access instructions (load/store)
    else if (isa<LoadInst>(I) || isa<StoreInst>(I))
      return false;

    // Cannot hoist call instructions
    else if (isa<CallInst>(I))
      return false;
    // Cannot hoist terminator instructions
    else if (I->isTerminator())
      return false;
    return true;
  }

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

    bool Changed = false;

    // Traverse all loops (process nested loops from inner to outer)
    std::vector<Loop *> Loops;
    for (auto *L : LI) {
      for (auto *SubL : L->getSubLoops()) {
        Loops.push_back(SubL);
      }
      Loops.push_back(L);
    }

    for (Loop *L : Loops) {
      BasicBlock *Preheader = L->getLoopPreheader();

      // Skip if there is no preheader
      if (!Preheader)
        continue;

      Instruction *InsertPoint = Preheader->getTerminator();

      // Traverse all basic blocks in the loop
      std::vector<BasicBlock *> BlocksToProcess(L->blocks().begin(),
                                                L->blocks().end());

      for (BasicBlock *BB : BlocksToProcess) {
        // Use iterator to traverse instructions, as the instruction sequence
        // may be modified
        for (auto It = BB->begin(); It != BB->end();) {
          Instruction *I = &*It++;

          // Check if the instruction is loop-invariant and safe to hoist
          if (isLoopInvariant(I, L, DT) && isSafeToHoist(I, L)) {
            // Check if all uses are within the loop
            // maybe that is used for the PHI [I, loop] node outside the loop?

            bool AllUsesInLoop = true;
            for (User *U : I->users()) {
              if (auto *UI = dyn_cast<Instruction>(U)) {
                if (!L->contains(UI)) {
                  AllUsesInLoop = false;
                  break;
                }
              }
            }

            // Only hoist if all uses are within the loop
            if (AllUsesInLoop) {
              I->moveBefore(InsertPoint->getIterator());
              Changed = true;
              errs() << "LICM: Hoisting instruction: " << *I << "\n";
            }
          }
        }
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME_EXT, LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == PASS_NAME) {
                    FPM.addPass(LoopInvariantCodeMotion());
                    return true;
                  }
                  return false;
                });
          }};
}
