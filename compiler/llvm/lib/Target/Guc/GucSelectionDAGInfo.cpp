//===-- GucSelectionDAGInfo.cpp - Guc SelectionDAG Info -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the GucSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "guc-selectiondag-info"
#include "GucTargetMachine.h"
using namespace llvm;

GucSelectionDAGInfo::GucSelectionDAGInfo(const GucTargetMachine &TM)
  : TargetSelectionDAGInfo(TM) {
}

GucSelectionDAGInfo::~GucSelectionDAGInfo() {
}
