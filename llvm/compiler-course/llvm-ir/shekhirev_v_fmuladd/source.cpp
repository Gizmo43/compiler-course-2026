#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {

struct FmuladdDecomposePass : llvm::PassInfoMixin<FmuladdDecomposePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &basic_block : func) {
      for (auto &instr : llvm::make_early_inc_range(basic_block)) {

        auto *intr = llvm::dyn_cast<llvm::IntrinsicInst>(&instr);
        if (!intr || intr->getIntrinsicID() != llvm::Intrinsic::fmuladd) {
          continue;
        }

        changed = true;

        llvm::IRBuilder<> builder(intr);
        builder.setFastMathFlags(intr->getFastMathFlags());

        llvm::Value *a = intr->getArgOperand(0);
        llvm::Value *b = intr->getArgOperand(1);
        llvm::Value *c = intr->getArgOperand(2);

        llvm::Value *mul = builder.CreateFMul(a, b, "decomp.mul");
        llvm::Value *add = builder.CreateFAdd(mul, c, "decomp.add");

        if (auto *mul_inst = llvm::dyn_cast<llvm::Instruction>(mul)) {
          mul_inst->copyMetadata(*intr);
        }
        if (auto *add_inst = llvm::dyn_cast<llvm::Instruction>(add)) {
          add_inst->copyMetadata(*intr);
        }

        intr->replaceAllUsesWith(add);
        intr->eraseFromParent();
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
  return {LLVM_PLUGIN_API_VERSION, "fmuladd_decompose", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "fmuladd_decompose") {
                    FPM.addPass(FmuladdDecomposePass{});
                    return true;
                  }
                  return false;
                });
          }};
}