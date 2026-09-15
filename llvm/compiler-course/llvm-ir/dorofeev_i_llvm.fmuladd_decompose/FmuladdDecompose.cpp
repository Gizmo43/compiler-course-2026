#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {

struct FmuladdDecomposePass : llvm::PassInfoMixin<FmuladdDecomposePass> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &) {
    bool Changed = false;
    llvm::SmallVector<llvm::CallInst *, 4> CallsToReplace;

    // Сначала собираем все вызовы fmuladd
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *Call = llvm::dyn_cast<llvm::CallInst>(&I)) {
          if (Call->getIntrinsicID() == llvm::Intrinsic::fmuladd) {
            CallsToReplace.push_back(Call);
          }
        }
      }
    }

    // Заменяем собранные вызовы
    for (auto *Call : CallsToReplace) {
      llvm::IRBuilder<> Builder(Call);

      // ВАЖНО: Копируем флаги быстрой математики
      Builder.setFastMathFlags(Call->getFastMathFlags());

      llvm::Value *A = Call->getArgOperand(0);
      llvm::Value *B = Call->getArgOperand(1);
      llvm::Value *C = Call->getArgOperand(2);

      llvm::Value *Mul = Builder.CreateFMul(A, B, "fmuladd.mul");
      llvm::Value *Add = Builder.CreateFAdd(Mul, C, "fmuladd.add");

      Call->replaceAllUsesWith(Add);
      Call->eraseFromParent();
      Changed = true;
    }

    return Changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FmuladdDecomposePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "fmuladd-decompose") {
                    FPM.addPass(FmuladdDecomposePass{});
                    return true;
                  }
                  return false;
                });
          }};
}