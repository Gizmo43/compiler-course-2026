#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct PowiUnrollPass : llvm::PassInfoMixin<PowiUnrollPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &f : func) {
      for (llvm::Instruction &I : llvm::make_early_inc_range(f)) {

        auto *intrin = llvm::dyn_cast<llvm::IntrinsicInst>(&I);
        if (!intrin || intrin->getIntrinsicID() != llvm::Intrinsic::powi)
          continue;

        llvm::Value *base = intrin->getArgOperand(0);
        llvm::Value *deg = intrin->getArgOperand(1);

        auto *C = llvm::dyn_cast<llvm::ConstantInt>(deg);
        if (!C)
          continue;

        int64_t e = C->getSExtValue();
        if (e < 0 || e > 4)
          continue;

        llvm::IRBuilder<> builder(intrin);
        builder.setFastMathFlags(intrin->getFastMathFlags());

        llvm::Value *result = nullptr;

        switch (e) {
        case 0:
          result = llvm::ConstantFP::get(base->getType(), 1.0);
          break;

        case 1:
          result = base;
          break;

        case 2:
          result = builder.CreateFMul(base, base);
          break;

        case 3: {
          auto *Mul1 = builder.CreateFMul(base, base);
          result = builder.CreateFMul(Mul1, base);
          break;
        }

        case 4: {
          auto *Mul1 = builder.CreateFMul(base, base);
          result = builder.CreateFMul(Mul1, Mul1);
          break;
        }
        }

        if (result) {
          intrin->replaceAllUsesWith(result);
          intrin->eraseFromParent();
          changed = true;
        }
      }
    }

    if (!changed)
      return llvm::PreservedAnalyses::all();

    llvm::PreservedAnalyses PA;
    PA.preserveSet<llvm::CFGAnalyses>();
    return PA;
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PowiUnrollPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "powi-unroll") {
                    FPM.addPass(PowiUnrollPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
