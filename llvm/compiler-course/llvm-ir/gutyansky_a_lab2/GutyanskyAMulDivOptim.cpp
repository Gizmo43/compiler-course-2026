#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"

namespace {
struct GutyanskyAMulDivOptim : llvm::PassInfoMixin<GutyanskyAMulDivOptim> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {

    bool hasChanges = false;

    for (auto &basicBlock : func) {
      for (auto &insn : llvm::make_early_inc_range(basicBlock)) {
        if (!insn.isBinaryOp())
          continue;

        const auto opcode = insn.getOpcode();
        if (opcode != llvm::Instruction::Mul &&
            opcode != llvm::Instruction::UDiv &&
            opcode != llvm::Instruction::SDiv)
          continue;

        const auto *rhs = insn.getOperand(1);
        const auto *constInt = llvm::dyn_cast<llvm::ConstantInt>(rhs);

        if (!constInt)
          continue;

        const auto apVal = constInt->getValue();
        const auto logBase2 = apVal.logBase2();

        if (!apVal.isPowerOf2() || !apVal.isStrictlyPositive())
          continue;

        auto *type = insn.getType();
        auto *lhs = insn.getOperand(0);
        auto *shiftRhs =
            llvm::cast<llvm::Value>(llvm::ConstantInt::get(type, logBase2));

        llvm::IRBuilder<> irBuilder(&insn);
        llvm::Value *newInsn = nullptr;

        if (opcode == llvm::Instruction::Mul) {
          newInsn = irBuilder.CreateShl(lhs, shiftRhs, "shl");
        } else if (opcode == llvm::Instruction::UDiv) {
          newInsn = irBuilder.CreateLShr(lhs, shiftRhs, "lshr");
        } else if (opcode == llvm::Instruction::SDiv) {
          const auto width = insn.getType()->getIntegerBitWidth();
          const auto corrAp = llvm::APInt::getLowBitsSet(width, logBase2);

          auto *cZero = llvm::cast<llvm::Value>(
              llvm::ConstantInt::get(insn.getType(), 0));
          auto *scmp = irBuilder.CreateICmpSLT(lhs, cZero);
          auto *corrSel =
              irBuilder.CreateSelect(scmp, irBuilder.getInt(corrAp), cZero);
          auto *corrected = irBuilder.CreateAdd(lhs, corrSel);

          newInsn = irBuilder.CreateAShr(corrected, shiftRhs);
        }

        if (newInsn) {
          insn.replaceAllUsesWith(newInsn);
          insn.eraseFromParent();
          hasChanges = true;
        }
      }
    }

    return hasChanges ? llvm::PreservedAnalyses::none()
                      : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "GutyanskyAMulDivOptim", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "gutyansky-a-mul-div-optim") {
                    FPM.addPass(GutyanskyAMulDivOptim{});
                    return true;
                  }
                  return false;
                });
          }};
}
