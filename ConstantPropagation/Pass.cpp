#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {
class ThePass : public PassInfoMixin<ThePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    bool Changed = false;
    Module *M = F.getParent();
    const DataLayout &DL = M->getDataLayout();

    for (BasicBlock &BB : F) {
      for (auto It = BB.begin(); It != BB.end();) {
        Instruction &I = *It++;
        // Handle binary operations
        if (auto *BO = dyn_cast<BinaryOperator>(&I)) {
          Value *Op0 = BO->getOperand(0);
          Value *Op1 = BO->getOperand(1);
          if (isa<ConstantInt, ConstantFP>(Op0) &&
              isa<ConstantInt, ConstantFP>(Op1)) {
            Constant *C0 = cast<Constant>(Op0);
            Constant *C1 = cast<Constant>(Op1);
            if (Constant *Folded = ConstantExpr::get(BO->getOpcode(), C0, C1)) {
              BO->replaceAllUsesWith(Folded);
              BO->eraseFromParent();
              Changed = true;
            }
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isOne()) &&
                     BO->getOpcode() == Instruction::Mul) {
            // 1 * x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantFP>(Op0) &&
                      cast<ConstantFP>(Op0)->isExactlyValue(1.0)) &&
                     BO->getOpcode() == Instruction::FMul) {
            // 1.0 * x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isOne()) &&
                     BO->getOpcode() == Instruction::Mul) {
            // x * 1 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantFP>(Op1) &&
                      cast<ConstantFP>(Op1)->isExactlyValue(1.0)) &&
                     BO->getOpcode() == Instruction::FMul) {
            // x * 1.0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isOne()) &&
                     (BO->getOpcode() == Instruction::SDiv ||
                      BO->getOpcode() == Instruction::UDiv)) {
            // x / 1 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantFP>(Op1) &&
                      cast<ConstantFP>(Op1)->isExactlyValue(1.0)) &&
                     BO->getOpcode() == Instruction::FDiv) {
            // x / 1.0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt, ConstantFP>(Op0) &&
                      cast<Constant>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Add ||
                      BO->getOpcode() == Instruction::FAdd)) {
            // 0 + x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt, ConstantFP>(Op1) &&
                      cast<Constant>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Add ||
                      BO->getOpcode() == Instruction::FAdd)) {
            // x + 0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt, ConstantFP>(Op1) &&
                      cast<Constant>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Sub ||
                      BO->getOpcode() == Instruction::FSub)) {
            // x - 0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt, ConstantFP>(Op0) &&
                      cast<Constant>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Mul ||
                      BO->getOpcode() == Instruction::FMul)) {
            // 0 * x => 0
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt, ConstantFP>(Op1) &&
                      cast<Constant>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Mul ||
                      BO->getOpcode() == Instruction::FMul)) {
            // x * 0 => 0
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::And)) {
            // x & 0 => 0
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::And)) {
            // 0 & x => 0
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isAllOnesValue()) &&
                     (BO->getOpcode() == Instruction::And)) {
            // x & -1 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isAllOnesValue()) &&
                     (BO->getOpcode() == Instruction::And)) {
            // -1 & x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Or)) {
            // x | 0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Or)) {
            // 0 | x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isAllOnesValue()) &&
                     (BO->getOpcode() == Instruction::Or)) {
            // x | -1 => -1
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isAllOnesValue()) &&
                     (BO->getOpcode() == Instruction::Or)) {
            // -1 | x => -1
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Xor)) {
            // x ^ 0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Xor)) {
            // 0 ^ x => x
            BO->replaceAllUsesWith(Op1);
            BO->eraseFromParent();
            Changed = true;
          } else if (Op0 == Op1 && BO->getOpcode() == Instruction::Xor) {
            // x ^ x => 0
            Constant *Zero = ConstantInt::get(BO->getType(), 0);
            BO->replaceAllUsesWith(Zero);
            BO->eraseFromParent();
            Changed = true;
          } else if (Op0 == Op1 && (BO->getOpcode() == Instruction::Sub ||
                                    BO->getOpcode() == Instruction::FSub)) {
            // x - x => 0
            Constant *Zero;
            if (BO->getType()->isIntegerTy()) {
              Zero = ConstantInt::get(BO->getType(), 0);
            } else {
              Zero = ConstantFP::get(BO->getType(), 0.0);
            }
            BO->replaceAllUsesWith(Zero);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op1) &&
                      cast<ConstantInt>(Op1)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Shl ||
                      BO->getOpcode() == Instruction::LShr ||
                      BO->getOpcode() == Instruction::AShr)) {
            // x << 0 => x, x >> 0 => x
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if ((isa<ConstantInt>(Op0) &&
                      cast<ConstantInt>(Op0)->isZeroValue()) &&
                     (BO->getOpcode() == Instruction::Shl ||
                      BO->getOpcode() == Instruction::LShr ||
                      BO->getOpcode() == Instruction::AShr)) {
            // 0 << x => 0, 0 >> x => 0
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if (Op0 == Op1 && BO->getOpcode() == Instruction::And) {
            // x & x => x (idempotent)
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          } else if (Op0 == Op1 && BO->getOpcode() == Instruction::Or) {
            // x | x => x (idempotent)
            BO->replaceAllUsesWith(Op0);
            BO->eraseFromParent();
            Changed = true;
          }
        }

        if (auto *IC = dyn_cast<ICmpInst>(&I)) {
          Value *Op0 = IC->getOperand(0);
          Value *Op1 = IC->getOperand(1);

          if (Op0 == Op1) {
            // Compare with self
            bool Result;
            switch (IC->getPredicate()) {
            case ICmpInst::ICMP_EQ:  // x == x => true
            case ICmpInst::ICMP_ULE: // x <= x => true (unsigned)
            case ICmpInst::ICMP_SLE: // x <= x => true (signed)
            case ICmpInst::ICMP_UGE: // x >= x => true (unsigned)
            case ICmpInst::ICMP_SGE: // x >= x => true (signed)
              Result = true;
              break;
            default:
              Result = false;
            }
            Constant *ResultConst = ConstantInt::get(IC->getType(), Result);
            IC->replaceAllUsesWith(ResultConst);
            IC->eraseFromParent();
            Changed = true;
          }
        }

        // Handle floating-point comparison instructions
        if (auto *FC = dyn_cast<FCmpInst>(&I)) {
          Value *Op0 = FC->getOperand(0);
          Value *Op1 = FC->getOperand(1);

          if (Op0 == Op1) {
            // Compare with self (assuming no NaN)
            bool Result;
            switch (FC->getPredicate()) {
            case FCmpInst::FCMP_OEQ: // ordered equal: x == x => true
            case FCmpInst::FCMP_OLE: // ordered <=: x <= x => true
            case FCmpInst::FCMP_OGE: // ordered >=: x >= x => true
            case FCmpInst::FCMP_UEQ: // unordered equal: x == x => true
            case FCmpInst::FCMP_ULE: // unordered <=: x <= x => true
            case FCmpInst::FCMP_UGE: // unordered >=: x >= x => true
              Result = true;
              break;
            case FCmpInst::FCMP_ONE: // ordered !=: x != x => false
            case FCmpInst::FCMP_OLT: // ordered <: x < x => false
            case FCmpInst::FCMP_OGT: // ordered >: x > x => false
            case FCmpInst::FCMP_ULT: // unordered <: x < x => false
            case FCmpInst::FCMP_UGT: // unordered >: x > x => false
              Result = false;
              break;
            // skip NaN-related predicates
            case FCmpInst::FCMP_UNE: // unordered !=: could be true if NaN
            case FCmpInst::FCMP_ORD: // ordered (not NaN): may be false if NaN
            case FCmpInst::FCMP_UNO: // unordered (is NaN): may be true if NaN
            default:
              continue;
            }
            Constant *ResultConst = ConstantInt::get(FC->getType(), Result);
            FC->replaceAllUsesWith(ResultConst);
            FC->eraseFromParent();
            Changed = true;
          }
        }

        // Handle select instructions
        if (auto *SI = dyn_cast<SelectInst>(&I)) {
          Value *Cond = SI->getCondition();
          Value *TrueVal = SI->getTrueValue();
          Value *FalseVal = SI->getFalseValue();

          if (auto *ConstCond = dyn_cast<ConstantInt>(Cond)) {
            // select true, x, y => x
            // select false, x, y => y
            Value *Result = ConstCond->isOne() ? TrueVal : FalseVal;
            SI->replaceAllUsesWith(Result);
            SI->eraseFromParent();
            Changed = true;
          } else if (TrueVal == FalseVal) {
            // select cond, x, x => x
            SI->replaceAllUsesWith(TrueVal);
            SI->eraseFromParent();
            Changed = true;
          }
        }
      }
    }

    if (Changed) {
      errs() << "Running Constant Propagation on function: " << F.getName()
             << "\n";
      return PreservedAnalyses::none();
    }
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
