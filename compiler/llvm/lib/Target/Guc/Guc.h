//==-- Guc.h - Top-level interface for Guc representation --*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Guc backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_NVFUC_H
#define LLVM_TARGET_NVFUC_H

#include "llvm/Target/TargetMachine.h"

namespace GucCC {
  // Guc specific condition code.
  enum CondCodes {
    COND_E  = 0,  // aka COND_Z
    COND_NE = 1,  // aka COND_NZ
    COND_HS = 2,  // aka COND_C
    COND_LO = 3,  // aka COND_NC
    COND_GE = 4,
    COND_L  = 5,

    COND_INVALID = -1
  };
}

namespace llvm {
  class GucTargetMachine;
  class FunctionPass;
  class formatted_raw_ostream;

  FunctionPass *createGucISelDag(GucTargetMachine &TM,
                                    CodeGenOpt::Level OptLevel);

  FunctionPass *createGucBranchSelectionPass();

  extern Target TheGucTarget;

} // end namespace llvm;

// Defines symbolic names for Guc registers.
// This defines a mapping from register name to register number.
#include "GucGenRegisterNames.inc"

// Defines symbolic names for the Guc instructions.
#include "GucGenInstrNames.inc"

#endif
