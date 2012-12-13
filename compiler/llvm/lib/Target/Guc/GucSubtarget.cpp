//===- GucSubtarget.cpp - Guc Subtarget Information ---------*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Guc specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "GucSubtarget.h"
#include "Guc.h"
#include "GucGenSubtarget.inc"

using namespace llvm;

GucSubtarget::GucSubtarget(const std::string &TT, const std::string &FS) {
  std::string CPU = "generic";

  // Parse features string.
  ParseSubtargetFeatures(FS, CPU);
}
