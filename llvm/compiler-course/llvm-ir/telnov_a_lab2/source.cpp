#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

struct InvertRelationalIcmpPass
    : public PassInfoMixin<InvertRelationalIcmpPass> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (auto &BB : F) {
      for (auto &Inst : make_early_inc_range(BB)) {
        auto *Cmp = dyn_cast<ICmpInst>(&Inst);
        if (!Cmp)
          continue;

        auto Pred = Cmp->getPredicate();

        // интересуют только gt / ge
        if (Pred != ICmpInst::ICMP_SGT && Pred != ICmpInst::ICMP_UGT &&
            Pred != ICmpInst::ICMP_SGE && Pred != ICmpInst::ICMP_UGE)
          continue;

        IRBuilder<> Builder(Cmp);

        // берём противоположный предикат (le, lt и т.д.)
        auto InvPred = Cmp->getInversePredicate();

        Value *NewCmp =
            Builder.CreateICmp(InvPred, Cmp->getOperand(0), Cmp->getOperand(1),
                               Cmp->getName() + ".inv");

        // делаем отрицание
        Value *Neg = Builder.CreateNot(NewCmp, Cmp->getName() + ".not");

        Cmp->replaceAllUsesWith(Neg);
        Cmp->eraseFromParent();

        Changed = true;
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InvertRelationalIcmpPass", "1.0",
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