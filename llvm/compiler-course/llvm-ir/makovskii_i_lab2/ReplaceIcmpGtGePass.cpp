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
        if (auto *CmpInst = dyn_cast<ICmpInst>(&Inst)) {
          if (CmpInst->isEquality()) {
            continue;
          }

          auto InvPred = CmpInst->getInversePredicate();
          IRBuilder<> Builder(CmpInst);

          Value *LHS = CmpInst->getOperand(0);
          Value *RHS = CmpInst->getOperand(1);

          Value *NewCmp = Builder.CreateICmp(InvPred, LHS, RHS,
                                             CmpInst->getName() + ".rev");
          Value *NegCmp =
              Builder.CreateNot(NewCmp, CmpInst->getName() + ".not");

          CmpInst->replaceAllUsesWith(NegCmp);
          CmpInst->eraseFromParent();

          Changed = true;
        }
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