#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace {

struct FmuladdReplacer {
  llvm::IntrinsicInst *Intrinsic;

  explicit FmuladdReplacer(llvm::IntrinsicInst *II) : Intrinsic(II) {}

  static bool matches(llvm::Instruction &instr) {
    auto *II = llvm::dyn_cast<llvm::IntrinsicInst>(&instr);
    return II && II->getIntrinsicID() == llvm::Intrinsic::fmuladd;
  }

  void replace() {
    llvm::Value *first_arg = Intrinsic->getArgOperand(0);
    llvm::Value *second_arg = Intrinsic->getArgOperand(1);
    llvm::Value *third_arg = Intrinsic->getArgOperand(2);
    llvm::IRBuilder<> builder(Intrinsic);
    builder.setFastMathFlags(Intrinsic->getFastMathFlags());
    llvm::Value *res_fm = builder.CreateFMul(first_arg, second_arg, "fmul");
    llvm::Value *res_fa = builder.CreateFAdd(res_fm, third_arg, "fadd");
    Intrinsic->replaceAllUsesWith(res_fa);
    Intrinsic->eraseFromParent();
  }
};
struct FmuladdScanner {
  llvm::SmallVector<llvm::IntrinsicInst *, 8> Found;

  void scan(llvm::Function &func) {
    for (llvm::BasicBlock &basic_block : func)
      for (llvm::Instruction &instr : basic_block)
        if (FmuladdReplacer::matches(instr))
          Found.push_back(llvm::cast<llvm::IntrinsicInst>(&instr));
  }

  bool empty() const { return Found.empty(); }

  void replaceAll() {
    for (auto *II : Found)
      FmuladdReplacer(II).replace();
  }
};

struct SannikovVerFmullAddDecPass
    : llvm::PassInfoMixin<SannikovVerFmullAddDecPass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    FmuladdScanner scanner;
    scanner.scan(func);
    if (scanner.empty())
      return llvm::PreservedAnalyses::all();
    scanner.replaceAll();
    return llvm::PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SannikovVerFmullAddDecPass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "sanverfmuladddec") {
                    FPM.addPass(SannikovVerFmullAddDecPass{});
                    return true;
                  }
                  return false;
                });
          }};
}