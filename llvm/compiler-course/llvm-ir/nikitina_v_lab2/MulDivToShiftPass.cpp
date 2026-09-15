#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct MulDivToShiftPass : PassInfoMixin<MulDivToShiftPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (auto &BB : F) {
      for (Instruction &InstRef : make_early_inc_range(BB)) {
        Instruction *Inst = &InstRef;

        auto *BO = dyn_cast<BinaryOperator>(Inst);
        if (!BO)
          continue;

        Value *LHS = BO->getOperand(0);
        Value *RHS = BO->getOperand(1);
        IRBuilder<> Builder(BO);

        if (BO->getOpcode() == Instruction::Mul) {
          ConstantInt *CI = nullptr;
          Value *VariableOp = nullptr;

          if ((CI = dyn_cast<ConstantInt>(RHS))) {
            VariableOp = LHS;
          } else if ((CI = dyn_cast<ConstantInt>(LHS))) {
            VariableOp = RHS;
          }

          if (CI && CI->getValue().isStrictlyPositive() &&
              CI->getValue().isPowerOf2()) {
            uint64_t ShiftVal = CI->getValue().logBase2();
            Value *ShiftConst = ConstantInt::get(CI->getType(), ShiftVal);
            Value *NewInst =
                Builder.CreateShl(VariableOp, ShiftConst, "shl_opt");

            BO->replaceAllUsesWith(NewInst);
            BO->eraseFromParent();
            Changed = true;
          }
        } else if (BO->getOpcode() == Instruction::UDiv) {
          if (auto *CI = dyn_cast<ConstantInt>(RHS)) {
            if (CI->getValue().isStrictlyPositive() &&
                CI->getValue().isPowerOf2()) {
              uint64_t ShiftVal = CI->getValue().logBase2();
              Value *ShiftConst = ConstantInt::get(CI->getType(), ShiftVal);
              Value *NewInst = Builder.CreateLShr(LHS, ShiftConst, "lshr_opt");

              BO->replaceAllUsesWith(NewInst);
              BO->eraseFromParent();
              Changed = true;
            }
          }
        } else if (BO->getOpcode() == Instruction::SDiv) {
          if (auto *CI = dyn_cast<ConstantInt>(RHS)) {
            if (CI->getValue().isStrictlyPositive() &&
                CI->getValue().isPowerOf2()) {
              uint64_t ShiftAmount = CI->getValue().logBase2();
              Type *Ty = BO->getType();
              APInt OffsetMask =
                  APInt::getLowBitsSet(CI->getBitWidth(), ShiftAmount);
              Value *Zero = ConstantInt::get(Ty, 0);
              Value *OffsetConst = ConstantInt::get(Ty, OffsetMask);
              Value *ShiftConst = ConstantInt::get(Ty, ShiftAmount);
              Value *IsNeg = Builder.CreateICmpSLT(LHS, Zero, "is_neg");
              Value *Offset =
                  Builder.CreateSelect(IsNeg, OffsetConst, Zero, "sdiv_offset");
              Value *AdjustedLHS = Builder.CreateAdd(LHS, Offset, "adjusted_x");
              Value *NewInst =
                  Builder.CreateAShr(AdjustedLHS, ShiftConst, "ashr_opt");
              BO->replaceAllUsesWith(NewInst);
              BO->eraseFromParent();
              Changed = true;
            }
          }
        }
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulDivToShiftPass", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "mul-div-to-shift") {
                    FPM.addPass(MulDivToShiftPass());
                    return true;
                  }
                  return false;
                });
          }};
}