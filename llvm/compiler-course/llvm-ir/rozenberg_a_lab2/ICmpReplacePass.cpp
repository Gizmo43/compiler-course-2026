#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct ICmpReplacePass : llvm::PassInfoMixin<ICmpReplacePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    for (auto &bb : func) {
      for (auto &instr : llvm::make_early_inc_range(bb)) {
        if (auto *icmp = llvm::dyn_cast<llvm::ICmpInst>(&instr)) {
          auto invPred = icmp->getInversePredicate();
          auto icmpName = icmp->getName().str();

          llvm::IRBuilder<> builder(icmp);
          llvm::Value *newICmp =
              builder.CreateICmp(invPred, icmp->getOperand(0),
                                 icmp->getOperand(1), icmpName + "_inv");
          llvm::Value *notNewICmp =
              builder.CreateNot(newICmp, icmpName + "_new");

          icmp->replaceAllUsesWith(notNewICmp);
          icmp->eraseFromParent();
        }
      }
    }
    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ICMPReplacePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "icmp-replace-opposite") {
                    FPM.addPass(ICmpReplacePass{});
                    return true;
                  }
                  return false;
                });
          }};
}
