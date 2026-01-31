#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

#include <unordered_map>

using namespace llvm;

namespace {
class ThePass : public PassInfoMixin<ThePass> {
private:
  // We need to create a map to track seen instructions.
  struct InstKey {
    unsigned Opcode;
    Type *Ty;
    SmallVector<Value *, 4> Ops;
  };

  struct InstKeyHash {
    size_t operator()(InstKey const &K) const noexcept {
      // combine opcode, type, and operand pointers into a hash
      size_t h = (size_t)K.Opcode;
      h = llvm::hash_combine(h, (uintptr_t)K.Ty);
      for (Value *V : K.Ops)
        h = llvm::hash_combine(h, (uintptr_t)V);
      return h;
    }
  };

  struct InstKeyEq {
    bool operator()(InstKey const &A, InstKey const &B) const noexcept {
      if (A.Opcode != B.Opcode)
        return false;
      if (A.Ty != B.Ty)
        return false;
      if (A.Ops.size() != B.Ops.size())
        return false;
      // LLVM IR is SSA format, so we can use pointer equality for operands.
      for (unsigned i = 0, e = A.Ops.size(); i != e; ++i)
        if (A.Ops[i] != B.Ops[i])
          return false;
      return true;
    }
  };

  // Build a key for an instruction. For commutative instructions we
  // canonicalize operand order by pointer value to catch operand-swapped
  // duplicates.
  InstKey makeKey(Instruction *I) {
    InstKey K;
    K.Opcode = I->getOpcode();
    K.Ty = I->getType();
    for (Use &U : I->operands())
      K.Ops.push_back(U.get());

    // canonicalize operand order
    if (I->isCommutative() && K.Ops.size() == 2) {
      if ((uintptr_t)K.Ops[0] > (uintptr_t)K.Ops[1])
        std::swap(K.Ops[0], K.Ops[1]);
    }
    return K;
  }

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    bool Changed = false;

    // Iterate over blocks and perform a simple local CSE: within a basic block,
    // if an instruction computes the same opcode and operands as a prior
    // instruction (and is safe to replace), replace uses and erase the dup.
    for (BasicBlock &BB : F) {
      std::unordered_map<InstKey, Instruction *, InstKeyHash, InstKeyEq> Seen;
      SmallVector<Instruction *, 16> ToErase;

      for (Instruction &I : BB) {
        // Skip PHI nodes and terminators
        if (isa<PHINode>(&I) || I.isTerminator())
          continue;

        // Skip instructions with side effects or memory operations
        if (I.mayHaveSideEffects() || I.mayReadOrWriteMemory())
          continue;

        // Build key
        InstKey K = makeKey(&I);

        auto It = Seen.find(K);
        if (It != Seen.end()) {
          Instruction *Prev = It->second;
          // Replace uses of I with Prev and mark I for deletion
          I.replaceAllUsesWith(Prev);
          ToErase.push_back(&I);
          Changed = true;
          errs() << "CSE: replaced " << I << " with " << *Prev << "\n";
        } else {
          Seen.emplace(std::move(K), &I);
        }
      }

      // Erase dead instructions
      for (Instruction *D : ToErase) {
        if (D->use_empty())
          D->eraseFromParent();
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
