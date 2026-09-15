#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

struct TrackedCell {
  Value *KnownValue = nullptr;
  StoreInst *LastStore = nullptr;
  bool Valid = false;
  bool WasLoaded = false;

  void resetAll() {
    KnownValue = nullptr;
    LastStore = nullptr;
    Valid = false;
    WasLoaded = false;
  }

  void resetStoreOnly() {
    LastStore = nullptr;
    WasLoaded = false;
  }
};

class LSEPass : public PassInfoMixin<LSEPass> {

  using MemoryMap = DenseMap<Value *, TrackedCell>;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;
    for (BasicBlock &BB : F) {
      MemoryMap Memory;
      SmallVector<Instruction *, 16> ToErase;

      for (Instruction &I : BB) {
        if (processLoad(I, Memory, ToErase))
          Changed = true;
        else if (processStore(I, Memory, ToErase))
          Changed = true;
        else
          invalidate(I, Memory);
      }

      for (Instruction *Inst : ToErase) {
        if (Inst->use_empty())
          Inst->eraseFromParent();
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }

private:
  bool processLoad(Instruction &I, MemoryMap &Memory,
                   SmallVector<Instruction *, 16> &ToErase) {

    auto *LI = dyn_cast<LoadInst>(&I);
    if (!LI)
      return false;

    if (LI->isVolatile() || LI->isAtomic())
      return true;

    Value *Ptr = LI->getPointerOperand();
    auto &Cell = Memory[Ptr];

    if (Cell.Valid && Cell.KnownValue) {
      LI->replaceAllUsesWith(Cell.KnownValue);
      ToErase.push_back(LI);
      return true;
    }

    Cell.KnownValue = LI;
    Cell.Valid = true;
    Cell.WasLoaded = true;

    return false;
  }

  bool processStore(Instruction &I, MemoryMap &Memory,
                    SmallVector<Instruction *, 16> &ToErase) {

    auto *SI = dyn_cast<StoreInst>(&I);
    if (!SI) {
      return false;
    }
    if (SI->isVolatile() || SI->isAtomic()) {
      return true;
    }
    invalidate(I, Memory);

    Value *Ptr = SI->getPointerOperand();
    auto &Cell = Memory[Ptr];

    if (Cell.LastStore && Cell.Valid && !Cell.WasLoaded) {
      ToErase.push_back(Cell.LastStore);
    }

    Cell.KnownValue = SI->getValueOperand();
    Cell.LastStore = SI;
    Cell.Valid = true;
    Cell.WasLoaded = false;

    return true;
  }

  void invalidate(Instruction &I, MemoryMap &Memory) {

    if (I.mayWriteToMemory()) {
      for (auto &Entry : Memory) {
        Entry.second.resetAll();
      }
      return;
    }

    if (I.mayReadFromMemory()) {
      for (auto &Entry : Memory) {
        Entry.second.resetStoreOnly();
      }
    }
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "KiselevLoadStoreElimination", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "kiselev-load-store-elimination") {
                    FPM.addPass(LSEPass());
                    return true;
                  }
                  return false;
                });
          }};
}