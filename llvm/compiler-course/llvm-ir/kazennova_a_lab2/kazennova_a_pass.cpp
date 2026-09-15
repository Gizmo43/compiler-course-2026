#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

class KazennovaFMulAddPass : public PassInfoMixin<KazennovaFMulAddPass> {
public:
  PreservedAnalyses run(Function &f, FunctionAnalysisManager &) {
    bool changed = false;

    for (BasicBlock &bb : f) {
      for (Instruction &i : make_early_inc_range(bb)) {
        auto *call = dyn_cast<CallInst>(&i);
        if (!call)
          continue;

        Function *callee = call->getCalledFunction();
        if (!callee)
          continue;

        if (callee->getIntrinsicID() == Intrinsic::fmuladd) {
          replaceFMulAdd(call);
          changed = true;
        }
      }
    }

    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

private:
  void replaceFMulAdd(CallInst *call) {
    if (call->use_empty()) {
      call->eraseFromParent();
      return;
    }

    IRBuilder<> builder(call);

    Value *a = call->getOperand(0);
    Value *b = call->getOperand(1);
    Value *c = call->getOperand(2);

    Value *mul = builder.CreateFMul(a, b);
    Value *add = builder.CreateFAdd(mul, c);

    if (auto *fmul = dyn_cast<Instruction>(mul))
      fmul->copyFastMathFlags(call);
    if (auto *fadd = dyn_cast<Instruction>(add))
      fadd->copyFastMathFlags(call);

    if (auto *mul_inst = dyn_cast<Instruction>(mul)) {
      mul_inst->setDebugLoc(call->getDebugLoc());
      mul_inst->copyMetadata(*call);
    }
    if (auto *add_inst = dyn_cast<Instruction>(add)) {
      add_inst->setDebugLoc(call->getDebugLoc());
      add_inst->copyMetadata(*call);
    }

    call->replaceAllUsesWith(add);
    call->eraseFromParent();
  }
};

} // namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "KazennovaFMulAddPass", "v0.1",
          [](PassBuilder &pb) {
            pb.registerPipelineParsingCallback(
                [](StringRef name, FunctionPassManager &fpm,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (name == "decompose-fmuladd") {
                    fpm.addPass(KazennovaFMulAddPass());
                    return true;
                  }
                  return false;
                });
          }};
}