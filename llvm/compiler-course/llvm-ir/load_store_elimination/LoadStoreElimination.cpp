#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

struct TrackedValue {
  Value *CurrentValue = nullptr;
  StoreInst *LastStore = nullptr;
  bool HasKnownValue = false;
  bool WasReadAfterStore = false;
};

class LoadStoreEliminationPass
    : public PassInfoMixin<LoadStoreEliminationPass> {
  using MemoryState = DenseMap<Value *, TrackedValue>;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (BasicBlock &BB : F) {
      MemoryState State;
      SmallVector<Instruction *, 16> ToErase;

      for (Instruction &I : BB) {
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
          Changed |= processLoad(*LI, State, ToErase);
          continue;
        }

        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          Changed |= processStore(*SI, State, ToErase);
          continue;
        }

        invalidateForInstruction(I, State);
      }

      for (Instruction *Inst : ToErase)
        Inst->eraseFromParent();
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }

private:
  bool processLoad(LoadInst &LI, MemoryState &State,
                   SmallVector<Instruction *, 16> &ToErase) {
    if (LI.isVolatile() || LI.isAtomic()) {
      invalidateForInstruction(LI, State);
      return false;
    }

    Value *Pointer = LI.getPointerOperand();
    auto &Tracked = State[Pointer];

    if (Tracked.HasKnownValue) {
      LI.replaceAllUsesWith(Tracked.CurrentValue);
      ToErase.push_back(&LI);
      return true;
    }

    Tracked.CurrentValue = &LI;
    Tracked.HasKnownValue = true;
    Tracked.WasReadAfterStore = true;
    return false;
  }

  bool processStore(StoreInst &SI, MemoryState &State,
                    SmallVector<Instruction *, 16> &ToErase) {
    if (SI.isVolatile() || SI.isAtomic()) {
      invalidateForInstruction(SI, State);
      return false;
    }

    Value *Pointer = SI.getPointerOperand();
    StoreInst *RedundantStore = nullptr;

    auto It = State.find(Pointer);
    if (It != State.end() && It->second.LastStore != nullptr &&
        !It->second.WasReadAfterStore) {
      RedundantStore = It->second.LastStore;
    }

    State.clear();

    if (RedundantStore != nullptr)
      ToErase.push_back(RedundantStore);

    auto &Tracked = State[Pointer];
    Tracked.CurrentValue = SI.getValueOperand();
    Tracked.LastStore = &SI;
    Tracked.HasKnownValue = true;
    Tracked.WasReadAfterStore = false;
    return RedundantStore != nullptr;
  }

  void invalidateForInstruction(Instruction &I, MemoryState &State) {
    if (I.mayWriteToMemory()) {
      State.clear();
      return;
    }

    if (I.mayReadFromMemory()) {
      for (auto &Entry : State) {
        Entry.second.LastStore = nullptr;
        Entry.second.WasReadAfterStore = false;
      }
    }
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoadStoreEliminationPass", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "load-store-elimination") {
                    FPM.addPass(LoadStoreEliminationPass());
                    return true;
                  }
                  return false;
                });
          }};
}