#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct InstrumentFunctionsPass : llvm::PassInfoMixin<InstrumentFunctionsPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    llvm::Module *M = func.getParent();
    llvm::LLVMContext &Ctx = M->getContext();
    llvm::FunctionType *HookTy =
        llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), false);

    llvm::FunctionCallee StartFn = M->getOrInsertFunction("instrument_start", HookTy);
    llvm::FunctionCallee EndFn = M->getOrInsertFunction("instrument_end", HookTy);

    std::vector<llvm::ReturnInst *> returns;

    for (llvm::BasicBlock &BB : func) { // find all returns
      if (auto *RI = llvm::dyn_cast<llvm::ReturnInst>(BB.getTerminator())) {
        returns.push_back(RI);
      }
    }

    llvm::BasicBlock &Entry = func.getEntryBlock();
    llvm::IRBuilder<> Builder(&Entry, Entry.getFirstInsertionPt());
    Builder.CreateCall(StartFn);

    for (llvm::ReturnInst *RI : returns) {
      llvm::IRBuilder<> Builder(RI);
      Builder.CreateCall(EndFn);
    }

    return llvm::PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InstrumentFunctionsPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "instrument-functions") {
                    FPM.addPass(InstrumentFunctionsPass{});
                    return true;
                  }
                  return false;
                });
          }};
}