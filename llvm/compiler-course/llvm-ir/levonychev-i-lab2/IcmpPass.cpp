#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct IcmpPass : llvm::PassInfoMixin<IcmpPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool is_changed = false;

    for (auto &BB : func) {
      for (auto Inst = BB.begin(), E = BB.end(); Inst != E; Inst++) {
        if (auto *Icmp = llvm::dyn_cast<llvm::ICmpInst>(&*Inst)) {
          llvm::CmpInst::Predicate Predicate = Icmp->getPredicate();
          llvm::CmpInst::Predicate NewPredicate;

          if (Predicate == llvm::ICmpInst::ICMP_SGT) {
            NewPredicate = llvm::ICmpInst::ICMP_SLE;
          } else if (Predicate == llvm::ICmpInst::ICMP_UGT) {
            NewPredicate = llvm::ICmpInst::ICMP_ULE;
          } else if (Predicate == llvm::ICmpInst::ICMP_SGE) {
            NewPredicate = llvm::ICmpInst::ICMP_SLT;
          } else if (Predicate == llvm::ICmpInst::ICMP_UGE) {
            NewPredicate = llvm::ICmpInst::ICMP_ULT;
          } else {
            continue;
          }

          Icmp->setPredicate(NewPredicate);

          llvm::IRBuilder<> Builder(Icmp->getNextNode());
          llvm::Value *NotIcmp =
              Builder.CreateNot(Icmp, Icmp->getName() + ".not");

          Icmp->replaceAllUsesWith(NotIcmp);
          llvm::cast<llvm::User>(NotIcmp)->setOperand(0, Icmp);

          is_changed = true;
        }
      }
    }

    return is_changed ? llvm::PreservedAnalyses::none()
                      : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "IcmpPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "Icmp") {
                    FPM.addPass(IcmpPass{});
                    return true;
                  }
                  return false;
                });
          }};
}