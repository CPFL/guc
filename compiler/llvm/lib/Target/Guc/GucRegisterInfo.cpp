//===- GucRegisterInfo.cpp - Guc Register Information ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Guc implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "guc-reg-info"

#include "Guc.h"
#include "GucMachineFunctionInfo.h"
#include "GucRegisterInfo.h"
#include "GucTargetMachine.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

// FIXME: Provide proper call frame setup / destroy opcodes.
GucRegisterInfo::GucRegisterInfo(GucTargetMachine &tm,
                                       const TargetInstrInfo &tii)
  : GucGenRegisterInfo(Guc::ADJCALLSTACKDOWN, Guc::ADJCALLSTACKUP),
    TM(tm), TII(tii) {
  StackAlign = TM.getFrameInfo()->getStackAlignment();
}

const unsigned*
GucRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  const Function* F = MF->getFunction();
  static const unsigned CalleeSavedRegs[] = {
    Guc::FP, Guc::R1, Guc::R2, Guc::R3,
    Guc::R4, Guc::R5, Guc::R6, Guc::R7,
    Guc::R8, Guc::R9, Guc::R10, Guc::R11,
    0
  };
#if 0
  static const unsigned CalleeSavedRegsIntr[] = {
    Guc::FP, Guc::R1, Guc::R2, Guc::R3,
    Guc::R4, Guc::R5, Guc::R6, Guc::R7,
    Guc::R8, Guc::R9, Guc::R10, Guc::R11,
    Guc::R12, Guc::R13, Guc::R14, Guc::R15,
    0
  };
  return (F->getCallingConv() == CallingConv::NVFUC_INTR ?
	  CalleeSavedRegsIntr : CalleeSavedRegs);
#endif
  F->getCallingConv();
  return CalleeSavedRegs;
}

BitVector GucRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Mark special registers as reserved.
  Reserved.set(Guc::FP);  
  Reserved.set(Guc::IV0);
  Reserved.set(Guc::IV1);
  Reserved.set(Guc::S2);
  Reserved.set(Guc::TV);
  Reserved.set(Guc::SP);
  Reserved.set(Guc::PC);
  Reserved.set(Guc::XCB);
  Reserved.set(Guc::XDB);
  Reserved.set(Guc::FLG);
  Reserved.set(Guc::CX);
  Reserved.set(Guc::CAU);
  Reserved.set(Guc::XTG);
  Reserved.set(Guc::TS);
  Reserved.set(Guc::S13);
  Reserved.set(Guc::S14);
  Reserved.set(Guc::S15);

  return Reserved;
}

const TargetRegisterClass *
GucRegisterInfo::getPointerRegClass(unsigned Kind) const {
  return &Guc::GR32RegClass;
}


bool GucRegisterInfo::hasFP(const MachineFunction &MF) const {
  return 0;
}

bool GucRegisterInfo::hasReservedCallFrame(const MachineFunction &MF) const {
  return !MF.getFrameInfo()->hasVarSizedObjects();
}

void GucRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  if (!hasReservedCallFrame(MF)) {
    // If the stack pointer can be changed after prologue, turn the
    // adjcallstackup instruction into a 'sub SP, <amt>' and the
    // adjcallstackdown instruction into 'add SP, <amt>'
    // TODO: consider using push / pop instead of sub + store / add
    MachineInstr *Old = I;
    uint64_t Amount = Old->getOperand(0).getImm();
    if (Amount != 0) {
      // We need to keep the stack aligned properly.  To do this, we round the
      // amount of space needed for the outgoing arguments up to the next
      // alignment boundary.
      Amount = (Amount+StackAlign-1)/StackAlign*StackAlign;

      MachineInstr *New = 0;
      if (Old->getOpcode() == getCallFrameSetupOpcode()) {
        New = BuildMI(MF, Old->getDebugLoc(),
                      TII.get(Guc::SUBsi), Guc::SP)
          .addReg(Guc::SP).addImm(Amount);
      } else {
        assert(Old->getOpcode() == getCallFrameDestroyOpcode());
        // factor out the amount the callee already popped.
        uint64_t CalleeAmt = Old->getOperand(1).getImm();
        Amount -= CalleeAmt;
        if (Amount)
          New = BuildMI(MF, Old->getDebugLoc(),
                        TII.get(Guc::ADDsi), Guc::SP)
            .addReg(Guc::SP).addImm(Amount);
      }

      if (New) {
        // The FLG implicit def is dead.
        New->getOperand(3).setIsDead();

        // Replace the pseudo instruction with a new instruction...
        MBB.insert(I, New);
      }
    }
  } else if (I->getOpcode() == getCallFrameDestroyOpcode()) {
    // If we are performing frame pointer elimination and if the callee pops
    // something off the stack pointer, add it back.
    if (uint64_t CalleeAmt = I->getOperand(1).getImm()) {
      MachineInstr *Old = I;
      MachineInstr *New =
        BuildMI(MF, Old->getDebugLoc(), TII.get(Guc::SUBsi),
                Guc::SP).addReg(Guc::SP).addImm(CalleeAmt);
      // The FLG implicit def is dead.
      New->getOperand(3).setIsDead();

      MBB.insert(I, New);
    }
  }

  MBB.erase(I);
}

void
GucRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                        int SPAdj, RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  unsigned i = 0;
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  DebugLoc dl = MI.getDebugLoc();
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();

  unsigned BasePtr = Guc::SP;
  int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);

  // Skip the saved PC
  Offset += 4;

  Offset += MF.getFrameInfo()->getStackSize();

  // Fold imm into offset
  Offset += MI.getOperand(i+1).getImm();

  if (MI.getOpcode() == Guc::ADDri) {
    // This is actually "load effective address" of the stack slot
    // instruction. We have only two-address instructions, thus we need to
    // expand it into mov + add

    MI.setDesc(TII.get(Guc::MOVrs));
    MI.getOperand(i).ChangeToRegister(BasePtr, false);

    if (Offset == 0)
      return;

    // We need to materialize the offset via add instruction.
    unsigned DstReg = MI.getOperand(0).getReg();
    if (Offset < 0)
      BuildMI(MBB, llvm::next(II), dl, TII.get(Guc::SUBri), DstReg)
        .addReg(DstReg).addImm(-Offset);
    else
      BuildMI(MBB, llvm::next(II), dl, TII.get(Guc::ADDri), DstReg)
        .addReg(DstReg).addImm(Offset);

    return;
  }

  MI.getOperand(i).ChangeToRegister(BasePtr, false);
  MI.getOperand(i+1).ChangeToImmediate(Offset);
}

void
GucRegisterInfo::processFunctionBeforeFrameFinalized(MachineFunction &MF)
                                                                         const {
}


void GucRegisterInfo::emitPrologue(MachineFunction &MF) const {
  MachineBasicBlock &MBB = MF.front();   // Prolog goes in entry BB
  MachineFrameInfo *MFI = MF.getFrameInfo();
  GucMachineFunctionInfo *GucFI = MF.getInfo<GucMachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Get the number of bytes to allocate from the FrameInfo.
  uint64_t StackSize = MFI->getStackSize();

  uint64_t NumBytes = StackSize - GucFI->getCalleeSavedFrameSize();

  // Skip the callee-saved push instructions.
  while (MBBI != MBB.end() && (MBBI->getOpcode() == Guc::PUSHr))
    ++MBBI;

  if (MBBI != MBB.end())
    DL = MBBI->getDebugLoc();

  if (NumBytes) { // adjust stack pointer: SP -= numbytes
    // If there is an SUBri of SP immediately before this instruction, merge
    // the two.
    //NumBytes -= mergeSPUpdates(MBB, MBBI, true);
    // If there is an ADDri or SUBri of SP immediately after this
    // instruction, merge the two instructions.
    // mergeSPUpdatesDown(MBB, MBBI, &NumBytes);

    if (NumBytes) {
      MachineInstr *MI =
        BuildMI(MBB, MBBI, DL, TII.get(Guc::SUBsi), Guc::SP)
	  .addReg(Guc::SP).addImm(NumBytes);
      // The FLG implicit def is dead.
      MI->getOperand(3).setIsDead();
    }
  }
}

void GucRegisterInfo::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  GucMachineFunctionInfo *GucFI = MF.getInfo<GucMachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = prior(MBB.end());
  unsigned RetOpcode = MBBI->getOpcode();
  DebugLoc DL = MBBI->getDebugLoc();

  switch (RetOpcode) {
  case Guc::RET:
  case Guc::RETI: break;  // These are ok
  default:
    llvm_unreachable("Can only insert epilog into returning blocks");
  }

  // Get the number of bytes to allocate from the FrameInfo
  uint64_t StackSize = MFI->getStackSize();
  unsigned CSSize = GucFI->getCalleeSavedFrameSize();
  uint64_t NumBytes = StackSize - CSSize;

  // Skip the callee-saved pop instructions.
  while (MBBI != MBB.begin()) {
    MachineBasicBlock::iterator PI = prior(MBBI);
    unsigned Opc = PI->getOpcode();
    if (Opc != Guc::POPr && !PI->getDesc().isTerminator())
      break;
    --MBBI;
  }

  DL = MBBI->getDebugLoc();

  // If there is an ADDri or SUBri of SP immediately before this
  // instruction, merge the two instructions.
  //if (NumBytes || MFI->hasVarSizedObjects())
  //  mergeSPUpdatesUp(MBB, MBBI, StackPtr, &NumBytes);

  if (MFI->hasVarSizedObjects()) {
    assert(1 && "Unexpected FP");
    BuildMI(MBB, MBBI, DL,
            TII.get(Guc::MOVrs), Guc::SP).addReg(Guc::FP);
    if (CSSize) {
      MachineInstr *MI =
        BuildMI(MBB, MBBI, DL,
                TII.get(Guc::SUBsi), Guc::SP)
        .addReg(Guc::SP).addImm(CSSize);
      // The FLG implicit def is dead.
      MI->getOperand(3).setIsDead();
    }
  } else {
    // adjust stack pointer back: SP += numbytes
    if (NumBytes) {
      MachineInstr *MI =
        BuildMI(MBB, MBBI, DL, TII.get(Guc::ADDsi), Guc::SP)
        .addReg(Guc::SP).addImm(NumBytes);
      // The FLG implicit def is dead.
      MI->getOperand(3).setIsDead();
    }
  }
}

unsigned GucRegisterInfo::getRARegister() const {
  return Guc::PC;
}

unsigned GucRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Guc::SP;
}

int GucRegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const {
  llvm_unreachable("Not implemented yet!");
  return 0;
}

#include "GucGenRegisterInfo.inc"
