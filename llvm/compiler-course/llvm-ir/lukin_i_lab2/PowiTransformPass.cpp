#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct ExamplePass : llvm::PassInfoMixin<ExamplePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool is_changed = false;

    llvm::SmallVector<llvm::Instruction *, 8> to_delete;

    for (auto &base_block : func) {
      for (auto &instruction : base_block) {
        if (auto *call = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
          if (call->getIntrinsicID() == llvm::Intrinsic::powi) {
            llvm::Value *base = call->getArgOperand(0);
            llvm::Value *exp = call->getArgOperand(1);

            if (auto *const_int = llvm::dyn_cast<llvm::ConstantInt>(exp)) {
              int64_t exp_val = const_int->getSExtValue();
              if (exp_val >= 0 && exp_val <= 4) {
                llvm::IRBuilder<> builder(call);
                llvm::Value *new_val = nullptr;

                if (exp_val == 0) {
                  new_val = llvm::ConstantFP::get(base->getType(), 1.0);
                } else if (exp_val == 1) {
                  new_val = base;
                } else if (exp_val == 2) {
                  new_val = builder.CreateFMul(base, base);
                } else if (exp_val == 3) {
                  llvm::Value *mul = builder.CreateFMul(base, base);
                  new_val = builder.CreateFMul(mul, base);
                } else if (exp_val == 4) {
                  llvm::Value *mul = builder.CreateFMul(base, base);
                  new_val = builder.CreateFMul(mul, mul);
                }

                call->replaceAllUsesWith(new_val);
                to_delete.push_back(call);
                is_changed = true;
              }
            }
          }
        }
      }
    }

    for (auto *instruction : to_delete) {
      instruction->eraseFromParent();
    }

    return is_changed ? llvm::PreservedAnalyses::none()
                      : llvm::PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PowiTransformPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "powi") {
                    FPM.addPass(ExamplePass{});
                    return true;
                  }
                  return false;
                });
          }};
}
