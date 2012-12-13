//==-- GucTargetMachine.h - Define TargetMachine for Guc ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Guc specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//


#ifndef LLVM_TARGET_NVFUC_TARGETMACHINE_H
#define LLVM_TARGET_NVFUC_TARGETMACHINE_H

#include "GucInstrInfo.h"
#include "GucISelLowering.h"
#include "GucSelectionDAGInfo.h"
#include "GucRegisterInfo.h"
#include "GucSubtarget.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

/// GucTargetMachine
///
class GucTargetMachine : public LLVMTargetMachine {
  GucSubtarget        Subtarget;
  const TargetData       DataLayout;       // Calculates type size & alignment
  GucInstrInfo        InstrInfo;
  GucTargetLowering   TLInfo;
  GucSelectionDAGInfo TSInfo;

  // Guc does not have any call stack frame, therefore not having
  // any Guc specific FrameInfo class.
  TargetFrameInfo       FrameInfo;

public:
  GucTargetMachine(const Target &T, const std::string &TT,
                      const std::string &FS);

  virtual const TargetFrameInfo *getFrameInfo() const { return &FrameInfo; }
  virtual const GucInstrInfo *getInstrInfo() const  { return &InstrInfo; }
  virtual const TargetData *getTargetData() const     { return &DataLayout;}
  virtual const GucSubtarget *getSubtargetImpl() const { return &Subtarget; }

  virtual const TargetRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }

  virtual const GucTargetLowering *getTargetLowering() const {
    return &TLInfo;
  }

  virtual const GucSelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }

  virtual bool addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
  virtual bool addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
}; // GucTargetMachine.

} // end namespace llvm

#endif // LLVM_TARGET_NVFUC_TARGETMACHINE_H
