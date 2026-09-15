#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct ReplaceICmpPass : public PassInfoMixin<ReplaceICmpPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (BasicBlock &BB : F) {
      for (Instruction &I : make_early_inc_range(BB)) {
        auto *ICmp = dyn_cast<ICmpInst>(&I);
        if (!ICmp)
          continue;

        CmpInst::Predicate Pred = ICmp->getPredicate();
        CmpInst::Predicate NewPred = CmpInst::BAD_ICMP_PREDICATE;
        bool IsReplaced = false;

        switch (Pred) {
        case CmpInst::ICMP_SGT:
          NewPred = CmpInst::ICMP_SLE;
          IsReplaced = true;
          break;
        case CmpInst::ICMP_SGE:
          NewPred = CmpInst::ICMP_SLT;
          IsReplaced = true;
          break;
        case CmpInst::ICMP_UGT:
          NewPred = CmpInst::ICMP_ULE;
          IsReplaced = true;
          break;
        case CmpInst::ICMP_UGE:
          NewPred = CmpInst::ICMP_ULT;
          IsReplaced = true;
          break;
        default:
          break;
        }

        if (!IsReplaced)
          continue;

        Value *NewICmp = new ICmpInst(ICmp, NewPred, ICmp->getOperand(0),
                                      ICmp->getOperand(1), "cmp.rev");

        Value *Not = BinaryOperator::CreateNot(NewICmp, "cmp.not", ICmp);

        ICmp->replaceAllUsesWith(Not);
        ICmp->eraseFromParent();

        Changed = true;
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ReplaceICmpPass", "1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "replace-icmp") {
                    FPM.addPass(ReplaceICmpPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
