#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {
struct LoadStoreEliminationPass
    : llvm::PassInfoMixin<LoadStoreEliminationPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool changed = false;

    for (llvm::BasicBlock &bb : func) {
      llvm::DenseMap<llvm::Value *, llvm::Value *> memValues;
      llvm::DenseMap<llvm::Value *, llvm::StoreInst *> lastStores;

      for (auto it = bb.begin(); it != bb.end();) {
        llvm::Instruction &inst = *it++;

        if (auto *load = llvm::dyn_cast<llvm::LoadInst>(&inst)) {
          if (load->isVolatile() || load->isAtomic())
            continue;

          llvm::Value *ptr = load->getPointerOperand();
          auto valIt = memValues.find(ptr);

          if (valIt != memValues.end()) {
            load->replaceAllUsesWith(valIt->second);
            load->eraseFromParent();
            changed = true;
          } else {
            memValues[ptr] = load;
          }
        } else if (auto *store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          if (store->isVolatile() || store->isAtomic())
            continue;

          llvm::Value *ptr = store->getPointerOperand();
          llvm::Value *valToStore = store->getValueOperand();

          if (memValues.count(ptr) && memValues[ptr] == valToStore) {
            store->eraseFromParent();
            changed = true;
            continue;
          }

          if (lastStores.count(ptr)) {
            lastStores[ptr]->eraseFromParent();
            changed = true;
          }

          lastStores[ptr] = store;
          memValues[ptr] = valToStore;
        } else if (inst.mayWriteToMemory()) {
          memValues.clear();
          lastStores.clear();
        }
      }
    }

    return changed ? llvm::PreservedAnalyses::none()
                   : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoadStoreEliminationPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "load-store-elim") {
                    FPM.addPass(LoadStoreEliminationPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
