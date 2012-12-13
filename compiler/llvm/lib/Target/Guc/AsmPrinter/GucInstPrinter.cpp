//===-- GucInstPrinter.cpp - Convert Guc MCInst to assembly syntax --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Guc MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "Guc.h"
#include "GucInstrInfo.h"
#include "GucInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include <stdio.h>
using namespace llvm;

// Include the auto-generated portion of the assembly writer.
#define MachineInstr MCInst
#include "GucGenAsmWriter.inc"
#undef MachineInstr

void GucInstPrinter::printInst(const MCInst *MI, raw_ostream &O) {
  printInstruction(MI, O);
}

void GucInstPrinter::printPCRelImmOperand(const MCInst *MI, unsigned OpNo,
                                             raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << Op.getImm();
  else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    O << *Op.getExpr();
  }
}

void GucInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    // print in hex for envyas.
    char hex[32];
    unsigned int n = Op.getImm();
    if (n > 0xffff) { /* in case of 32 bits. */
      sprintf(hex, "0x%x", n & 0xffff0000);
    }
    else {
      sprintf(hex, "0x%x", n & 0x0000ffff);
    }
    O << hex;
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << '#' << *Op.getExpr();
  }
}

void GucInstPrinter::printSExtOperand(const MCInst *MI, unsigned OpNo,
					raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    // print in hex for envyas.
    char hex[32];
    unsigned int n = Op.getImm();
    if (n < 0x8000) {
      sprintf(hex, "0x%x", n & 0xffff);
    }
    else {
      sprintf(hex, "-0x%x", ((0x8000 + 0x8000) - n) & 0xffff);
    }
    O << hex;
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << '#' << *Op.getExpr();
  }
}

void GucInstPrinter::printFlagsOperand(const MCInst *MI, unsigned OpNo,
				         raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  // print in hex for envyas.
  char operands[32];
  unsigned int n = Op.getImm();
  switch (Op.getImm()) {
      case 8:
	  sprintf(operands, "$flags c");
	  break;
      case 9:
	  sprintf(operands, "$flags o");
	  break;
      case 0xa:
	  sprintf(operands, "$flags s");
	  break;
      case 0xb:
	  sprintf(operands, "$flags z");
	  break;
  }
  O << operands;
}

void GucInstPrinter::printSrcMemOperand(const MCInst *MI, unsigned OpNo,
                                           raw_ostream &O,
                                           const char *Modifier) {
  const MCOperand &Base = MI->getOperand(OpNo);
  const MCOperand &Disp = MI->getOperand(OpNo+1);

  // Print displacement first

  // If the global address expression is a part of displacement field with a
  // register base, we should not emit any prefix symbol here, e.g.
  //   mov.w &foo, r1
  // vs
  //   mov.w glb(r1), r2
  // Otherwise (!) guc-as will silently miscompile the output :(
  if (!Base.getReg())
    O << '&';

  if (Disp.isExpr()) {
      O << *Disp.getExpr();
      if (Base.getReg())
	  O << '(' << getRegisterName(Base.getReg()) << ')';
  }
  else {
      // print in hex for envyas.
      char hex[32];
      unsigned int n = Disp.getImm();
      assert(Disp.isImm() && "Expected immediate in displacement field");
      if (n < 0x8000) {
	  sprintf(hex, "+0x%x", n & 0xffff);
      }
      else {
	  sprintf(hex, "-0x%x", ((0x8000 + 0x8000) - n) & 0xffff);
      }
      if (Base.getReg()) {
	  O << "D[" << getRegisterName(Base.getReg()) << hex << ']';
      }
  }
}

void GucInstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O) {
  unsigned CC = MI->getOperand(OpNo).getImm();

  switch (CC) {
  default:
   llvm_unreachable("Unsupported CC code");
   break;
  case GucCC::COND_E:
   O << "e";
   break;
  case GucCC::COND_NE:
   O << "ne";
   break;
  case GucCC::COND_HS:
   O << "c";
   break;
  case GucCC::COND_LO:
   O << "nc";
   break;
  case GucCC::COND_GE:
   O << "ge";
   break;
  case GucCC::COND_L:
   O << 'l';
   break;
  }
}
