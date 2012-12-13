//===-- GucTargetMachine.cpp - Define TargetMachine for Guc ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top-level implementation for the Guc target.
//
//===----------------------------------------------------------------------===//

#include "Guc.h"
#include "GucMCAsmInfo.h"
#include "GucTargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Target/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeGucTarget() {
  // Register the target.
  RegisterTargetMachine<GucTargetMachine> X(TheGucTarget);
  RegisterAsmInfo<GucMCAsmInfo> Z(TheGucTarget);
}

GucTargetMachine::GucTargetMachine(const Target &T,
                                         const std::string &TT,
                                         const std::string &FS) :
  LLVMTargetMachine(T, TT),
  Subtarget(TT, FS),
  // FIXME: Check TargetData string.
  //DataLayout("e-p:32:32:32-i32:16:16-i32:32:32-i32:32:64-n16:32"),
  DataLayout("e-p:32:32:32:32"),  
  InstrInfo(*this), TLInfo(*this), TSInfo(*this),
  //FrameInfo(TargetFrameInfo::StackGrowsDown, 4, -4) { }
  FrameInfo(TargetFrameInfo::StackGrowsDown, 4, -4) { }


bool GucTargetMachine::addInstSelector(PassManagerBase &PM,
                                          CodeGenOpt::Level OptLevel) {
  // Install an instruction selector.
  PM.add(createGucISelDag(*this, OptLevel));
  return false;
}

bool GucTargetMachine::addPreEmitPass(PassManagerBase &PM,
                                         CodeGenOpt::Level OptLevel) {
  // Must run branch selection immediately preceding the asm printer.
  PM.add(createGucBranchSelectionPass());
  return false;
}
