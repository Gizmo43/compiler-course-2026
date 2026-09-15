#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

class PowiLoweringPass : public PassInfoMixin<PowiLoweringPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool updated = false;

    for (BasicBlock &BB : F) {
      for (auto It = BB.begin(); It != BB.end();) {
        Instruction *Inst = &*It++;

        auto *Call = dyn_cast<CallInst>(Inst);
        if (!Call)
          continue;

        auto *Fn =
            dyn_cast<Function>(Call->getCalledOperand()->stripPointerCasts());
        if (!Fn || Fn->getIntrinsicID() != Intrinsic::powi)
          continue;

        auto *ExpConst = dyn_cast<ConstantInt>(Call->getArgOperand(1));
        if (!ExpConst)
          continue;

        int64_t Exp = ExpConst->getSExtValue();
        if (Exp < 0 || Exp > 4)
          continue;

        Value *Base = Call->getArgOperand(0);
        IRBuilder<> B(Call);
        Value *Result = nullptr;

        switch (Exp) {
        case 0:
          Result = ConstantFP::get(Base->getType(), 1.0);
          break;
        case 1:
          Result = Base;
          break;
        case 2:
          Result = B.CreateFMul(Base, Base);
          break;
        case 3: {
          Value *Sq = B.CreateFMul(Base, Base);
          Result = B.CreateFMul(Sq, Base);
          break;
        }
        case 4: {
          Value *Sq = B.CreateFMul(Base, Base);
          Result = B.CreateFMul(Sq, Sq);
          break;
        }
        }

        if (Result) {
          Call->replaceAllUsesWith(Result);
          Call->eraseFromParent();
          updated = true;
        }
      }
    }
    return updated ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PowiLoweringPass", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "example2") {
                    FPM.addPass(PowiLoweringPass());
                    return true;
                  }
                  return false;
                });
          }};
}