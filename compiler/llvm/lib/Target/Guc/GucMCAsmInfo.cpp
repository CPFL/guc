//===-- GucMCAsmInfo.cpp - Guc asm properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the GucMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "GucMCAsmInfo.h"
using namespace llvm;

GucMCAsmInfo::GucMCAsmInfo(const Target &T, StringRef TT) {
  PrivateGlobalPrefix = ".L";
  WeakRefDirective ="\t.weak\t";
  PCSymbol=".";
  CommentString = "//";

  //AlignmentIsInBytes = false;
  AlignmentIsInBytes = true;
  AllowNameToStartWithDigit = true;
  UsesELFSectionDirectiveForBSS = true;
}
