#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

namespace {

class FmaDecomposePass : public MachineFunctionPass {
public:
  static char ID;
  FmaDecomposePass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;
};

char FmaDecomposePass::ID = 0;

bool FmaDecomposePass::runOnMachineFunction(MachineFunction &MF) {
  const X86Subtarget &ST = MF.getSubtarget<X86Subtarget>();
  const X86InstrInfo *TII = ST.getInstrInfo();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  bool Changed = false;

  std::vector<MachineInstr *> toReplace;
  for (auto &MBB : MF) { // find all FMA instructions
    for (auto &MI : MBB) {
      unsigned Op = MI.getOpcode();
      if (Op == X86::VFMADD213SDr || Op == X86::VFMADD213SSr)
        toReplace.push_back(&MI);
    }
  }

  for (MachineInstr *FMA : toReplace) {
    // which mul/add opcode to use
    unsigned MulOpcode, AddOpcode;
    if (FMA->getOpcode() == X86::VFMADD213SDr) {
      MulOpcode = X86::VMULSDrr;
      AddOpcode = X86::VADDSDrr;
    } else {
      MulOpcode = X86::VMULSSrr;
      AddOpcode = X86::VADDSSrr;
    }

    // Dst = A * B + C
    Register Dst = FMA->getOperand(0).getReg();
    Register A = FMA->getOperand(1).getReg();
    Register B = FMA->getOperand(2).getReg();
    Register C = FMA->getOperand(3).getReg();

    MachineBasicBlock *MBB = FMA->getParent();
    DebugLoc DL = FMA->getDebugLoc();
    const TargetRegisterClass *RC = MRI.getRegClass(Dst);

    Register MulReg = MRI.createVirtualRegister(RC);
    Register AddReg = MRI.createVirtualRegister(RC);

    BuildMI(*MBB, FMA, DL, TII->get(MulOpcode), MulReg).addReg(A).addReg(B);
    BuildMI(*MBB, FMA, DL, TII->get(AddOpcode), AddReg)
        .addReg(MulReg)
        .addReg(C);

    MRI.replaceRegWith(Dst, AddReg);
    FMA->eraseFromParent();

    Changed = true;
  }

  return Changed;
}

} // namespace

static RegisterPass<FmaDecomposePass>
    X("fma-decompose-x86", "Decompose FMA instructions into mul+add", false,
      false);