#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct StrengthReductionPass : PassInfoMixin<StrengthReductionPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (auto &BasicBlock : F) {
      for (Instruction &Inst : make_early_inc_range(BasicBlock)) {
        auto *BinOp = dyn_cast<BinaryOperator>(&Inst);
        if (!BinOp)
          continue;

        unsigned OpCode = BinOp->getOpcode();

        if (OpCode != Instruction::Mul && OpCode != Instruction::SDiv &&
            OpCode != Instruction::UDiv)
          continue;

        auto *CI = dyn_cast<ConstantInt>(BinOp->getOperand(1));
        if (!CI)
          continue;

        const APInt &Val = CI->getValue();

        if (Val.isPowerOf2() && Val.getZExtValue() > 1) {
          uint64_t ShiftAmount = Val.logBase2();

          IRBuilder<> Builder(BinOp);
          Value *NewInst = nullptr;
          Type *Ty = BinOp->getType();
          Value *ShiftConst = ConstantInt::get(Ty, ShiftAmount);

          if (OpCode == Instruction::Mul) {
            NewInst =
                Builder.CreateShl(BinOp->getOperand(0), ShiftConst, "shl_opt");
          } else if (OpCode == Instruction::UDiv) {
            NewInst = Builder.CreateLShr(BinOp->getOperand(0), ShiftConst,
                                         "lshr_opt");
          } else if (OpCode == Instruction::SDiv) {
            uint64_t BiasVal = (1ULL << ShiftAmount) - 1;
            Value *X = BinOp->getOperand(0);
            Value *IsNeg =
                Builder.CreateICmpSLT(X, ConstantInt::get(Ty, 0), "is_neg");
            Value *Bias =
                Builder.CreateSelect(IsNeg, ConstantInt::get(Ty, BiasVal),
                                     ConstantInt::get(Ty, 0), "bias");
            Value *AddBias = Builder.CreateAdd(X, Bias, "add_bias");
            NewInst = Builder.CreateAShr(AddBias, ShiftConst, "ashr_opt");
          }

          if (NewInst) {
            BinOp->replaceAllUsesWith(NewInst);
            BinOp->eraseFromParent();
            Changed = true;
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
  return {LLVM_PLUGIN_API_VERSION, "StrengthReductionPlugin", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "strength-reduction") {
                    FPM.addPass(StrengthReductionPass{});
                    return true;
                  }
                  return true;
                });
          }};
}
