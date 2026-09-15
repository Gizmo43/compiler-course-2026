#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct LE_FmullAddDecPass : llvm::PassInfoMixin<LE_FmullAddDecPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    for (llvm::BasicBlock &basic_block : func) {
      for (llvm::Instruction &instr : llvm::make_early_inc_range(basic_block)) {
        if (llvm::IntrinsicInst *intrinsic =
                llvm::dyn_cast<llvm::IntrinsicInst>(&instr)) {
          if (intrinsic->getIntrinsicID() == llvm::Intrinsic::fmuladd) {
            llvm::Value *a = intrinsic->getArgOperand(0);
            llvm::Value *b = intrinsic->getArgOperand(1);
            llvm::Value *c = intrinsic->getArgOperand(2);

            llvm::FastMathFlags flags = intrinsic->getFastMathFlags();
            llvm::IRBuilder<> builder(intrinsic);
            builder.setFastMathFlags(flags);

            llvm::Value *mul_res = builder.CreateFMul(a, b, "fmul");
            llvm::Value *add_res = builder.CreateFAdd(mul_res, c, "fadd");

            intrinsic->replaceAllUsesWith(add_res);
            intrinsic->eraseFromParent();
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
  return {LLVM_PLUGIN_API_VERSION, "LE_FmullAddDecPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "fmuladddec") {
                    FPM.addPass(LE_FmullAddDecPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
