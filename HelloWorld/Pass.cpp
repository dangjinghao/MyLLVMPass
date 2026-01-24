#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

#ifndef PASS_NAME_EXT
#define PASS_NAME_EXT "AnonPass"
#endif

#ifndef PASS_NAME
#define PASS_NAME "Anon"
#endif

using namespace llvm;

namespace {
struct ThePass : public PassInfoMixin<ThePass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << F.getName() << "\n";
    return PreservedAnalyses::all();
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
