#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {
class GusevDLab2Pass : public llvm::PassInfoMixin<GusevDLab2Pass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &Function,
                              llvm::FunctionAnalysisManager &) {
    llvm::SmallVector<llvm::BinaryOperator *, 8> Remainders;

    for (llvm::BasicBlock &Block : Function) {
      for (llvm::Instruction &Instruction : Block) {
        auto *BinaryOp = llvm::dyn_cast<llvm::BinaryOperator>(&Instruction);
        if (BinaryOp == nullptr) {
          continue;
        }

        switch (BinaryOp->getOpcode()) {
        case llvm::Instruction::SRem:
        case llvm::Instruction::URem:
        case llvm::Instruction::FRem:
          Remainders.push_back(BinaryOp);
          break;
        default:
          break;
        }
      }
    }

    if (Remainders.empty()) {
      return llvm::PreservedAnalyses::all();
    }

    for (llvm::BinaryOperator *Remainder : Remainders) {
      decomposeRemainder(*Remainder);
    }

    return llvm::PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }

private:
  static void decomposeRemainder(llvm::BinaryOperator &Remainder) {
    llvm::Value *Dividend = Remainder.getOperand(0);
    llvm::Value *Divisor = Remainder.getOperand(1);

    llvm::IRBuilder<> Builder(&Remainder);
    Builder.SetCurrentDebugLocation(Remainder.getDebugLoc());

    llvm::Value *Quotient = nullptr;
    llvm::Value *Product = nullptr;
    llvm::Value *Replacement = nullptr;

    switch (Remainder.getOpcode()) {
    case llvm::Instruction::SRem:
      Quotient = Builder.CreateSDiv(Dividend, Divisor, "rem.div");
      Product = Builder.CreateMul(Quotient, Divisor, "rem.mul");
      Replacement = Builder.CreateSub(Dividend, Product, "rem.sub");
      break;
    case llvm::Instruction::URem:
      Quotient = Builder.CreateUDiv(Dividend, Divisor, "rem.div");
      Product = Builder.CreateMul(Quotient, Divisor, "rem.mul");
      Replacement = Builder.CreateSub(Dividend, Product, "rem.sub");
      break;
    case llvm::Instruction::FRem: {
      Builder.setFastMathFlags(Remainder.getFastMathFlags());
      Quotient = Builder.CreateFDiv(Dividend, Divisor, "rem.div");
      llvm::Value *TruncatedQuotient = Builder.CreateUnaryIntrinsic(
          llvm::Intrinsic::trunc, Quotient, nullptr, "rem.trunc");
      Product = Builder.CreateFMul(TruncatedQuotient, Divisor, "rem.mul");
      Replacement = Builder.CreateFSub(Dividend, Product, "rem.sub");
      break;
    }
    default:
      return;
    }

    Remainder.replaceAllUsesWith(Replacement);
    Remainder.eraseFromParent();
  }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "GusevDLab2Pass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name != "gusev-d-lab2") {
                    return false;
                  }

                  FPM.addPass(GusevDLab2Pass{});
                  return true;
                });
          }};
}
