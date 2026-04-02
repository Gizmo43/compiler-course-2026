#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct ReplaceFmuladdPass : llvm::PassInfoMixin<ReplaceFmuladdPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    std::vector<llvm::IntrinsicInst *> toReplace;

    for (llvm::BasicBlock &BB : func) { // find all fmuladd
      for (llvm::Instruction &I : BB) {
        if (auto *call = llvm::dyn_cast<llvm::IntrinsicInst>(&I)) {
          if (call->getIntrinsicID() == llvm::Intrinsic::fmuladd) {
            toReplace.push_back(call);
          }
        }
      }
    }

    for (llvm::IntrinsicInst *FMulAdd : toReplace) {
      // get args and flags
      llvm::Value *A = FMulAdd->getArgOperand(0);
      llvm::Value *B = FMulAdd->getArgOperand(1);
      llvm::Value *C = FMulAdd->getArgOperand(2);
      llvm::FastMathFlags FMF = FMulAdd->getFastMathFlags();

      llvm::IRBuilder<> Builder(FMulAdd);
      Builder.setFastMathFlags(FMF);

      llvm::Value *Mul = Builder.CreateFMul(A, B, "fmuladd.mul");
      llvm::Value *Add = Builder.CreateFAdd(Mul, C, "fmuladd.add");

      FMulAdd->replaceAllUsesWith(Add);
      FMulAdd->eraseFromParent();

      changed = true;
    }

    return changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ReplaceFmuladdPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "replace-fmuladd") {
                    FPM.addPass(ReplaceFmuladdPass{});
                    return true;
                  }
                  return false;
                });
          }};
}