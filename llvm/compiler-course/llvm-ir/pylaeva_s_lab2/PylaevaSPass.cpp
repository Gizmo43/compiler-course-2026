#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct PylaevaSPass : llvm::PassInfoMixin<PylaevaSPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    for (llvm::BasicBlock &bb : func) {
      for (llvm::Instruction &instr : llvm::make_early_inc_range(bb)) {
        if (llvm::BinaryOperator *binOp =
                llvm::dyn_cast<llvm::BinaryOperator>(&instr)) {

          llvm::Value *lhs = binOp->getOperand(0);
          llvm::Value *rhs = binOp->getOperand(1);
          llvm::IRBuilder<> builder(binOp);

          if (binOp->getOpcode() == llvm::Instruction::FRem) {

            // trunc(a/b)
            // a-(trunc(a/b)*b)
            llvm::Value *fdiv = builder.CreateUnaryIntrinsic(
                llvm::Intrinsic::trunc, builder.CreateFDiv(lhs, rhs));
            llvm::Value *fsub =
                builder.CreateFSub(lhs, builder.CreateFMul(fdiv, rhs));

            binOp->replaceAllUsesWith(fsub);
            binOp->eraseFromParent();
            changed = true;

          } else if (binOp->getOpcode() == llvm::Instruction::URem) {

            // a-((a/b)*b)
            llvm::Value *sub = builder.CreateSub(
                lhs, builder.CreateMul(builder.CreateUDiv(lhs, rhs), rhs));

            binOp->replaceAllUsesWith(sub);
            binOp->eraseFromParent();
            changed = true;

          } else if (binOp->getOpcode() == llvm::Instruction::SRem) {

            // a-((a/b)*b)
            llvm::Value *sub = builder.CreateSub(
                lhs, builder.CreateMul(builder.CreateSDiv(lhs, rhs), rhs));

            binOp->replaceAllUsesWith(sub);
            binOp->eraseFromParent();
            changed = true;
          }
        }
      }
    }

    if (changed) {
      return llvm::PreservedAnalyses::none();
    }
    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PylaevaSPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "pylaeva_s_lab2") {
                    FPM.addPass(PylaevaSPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
