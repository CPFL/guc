//===-- GucSelectionDAGInfo.h - Guc SelectionDAG Info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Guc subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef GucSELECTIONDAGINFO_H
#define GucSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class GucTargetMachine;

class GucSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit GucSelectionDAGInfo(const GucTargetMachine &TM);
  ~GucSelectionDAGInfo();
};

}

#endif
