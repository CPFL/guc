//===-- GucTargetInfo.cpp - Guc Target Implementation ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Guc.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetRegistry.h"
using namespace llvm;

Target llvm::TheGucTarget;

extern "C" void LLVMInitializeGucTargetInfo() { 
  RegisterTarget<Triple::guc> 
    X(TheGucTarget, "guc", "Guc [experimental]");
}
