#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct PowiPass : llvm::PassInfoMixin<PowiPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &f : func) {
      for (auto &instr : llvm::make_early_inc_range(f)) {
        if (auto *powi = llvm::dyn_cast<llvm::IntrinsicInst>(&instr)) {
          if (powi->getIntrinsicID() != llvm::Intrinsic::powi)
            continue;

          llvm::Value *base = powi->getArgOperand(0);
          llvm::Value *exp = powi->getArgOperand(1);

          auto *C = llvm::dyn_cast<llvm::ConstantInt>(exp);
          if (!C)
            continue;

          int64_t power = C->getSExtValue();
          if (power < 0 || power > 4)
            continue;

          llvm::IRBuilder<> builder(powi);
          llvm::Value *result = nullptr;

          switch (power) {
          case 0: {
            llvm::Type *scalarTy = base->getType()->getScalarType();
            llvm::Value *one = llvm::ConstantFP::get(scalarTy, 1.0);
            if (auto *vecTy =
                    llvm::dyn_cast<llvm::FixedVectorType>(base->getType())) {
              one = llvm::ConstantVector::getSplat(
                  vecTy->getElementCount(), llvm::cast<llvm::Constant>(one));
            }
            result = one;
            break;
          }
          case 1:
            result = base;
            break;
          case 2:
            result = builder.CreateFMul(base, base);
            break;
          case 3: {
            auto *tmp = builder.CreateFMul(base, base);
            result = builder.CreateFMul(tmp, base);
            break;
          }
          case 4: {
            auto *tmp = builder.CreateFMul(base, base);
            result = builder.CreateFMul(tmp, tmp);
            break;
          }
          }

          if (result) {
            powi->replaceAllUsesWith(result);
            powi->eraseFromParent();
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
  return {LLVM_PLUGIN_API_VERSION, "PowiPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "lab2_llvm_powi") {
                    FPM.addPass(PowiPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
