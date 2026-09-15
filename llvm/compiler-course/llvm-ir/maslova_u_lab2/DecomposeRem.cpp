#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct DecomposeRemPass : public PassInfoMixin<DecomposeRemPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    bool Changed = false;
    std::vector<Instruction *> ToRemove;

    for (auto &BB : F) {
      for (auto &I : BB) {
        unsigned Opcode = I.getOpcode();

        if (Opcode == Instruction::SRem || Opcode == Instruction::URem ||
            Opcode == Instruction::FRem) {
          IRBuilder<> Builder(&I);
          Value *LHS = I.getOperand(0);
          Value *RHS = I.getOperand(1);
          Value *Result = nullptr;

          if (Opcode == Instruction::SRem) {
            Value *Quot = Builder.CreateSDiv(LHS, RHS);
            Value *Prod = Builder.CreateMul(Quot, RHS);
            Result = Builder.CreateSub(LHS, Prod);
          } else if (Opcode == Instruction::URem) {
            Value *Quot = Builder.CreateUDiv(LHS, RHS);
            Value *Prod = Builder.CreateMul(Quot, RHS);
            Result = Builder.CreateSub(LHS, Prod);
          } else if (Opcode == Instruction::FRem) {
            Value *FDiv = Builder.CreateFDiv(LHS, RHS);
            Function *TruncFunc = Intrinsic::getDeclaration(
                F.getParent(), Intrinsic::trunc, FDiv->getType());
            Value *Trunc = Builder.CreateCall(TruncFunc, FDiv);
            Value *FMul = Builder.CreateFMul(Trunc, RHS);
            Result = Builder.CreateFSub(LHS, FMul);
          }

          if (Result) {
            I.replaceAllUsesWith(Result);
            ToRemove.push_back(&I);
            Changed = true;
          }
        }
      }
    }

    for (auto *I : ToRemove) {
      I->eraseFromParent();
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DecomposeRem", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "decompose-rem") {
                    FPM.addPass(DecomposeRemPass());
                    return true;
                  }
                  return false;
                });
          }};
}
