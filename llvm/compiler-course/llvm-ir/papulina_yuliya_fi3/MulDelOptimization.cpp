#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct MulDelOptimization : llvm::PassInfoMixin<MulDelOptimization> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool isChanged = false;
    for (llvm::BasicBlock &bb : func) {
      for (llvm::Instruction &instr : llvm::make_early_inc_range(bb)) {
        if (llvm::BinaryOperator *binOp =
                llvm::dyn_cast<llvm::BinaryOperator>(&instr)) {
          if (binOp->getOpcode() != llvm::Instruction::Mul &&
              binOp->getOpcode() != llvm::Instruction::UDiv &&
              binOp->getOpcode() != llvm::Instruction::SDiv)
            continue;
          llvm::Value *rhs = binOp->getOperand(1);

          auto *C = llvm::dyn_cast<llvm::ConstantInt>(rhs);
          if (!C)
            continue;
          if (!C->getValue().isPowerOf2() ||
              !C->getValue().isStrictlyPositive())
            continue;

          llvm::Value *lhs = binOp->getOperand(0);
          llvm::Value *shiftOp = nullptr;
          llvm::IRBuilder<> builder(binOp);
          llvm::Value *shiftAmountVal = llvm::ConstantInt::get(
              binOp->getType(), C->getValue().logBase2());

          if (binOp->getOpcode() == llvm::Instruction::Mul) {
            shiftOp = builder.CreateShl(lhs, shiftAmountVal, "Shl");
          } else if (binOp->getOpcode() == llvm::Instruction::UDiv) {
            shiftOp = builder.CreateLShr(lhs, shiftAmountVal, "Shr");
          } else if (binOp->getOpcode() == llvm::Instruction::SDiv) {
            llvm::Type *type = binOp->getType();
            unsigned width = type->getIntegerBitWidth();
            if (!C->getValue().isStrictlyPositive())
              continue;
            llvm::APInt mask =
                llvm::APInt::getLowBitsSet(width, C->getValue().logBase2());
            llvm::Value *zero = llvm::ConstantInt::get(type, 0);
            llvm::Value *maskOffset = llvm::ConstantInt::get(type, mask);
            llvm::Value *isNeg = builder.CreateICmpSLT(lhs, zero);
            llvm::Value *offset = builder.CreateSelect(isNeg, maskOffset, zero);
            llvm::Value *lhsChanged = builder.CreateAdd(lhs, offset);
            shiftOp = builder.CreateAShr(lhsChanged, shiftAmountVal, "AShr");
          }
          if (shiftOp) {
            binOp->replaceAllUsesWith(shiftOp);
            binOp->eraseFromParent();
            isChanged = true;
          }
        }
      }
    }
    return isChanged ? llvm::PreservedAnalyses::none()
                     : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulDelOptimization", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "mul-del-optimization") {
                    FPM.addPass(MulDelOptimization{});
                    return true;
                  }
                  return false;
                });
          }};
}
