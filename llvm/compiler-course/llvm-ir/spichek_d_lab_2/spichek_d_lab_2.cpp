#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {
struct LoadStoreEliminationPass : PassInfoMixin<LoadStoreEliminationPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (BasicBlock &BB : F) {
      DenseMap<Value *, Value *> AvailableValues;
      DenseMap<Value *, StoreInst *> LastStore;

      for (Instruction &I : llvm::make_early_inc_range(BB)) {
        Instruction *Inst = &I;

        if (auto *Load = dyn_cast<LoadInst>(Inst)) {
          Value *Ptr = Load->getPointerOperand();

          if (AvailableValues.count(Ptr)) {
            Load->replaceAllUsesWith(AvailableValues[Ptr]);
            Load->eraseFromParent();
            Changed = true;
          } else {
            AvailableValues[Ptr] = Load;
            LastStore.erase(Ptr);
          }
        } else if (auto *Store = dyn_cast<StoreInst>(Inst)) {
          Value *Ptr = Store->getPointerOperand();
          Value *Val = Store->getValueOperand();

          StoreInst *PrevStore = nullptr;
          if (LastStore.count(Ptr)) {
            PrevStore = LastStore[Ptr];
          }
          AvailableValues.clear();
          LastStore.clear();

          if (PrevStore) {
            PrevStore->eraseFromParent();
            Changed = true;
          }

          AvailableValues[Ptr] = Val;
          LastStore[Ptr] = Store;
        } else if (Inst->mayReadOrWriteMemory()) {
          AvailableValues.clear();
          LastStore.clear();
        }
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoadStoreEliminationPass", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (name == "lse") {
                    FPM.addPass(LoadStoreEliminationPass{});
                    return true;
                  }
                  return false;
                });
          }};
}