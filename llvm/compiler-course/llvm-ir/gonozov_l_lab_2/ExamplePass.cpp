#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct ExamplePass : llvm::PassInfoMixin<ExamplePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;
    for (auto &bb : func) {
      for (auto &instr : llvm::make_early_inc_range(bb)) {
        if (auto *callInst = llvm::dyn_cast<llvm::CallInst>(&instr)) {
          if (llvm::Function *calledFunc = callInst->getCalledFunction()) {
            llvm::Intrinsic::ID id = calledFunc->getIntrinsicID();
            if (id == llvm::Intrinsic::powi) {
              auto *lhs = callInst->getOperand(0);
              auto *rhs = callInst->getOperand(1);
              int64_t power =
                  llvm::dyn_cast<llvm::ConstantInt>(rhs)->getSExtValue();
              if (power >= 0 && power <= 4) {
                llvm::IRBuilder<> builder(callInst);
                llvm::Value *newValue = nullptr;
                if (power == 0) {
                  newValue = llvm::ConstantFP::get(lhs->getType(), 1.0);
                } else if (power == 1) {
                  newValue = lhs;
                } else if (power == 2) {
                  newValue = builder.CreateFMul(lhs, lhs);
                } else if (power == 3) {
                  auto *intermediate = builder.CreateFMul(lhs, lhs);
                  newValue = builder.CreateFMul(intermediate, lhs);
                } else if (power == 4) {
                  auto *intermediate = builder.CreateFMul(lhs, lhs);
                  newValue = builder.CreateFMul(intermediate, intermediate);
                }

                callInst->replaceAllUsesWith(newValue);
                callInst->eraseFromParent();
                changed = true;
              }
            }
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
  return {LLVM_PLUGIN_API_VERSION, "ExamplePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "example") {
                    FPM.addPass(ExamplePass{});
                    return true;
                  }
                  return false;
                });
          }};
}
