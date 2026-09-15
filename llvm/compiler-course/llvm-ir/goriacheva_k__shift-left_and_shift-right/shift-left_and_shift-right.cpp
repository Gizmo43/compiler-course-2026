#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct ShiftPass : llvm::PassInfoMixin<ShiftPass> {
  PreservedAnalyses run(Function &func, FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &bb : func) {
      for (auto &instr : make_early_inc_range(bb)) {

        auto *binOp = dyn_cast<BinaryOperator>(&instr);
        if (!binOp)
          continue;

        auto opcode = binOp->getOpcode();

        if (opcode != Instruction::Mul && opcode != Instruction::UDiv &&
            opcode != Instruction::SDiv)
          continue;

        Value *lhs = binOp->getOperand(0);
        Value *rhs = binOp->getOperand(1);

        ConstantInt *constInt = dyn_cast<ConstantInt>(rhs);
        Value *var = lhs;

        if (!constInt) {
          constInt = dyn_cast<ConstantInt>(lhs);
          var = rhs;
        }

        if (!constInt)
          continue;

        if (!constInt->getValue().isPowerOf2())
          continue;

        unsigned shift = constInt->getValue().logBase2();

        IRBuilder<> builder(binOp);
        Value *newInstr = nullptr;

        if (opcode == Instruction::Mul) {
          newInstr = builder.CreateShl(var, shift);
        } else if (opcode == Instruction::UDiv) {
          newInstr = builder.CreateLShr(var, shift);
        } else if (opcode == Instruction::SDiv) {
          Value *isNegative =
              builder.CreateICmpSLT(var, ConstantInt::get(var->getType(), 0));
          Value *bias = builder.CreateSelect(
              isNegative, ConstantInt::get(var->getType(), (1 << shift) - 1),
              ConstantInt::get(var->getType(), 0));
          Value *adjusted = builder.CreateAdd(var, bias);
          newInstr = builder.CreateAShr(adjusted, shift);
        }

        if (newInstr) {
          binOp->replaceAllUsesWith(newInstr);
          binOp->eraseFromParent();
          changed = true;
        }
      }
    }

    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ShiftPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "shift") {
                    FPM.addPass(ShiftPass{});
                    return true;
                  }
                  return false;
                });
          }};
}