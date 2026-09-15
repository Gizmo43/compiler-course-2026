#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {
class ReplaceIcmpPass : public llvm::PassInfoMixin<ReplaceIcmpPass> {
  static llvm::CmpInst::Predicate
  getOppositePredicate(llvm::CmpInst::Predicate Predicate) {
    switch (Predicate) {
    case llvm::CmpInst::ICMP_SGT:
      return llvm::CmpInst::ICMP_SLE;
    case llvm::CmpInst::ICMP_SGE:
      return llvm::CmpInst::ICMP_SLT;
    case llvm::CmpInst::ICMP_UGT:
      return llvm::CmpInst::ICMP_ULE;
    case llvm::CmpInst::ICMP_UGE:
      return llvm::CmpInst::ICMP_ULT;
    default:
      return llvm::CmpInst::BAD_ICMP_PREDICATE;
    }
  }

public:
  llvm::PreservedAnalyses run(llvm::Function &Function,
                              llvm::FunctionAnalysisManager &) {
    llvm::SmallVector<llvm::ICmpInst *, 8> InstructionsToReplace;

    for (llvm::BasicBlock &BB : Function) {
      for (llvm::Instruction &I : BB) {
        auto *Cmp = llvm::dyn_cast<llvm::ICmpInst>(&I);
        if (!Cmp) {
          continue;
        }

        llvm::CmpInst::Predicate OppositePredicate =
            getOppositePredicate(Cmp->getPredicate());
        if (OppositePredicate != llvm::CmpInst::BAD_ICMP_PREDICATE) {
          InstructionsToReplace.push_back(Cmp);
        }
      }
    }

    bool Changed = false;
    for (llvm::ICmpInst *Cmp : InstructionsToReplace) {
      llvm::CmpInst::Predicate OppositePredicate =
          getOppositePredicate(Cmp->getPredicate());

      auto *NewCmp = llvm::cast<llvm::ICmpInst>(Cmp->clone());
      NewCmp->setPredicate(OppositePredicate);
      NewCmp->insertBefore(Cmp->getIterator());
      if (Cmp->hasName()) {
        NewCmp->setName(Cmp->getName() + ".opposite");
      }

      auto *NegatedCmp = llvm::BinaryOperator::CreateNot(
          NewCmp, Cmp->hasName() ? Cmp->getName() + ".not" : llvm::Twine(""),
          Cmp->getIterator());
      NegatedCmp->setDebugLoc(Cmp->getDebugLoc());

      Cmp->replaceAllUsesWith(NegatedCmp);
      Cmp->eraseFromParent();
      Changed = true;
    }

    return Changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ReplaceIcmpPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (Name == "replace-icmp") {
                    FPM.addPass(ReplaceIcmpPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
