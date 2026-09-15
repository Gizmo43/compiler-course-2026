#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "lifanov-rem-decompose"

using namespace llvm;

namespace {

struct LifanovKRemDecompose : public PassInfoMixin<LifanovKRemDecompose> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    LLVM_DEBUG(dbgs() << "Analyzing function: '" << F.getName() << "'\n");

    for (auto &BB : F) {
      for (Instruction &I : make_early_inc_range(BB)) {
        if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
          if (handleBinOp(BinOp)) {
            Changed = true;
          }
        }
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }

private:
  bool handleBinOp(BinaryOperator *BinOp) {
    unsigned Opcode = BinOp->getOpcode();

    if (Opcode != Instruction::FRem && Opcode != Instruction::SRem &&
        Opcode != Instruction::URem) {
      return false;
    }

    IRBuilder<> Builder(BinOp);
    Value *LHS = BinOp->getOperand(0);
    Value *RHS = BinOp->getOperand(1);
    Value *Result = nullptr;

    switch (Opcode) {
    case Instruction::FRem: {
      Value *Div = Builder.CreateFDiv(LHS, RHS, "fdiv.tmp");
      Value *Trunc = Builder.CreateUnaryIntrinsic(Intrinsic::trunc, Div,
                                                  nullptr, "trunc.tmp");
      Value *Mul = Builder.CreateFMul(Trunc, RHS, "fmul.tmp");
      Result = Builder.CreateFSub(LHS, Mul, "fsub.tmp");
      break;
    }
    case Instruction::SRem: {
      Value *Div = Builder.CreateSDiv(LHS, RHS, "sdiv.tmp");
      Value *Mul = Builder.CreateMul(Div, RHS, "smul.tmp");
      Result = Builder.CreateSub(LHS, Mul, "ssub.tmp");
      break;
    }
    case Instruction::URem: {
      Value *Div = Builder.CreateUDiv(LHS, RHS, "udiv.tmp");
      Value *Mul = Builder.CreateMul(Div, RHS, "umul.tmp");
      Result = Builder.CreateSub(LHS, Mul, "usub.tmp");
      break;
    }
    }

    if (Result) {
      BinOp->replaceAllUsesWith(Result);
      BinOp->eraseFromParent();
      return true;
    }
    return false;
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LifanovKRemDecompose", "1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "lifanov_rem_decompose") {
                    FPM.addPass(LifanovKRemDecompose());
                    return true;
                  }
                  return false;
                });
          }};
}