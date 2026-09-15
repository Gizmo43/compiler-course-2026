#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {

/// Replaces every llvm.fmuladd.* intrinsic call in the given function with
/// a pair of scalar fmul + fadd instructions, preserving fast-math flags.
struct FMulAddDecomposePass : llvm::PassInfoMixin<FMulAddDecomposePass> {

  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    llvm::SmallVector<llvm::IntrinsicInst *, 8> worklist;

    for (auto &BB : F)
      for (auto &I : BB)
        if (auto *II = llvm::dyn_cast<llvm::IntrinsicInst>(&I))
          if (II->getIntrinsicID() == llvm::Intrinsic::fmuladd)
            worklist.push_back(II);

    for (llvm::IntrinsicInst *II : worklist) {
      // llvm.fmuladd(a, b, c)  =>  fmul a, b  ;  fadd <result>, c
      llvm::Value *A = II->getArgOperand(0);
      llvm::Value *B = II->getArgOperand(1);
      llvm::Value *C = II->getArgOperand(2);

      llvm::IRBuilder<> Builder(II);

      llvm::FastMathFlags FMF = II->getFastMathFlags();
      Builder.setFastMathFlags(FMF);

      llvm::Value *Mul = Builder.CreateFMul(A, B, "fmuladd.mul");
      llvm::Value *Add = Builder.CreateFAdd(Mul, C, "fmuladd.add");

      II->replaceAllUsesWith(Add);
      II->eraseFromParent();

      changed = true;
    }

    return changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // anonymous namespace

//===----------------------------------------------------------------------===//
// Plugin registration
//===----------------------------------------------------------------------===//

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FMulAddDecomposePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (Name == "fmuladd-decompose") {
                    FPM.addPass(FMulAddDecomposePass{});
                    return true;
                  }
                  return false;
                });
          }};
}
