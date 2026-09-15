#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {
struct InvertRelationalIcmpPass : PassInfoMixin<InvertRelationalIcmpPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (auto &BB : F) {
      for (auto &Inst : make_early_inc_range(BB)) {
        auto *Cmp = dyn_cast<ICmpInst>(&Inst);
        if (!Cmp)
          continue;

        auto Pred = Cmp->getPredicate();

        if (Pred != ICmpInst::ICMP_SGT && Pred != ICmpInst::ICMP_UGT &&
            Pred != ICmpInst::ICMP_SGE && Pred != ICmpInst::ICMP_UGE)
          continue;

        auto InvPred = Cmp->getInversePredicate();
        IRBuilder<> Builder(Cmp);

        Value *LHS = Cmp->getOperand(0);
        Value *RHS = Cmp->getOperand(1);

        Value *NewCmp =
            Builder.CreateICmp(InvPred, LHS, RHS, Cmp->getName() + ".rev");
        Value *NegCmp = Builder.CreateNot(NewCmp, Cmp->getName() + ".not");

        Cmp->replaceAllUsesWith(NegCmp);
        Cmp->eraseFromParent();

        Changed = true;
      }
    }
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InvertRelationalIcmpPass", "v1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "invert-relational-icmp") {
                    FPM.addPass(InvertRelationalIcmpPass());
                    return true;
                  }
                  return false;
                });
          }};
}