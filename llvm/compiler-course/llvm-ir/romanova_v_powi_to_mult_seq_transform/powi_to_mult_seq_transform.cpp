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
struct PowiToMultSeqPass : llvm::PassInfoMixin<PowiToMultSeqPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &f : func) {
      for (llvm::Instruction &I : llvm::make_early_inc_range(f)) {
        auto *intrinsic = llvm::dyn_cast<llvm::IntrinsicInst>(&I);
        if (!intrinsic || intrinsic->getIntrinsicID() != llvm::Intrinsic::powi)
          continue;

        llvm::Value *base = intrinsic->getArgOperand(0);
        llvm::Value *pow = intrinsic->getArgOperand(1);

        auto *C = llvm::dyn_cast<llvm::ConstantInt>(pow);
        if (!C)
          continue;

        int64_t pow_val = C->getSExtValue();

        llvm::IRBuilder<> builder(intrinsic);

        llvm::Value *new_val = nullptr;

        switch (pow_val) {
        case 0:
          new_val = llvm::ConstantFP::get(base->getType(), 1.0);
          break;
        case 1:
          new_val = base;
          break;
        case 2:
          new_val = builder.CreateFMul(base, base);
          break;
        case 3: {
          auto *Mul = builder.CreateFMul(base, base);
          new_val = builder.CreateFMul(Mul, base);
          break;
        }
        case 4: {
          auto *Mul = builder.CreateFMul(base, base);
          new_val = builder.CreateFMul(Mul, Mul);
          break;
        }

        default:
          break;
        }

        if (new_val) {
          intrinsic->replaceAllUsesWith(new_val);
          intrinsic->eraseFromParent();
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
  return {LLVM_PLUGIN_API_VERSION, "PowiToMultSeqPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "powi-to-mult-seq") {
                    FPM.addPass(PowiToMultSeqPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
