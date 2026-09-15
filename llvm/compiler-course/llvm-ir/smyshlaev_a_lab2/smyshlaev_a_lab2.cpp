#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct MulShiftReplacePass : llvm::PassInfoMixin<MulShiftReplacePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    for (auto &bb : func) {
      for (auto &instr : llvm::make_early_inc_range(bb)) {
        if (auto *binOp = llvm::dyn_cast<llvm::BinaryOperator>(&instr)) {
          unsigned opCode = binOp->getOpcode();
          if (opCode != llvm::Instruction::Mul &&
              opCode != llvm::Instruction::UDiv &&
              opCode != llvm::Instruction::SDiv)
            continue;
          auto *lhs = binOp->getOperand(0);
          auto *rhs = binOp->getOperand(1);
          auto *lhsConstant = llvm::dyn_cast<llvm::ConstantInt>(lhs);
          auto *rhsConstant = llvm::dyn_cast<llvm::ConstantInt>(rhs);
          if (!(lhsConstant || rhsConstant))
            continue;
          llvm::IRBuilder<> builder(binOp);
          if (lhsConstant) {
            if (lhsConstant->getValue().isPowerOf2()) {
              uint64_t shift = lhsConstant->getValue().logBase2();
              auto *shiftConst =
                  llvm::ConstantInt::get(binOp->getType(), shift);
              if (opCode == llvm::Instruction::Mul) {
                auto *newInstr = builder.CreateShl(rhs, shiftConst);
                binOp->replaceAllUsesWith(newInstr);
                binOp->eraseFromParent();
                changed = true;
                continue;
              }
            }
          }
          if (rhsConstant) {
            if (rhsConstant->getValue().isPowerOf2()) {
              llvm::Value *newInstr = nullptr;
              uint64_t shift = rhsConstant->getValue().logBase2();
              auto *shiftConst =
                  llvm::ConstantInt::get(binOp->getType(), shift);
              if (opCode == llvm::Instruction::Mul)
                newInstr = builder.CreateShl(lhs, shiftConst);
              else if (opCode == llvm::Instruction::UDiv)
                newInstr = builder.CreateLShr(lhs, shiftConst);
              else if (opCode == llvm::Instruction::SDiv) {
                if (binOp->isExact()) {
                  newInstr = builder.CreateAShr(lhs, shiftConst);
                } else {
                  uint64_t maskVal = (1ULL << shift) - 1;
                  auto *maskConst =
                      llvm::ConstantInt::get(binOp->getType(), maskVal);
                  auto *zeroConst = llvm::ConstantInt::get(binOp->getType(), 0);
                  auto *isNeg = builder.CreateICmpSLT(lhs, zeroConst);
                  auto *adjustedLhs = builder.CreateAdd(lhs, maskConst);
                  auto *selectedVal =
                      builder.CreateSelect(isNeg, adjustedLhs, lhs);
                  newInstr = builder.CreateAShr(selectedVal, shiftConst);
                }
              }
              if (newInstr) {
                binOp->replaceAllUsesWith(newInstr);
                binOp->eraseFromParent();
                changed = true;
              }
            }
          }
        }
      }
    }
    return changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulShiftReplacePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "mulshift") {
                    FPM.addPass(MulShiftReplacePass{});
                    return true;
                  }
                  return false;
                });
          }};
}
