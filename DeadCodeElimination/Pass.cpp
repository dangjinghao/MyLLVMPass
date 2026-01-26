#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {
class ThePass : public PassInfoMixin<ThePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << "Running DCE on function: " << F.getName() << "\n";

    bool Changed = false;
    SmallVector<Instruction *, 16> ToErase;

    for (auto &BB : F) {
      for (auto &I : BB) {
        // terminator and side-effect instructions are not dead code
        if (I.mayHaveSideEffects() || I.isTerminator())
          continue;
        // there are no uses of this instruction, mark it for deletion
        if (I.use_empty()) {
          ToErase.push_back(&I);
        }
      }
    }

    // Iteratively delete dead instructions, because there may be new dead code
    // after deletion
    while (!ToErase.empty()) {
      Instruction *I = ToErase.pop_back_val();
      if (!I->use_empty() || I->mayHaveSideEffects() || I->isTerminator())
        continue;

      // save the operands before deletion to check for new dead code later
      SmallVector<Instruction *, 4> Operands;
      for (auto &Op : I->operands()) {
        if (auto *OpInst = dyn_cast<Instruction>(Op)) {
          Operands.push_back(OpInst);
        }
      }

      I->eraseFromParent();
      Changed = true;

      // check operands for new dead code
      for (auto *OpInst : Operands) {
        if (OpInst->use_empty() && !OpInst->mayHaveSideEffects() &&
            !OpInst->isTerminator()) {
          ToErase.push_back(OpInst);
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
                    FPM.addPass(ThePass());
                    return true;
                  }
                  return false;
                });
          }};
}
