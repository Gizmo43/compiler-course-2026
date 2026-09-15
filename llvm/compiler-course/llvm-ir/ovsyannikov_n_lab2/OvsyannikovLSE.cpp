#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <vector>

using namespace llvm;

namespace {
struct OvsyannikovLSEPass : public PassInfoMixin<OvsyannikovLSEPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;
    for (auto &BB : F) {
      DenseMap<Value *, Value *> LastValues;
      DenseMap<Value *, StoreInst *> LastStores;
      std::vector<Instruction *> ToDelete;

      for (auto &I : BB) {
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
          if (LI->isVolatile() || LI->isAtomic())
            continue;
          Value *Ptr = LI->getPointerOperand();
          if (LastValues.count(Ptr)) {
            LI->replaceAllUsesWith(LastValues[Ptr]);
            ToDelete.push_back(LI);
            Changed = true;
          } else {
            LastValues[Ptr] = LI;
          }
        } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
          if (SI->isVolatile() || SI->isAtomic())
            continue;
          Value *Ptr = SI->getPointerOperand();
          if (LastStores.count(Ptr)) {
            ToDelete.push_back(LastStores[Ptr]);
            Changed = true;
          }
          LastStores[Ptr] = SI;
          LastValues[Ptr] = SI->getValueOperand();
        } else if (I.mayWriteToMemory()) {
          LastValues.clear();
          LastStores.clear();
        }
      }
      for (auto *I : ToDelete)
        I->eraseFromParent();
    }
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OvsyannikovLSE", "1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ovsyannikov-lse") {
                    FPM.addPass(OvsyannikovLSEPass());
                    return true;
                  }
                  return false;
                });
          }};
}
