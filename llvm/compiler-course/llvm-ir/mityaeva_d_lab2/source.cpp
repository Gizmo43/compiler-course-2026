#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

class FmuladdToMulAdd final : public PassInfoMixin<FmuladdToMulAdd> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool changed = false;
    SmallVector<IntrinsicInst *, 8> worklist;

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (auto *II = dyn_cast<IntrinsicInst>(&I)) {
          if (II->getIntrinsicID() == Intrinsic::fmuladd) {
            worklist.push_back(II);
          }
        }
      }
    }

    for (IntrinsicInst *fmuladdCall : worklist) {
      Value *opA = fmuladdCall->getArgOperand(0);
      Value *opB = fmuladdCall->getArgOperand(1);
      Value *opC = fmuladdCall->getArgOperand(2);
      FastMathFlags fmf = fmuladdCall->getFastMathFlags();

      IRBuilder<> builder(fmuladdCall);
      builder.setFastMathFlags(fmf);

      Value *mul = builder.CreateFMul(opA, opB, "fmuladd.mul");
      Value *add = builder.CreateFAdd(mul, opC, "fmuladd.add");

      fmuladdCall->replaceAllUsesWith(add);
      fmuladdCall->eraseFromParent();
      changed = true;
    }

    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FmuladdToMulAdd", "1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (name == "replace-fmuladd") {
                    FPM.addPass(FmuladdToMulAdd{});
                    return true;
                  }
                  return false;
                });
          }};
}