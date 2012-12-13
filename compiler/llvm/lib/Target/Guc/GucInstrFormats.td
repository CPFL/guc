//===- GucInstrFormats.td - Guc Instruction Formats-----*- tblgen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe Guc instructions format here
//

// Format specifies the encoding used by the instruction.  This is part of the
// ad-hoc solution used to emit machine instruction encodings by our machine
// code emitter.
class Format<bits<2> val> {
  bits<2> Value = val;
}

def PseudoFrm   : Format<0>;
def SingleOpFrm : Format<1>;
def DoubleOpFrm : Format<2>;
def CondJumpFrm : Format<3>;

class SourceMode<bits<2> val> {
  bits<2> Value = val;
}

def SrcReg      : SourceMode<0>;
def SrcMem      : SourceMode<1>;
def SrcIndReg   : SourceMode<2>;
def SrcPostInc  : SourceMode<3>;
def SrcImm      : SourceMode<3>;

class DestMode<bit val> {
  bit Value = val;
}

def DstReg      : DestMode<0>;
def DstMem      : DestMode<1>;

class SizeVal<bits<3> val> {
  bits<3> Value = val;
}

def SizeUnknown : SizeVal<0>; // Unknown / unset size
def SizeSpecial : SizeVal<1>; // Special instruction, e.g. pseudo
def Size2Bytes  : SizeVal<2>;
def Size4Bytes  : SizeVal<3>;
def Size6Bytes  : SizeVal<4>;

// Generic Guc Format
class GucInst<dag outs, dag ins, SizeVal sz, Format f, string asmstr> 
      : Instruction {
  field bits<32> Inst;

  let Namespace = "Guc";

  dag OutOperandList = outs;
  dag InOperandList  = ins;

  Format Form = f;
  SizeVal Sz = sz;

  // Define how we want to layout our TargetSpecific information field... This
  // should be kept up-to-date with the fields in the GucInstrInfo.h file.
  let TSFlags{1-0} = Form.Value;
  let TSFlags{4-2} = Sz.Value;

  let AsmString   = asmstr;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : GucInst<outs, ins, Size4Bytes, PseudoFrm, asmstr> {
  let Pattern = pattern;
  let Inst{31-0} = 0;
}
