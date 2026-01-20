#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {
static size_t f_number = 0;
struct HelloWorldPass : public PassInfoMixin<HelloWorldPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << F.getName() << "\n";
    return PreservedAnalyses::all();
  }
};
} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HelloWorldPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hello-world") {
                    FPM.addPass(HelloWorldPass());
                    return true;
                  }
                  return false;
                });
          }};
}
