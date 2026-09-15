#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct IcmpInvertPass : PassInfoMixin<IcmpInvertPass> {
  PreservedAnalyses run(Function &func, FunctionAnalysisManager &) {
    bool changed = false;
    for (auto &bb : func) {
      for (auto &instr : make_early_inc_range(bb)) {
        if (auto *icmp = dyn_cast<ICmpInst>(&instr)) {
          if (icmp->isEquality()) {
            continue;
          }

          ICmpInst::Predicate invPred = icmp->getInversePredicate();

          IRBuilder<> builder(icmp);

          Value *lhs = icmp->getOperand(0);
          Value *rhs = icmp->getOperand(1);

          Value *newCmp =
              builder.CreateICmp(invPred, lhs, rhs, icmp->getName() + ".inv");
          Value *notCmp = builder.CreateNot(newCmp, icmp->getName() + ".not");

          icmp->replaceAllUsesWith(notCmp);
          icmp->eraseFromParent();

          changed = true;
        }
      }
    }
    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "IcmpInversePass", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (name == "IcmpInverse") {
                    FPM.addPass(IcmpInvertPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
