#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct ChyokotovAFremPass : llvm::PassInfoMixin<ChyokotovAFremPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    for (auto &bb : func) {
      for (auto &instr : llvm::make_early_inc_range(bb)) {
        if (auto *binOp = llvm::dyn_cast<llvm::BinaryOperator>(&instr)) {
          auto opCode = binOp->getOpcode();
          if (opCode == llvm::Instruction::FRem) {
            auto *lhs = binOp->getOperand(0);
            auto *rhs = binOp->getOperand(1);
            llvm::IRBuilder<> builder(binOp);

            llvm::Value *div = builder.CreateUnaryIntrinsic(
                llvm::Intrinsic::trunc, builder.CreateFDiv(lhs, rhs));
            auto *mul = builder.CreateFMul(div, rhs);
            auto *sub = builder.CreateFSub(lhs, mul);

            binOp->replaceAllUsesWith(sub);
            binOp->eraseFromParent();
            changed = true;
          } else if (opCode == llvm::Instruction::URem) {
            auto *lhs = binOp->getOperand(0);
            auto *rhs = binOp->getOperand(1);
            llvm::IRBuilder<> builder(binOp);

            auto *div = builder.CreateUDiv(lhs, rhs);
            auto *mul = builder.CreateMul(div, rhs);
            auto *sub = builder.CreateSub(lhs, mul);

            binOp->replaceAllUsesWith(sub);
            binOp->eraseFromParent();
            changed = true;
          } else if (opCode == llvm::Instruction::SRem) {
            auto *lhs = binOp->getOperand(0);
            auto *rhs = binOp->getOperand(1);
            llvm::IRBuilder<> builder(binOp);

            auto *div = builder.CreateSDiv(lhs, rhs);
            auto *mul = builder.CreateMul(div, rhs);
            auto *sub = builder.CreateSub(lhs, mul);

            binOp->replaceAllUsesWith(sub);
            binOp->eraseFromParent();
            changed = true;
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
  return {LLVM_PLUGIN_API_VERSION, "ChyokotovAFremPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "chyokotov_a_frem") {
                    FPM.addPass(ChyokotovAFremPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
