//===- GucInstrInfo.td - Guc Instruction defs -----------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Guc instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "GucInstrFormats.td"

//===----------------------------------------------------------------------===//
// Type Profiles.
//===----------------------------------------------------------------------===//
// 
// Note: SDTypeProfile<int numresults, 
//                     int numoperands,
//                     list<SDTypeConstraint> constraints>;
//
def SDT_NVFUC_Call         : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_NVFUC_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_NVFUC_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_NVFUC_Wrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                                                  SDTCisPtrTy<0>]>;
def SDT_NVFUC_Cmp          : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def SDT_NVFUC_BrCC         : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>,
                                                  SDTCisVT<1, i16>]>;
def SDT_NVFUC_SelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>,
                                                  SDTCisSameAs<1, 2>, 
                                                  SDTCisVT<3, i16>]>;
//def SDT_NVFUC_XbitFlag     : SDTypeProfile<1, 1, [SDTCisVT<0, i32>]>;
def SDT_NVFUC_XbitFlag     : SDTypeProfile<1, 2, [SDTCisSameAs<0, 1>,
                                                  SDTCisVT<2, i32>]>;
def SDT_NVFUC_Xbit         : SDTypeProfile<1, 2, [SDTCisSameAs<0, 1>,
                                                  SDTCisVT<2, i32>]>;

//===----------------------------------------------------------------------===//
// Guc Specific Node Definitions.
//===----------------------------------------------------------------------===//
def Gucretflag  : SDNode<"GucISD::RET_FLAG", SDTNone,
                       [SDNPHasChain, SDNPOptInFlag]>;
def Gucretiflag : SDNode<"GucISD::RETI_FLAG", SDTNone,
                       [SDNPHasChain, SDNPOptInFlag]>;

def Guccall    : SDNode<"GucISD::CALL", SDT_NVFUC_Call,
                     [SDNPHasChain, SDNPOutFlag, SDNPOptInFlag, SDNPVariadic]>;
def Guccallseq_start :
                 SDNode<"ISD::CALLSEQ_START", SDT_NVFUC_CallSeqStart,
                        [SDNPHasChain, SDNPOutFlag]>;
def Guccallseq_end :
                 SDNode<"ISD::CALLSEQ_END",   SDT_NVFUC_CallSeqEnd,
                        [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;
def GucWrapper : SDNode<"GucISD::Wrapper", SDT_NVFUC_Wrapper>;
def Guccmp     : SDNode<"GucISD::CMP", SDT_NVFUC_Cmp, [SDNPOutFlag]>;
def Gucbrcc    : SDNode<"GucISD::BR_CC", SDT_NVFUC_BrCC,
                            [SDNPHasChain, SDNPInFlag]>;
def Gucselectcc: SDNode<"GucISD::SELECT_CC", SDT_NVFUC_SelectCC,
                            [SDNPInFlag]>;
def Gucxbitflag: SDNode<"GucISD::XBIT_FLAG", SDT_NVFUC_XbitFlag,
                           [SDNPInFlag]>;
def Gucxbit: SDNode<"GucISD::XBIT", SDT_NVFUC_Xbit, [SDNPInFlag]>;

//==-----------------------------------------------------------------------===//
// ZERO, LO16, HI16
def ZERO : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(0, MVT::i32);
}]>;

def I1 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0x00000001,
                                   MVT::i32);
}]>;

def LO16 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0x0000ffff,
                                   MVT::i32);
}]>;

def SExtLO16 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getSExtValue() & 0x0000ffff,
                                   MVT::i32);
}]>;

def HI16 : SDNodeXForm<imm, [{
  // Transformation function: shift the immediate value down into the low bits.
  return CurDAG->getTargetConstant(((unsigned)N->getZExtValue() & 0xffff0000), 
                                   MVT::i32);
}]>;

def ZEROimm : PatLeaf<(imm), [{
  return (0 == (unsigned)N->getZExtValue());
}], LO16>;

def LO16imm : PatLeaf<(imm), [{
  return (((unsigned)N->getZExtValue()) & 0x0000ffff) ==
         (unsigned)N->getZExtValue();
}], LO16>;

def HI16imm : PatLeaf<(imm), [{
  return (((unsigned)N->getZExtValue() >> 16) << 16) ==
         (unsigned)N->getZExtValue();
}], HI16>;


// Node immediate fits as 16-bit sign extended on target immediate.
def SExtLO16imm : PatLeaf<(imm), [{ 
  return isInt<16>(N->getSExtValue()); 
}], SExtLO16>;

//===----------------------------------------------------------------------===//
// Guc Operand Definitions.
//===----------------------------------------------------------------------===//

def sext32imm : Operand<i32> {
  let PrintMethod = "printSExtOperand";
}

def sext16imm : Operand<i16> {
  let PrintMethod = "printSExtOperand";
}

def flagimm : Operand<i32> {
  let PrintMethod = "printFlagsOperand";
}

// Address operands
def memsrc : Operand<i32> {
  let PrintMethod = "printSrcMemOperand";
  let MIOperandInfo = (ops GR32, i32imm);
}

def memdst : Operand<i32> {
  let PrintMethod = "printSrcMemOperand";
  let MIOperandInfo = (ops GR32, i32imm);
}

// Short jump targets have OtherVT type and are printed as pcrel imm values.
def jmptarget : Operand<OtherVT> {
  let PrintMethod = "printPCRelImmOperand";
}

// Operand for printing out a condition code.
def cc : Operand<i16> {
  let PrintMethod = "printCCOperand";
}

//===----------------------------------------------------------------------===//
// Guc Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Pattern Fragments
def zextloadi32i16 : PatFrag<(ops node:$ptr), (i32 (zextloadi16 node:$ptr))>;
def  extloadi32i16 : PatFrag<(ops node:$ptr), (i32 ( extloadi16 node:$ptr))>;
def and_su : PatFrag<(ops node:$lhs, node:$rhs), (and node:$lhs, node:$rhs), [{
  return N->hasOneUse();
}]>;
//===----------------------------------------------------------------------===//
// Instruction list..

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber FLG.
let Defs = [SP, FLG], Uses = [SP] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),
                              "#ADJCALLSTACKDOWN",
                              [(Guccallseq_start timm:$amt)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              "#ADJCALLSTACKUP",
                              [(Guccallseq_end timm:$amt1, timm:$amt2)]>;
}

let usesCustomInserter = 1 in {
  def Select : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2, i16imm:$cc),
                      "// Select PSEUDO",
                      [(set GR32:$dst,
                       (Gucselectcc GR32:$src, GR32:$src2, imm:$cc))]>;
}

let neverHasSideEffects = 1 in
def NOP : Pseudo<(outs), (ins), "nop", []>;

//===----------------------------------------------------------------------===//
//  Control Flow Instructions...
//

// FIXME: Provide proper encoding!
let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET  : Pseudo<(outs), (ins), "ret",  [(Gucretflag)]>;
  def RETI : Pseudo<(outs), (ins), "iret", [(Gucretiflag)]>;
}

let isBranch = 1, isTerminator = 1 in {

// FIXME: expand opcode & cond field for branches!
// Jump instructions are treated in llvm2envyas.

// Direct branch
let isBarrier = 1 in {
  // Short branch
  def JMP : Pseudo<(outs), (ins jmptarget:$dst),
                   "bra\t#$dst",
                   [(br bb:$dst)]>;
let isIndirectBranch = 1 in {
    // Long branches
    def Bi  : Pseudo<(outs), (ins i32imm:$brdst),
                    "bra\t#$brdst",
                    [(brind tblockaddress:$brdst)]>;
  }
}

// Conditional branches
let Uses = [FLG] in
  def JCC : Pseudo<(outs), (ins jmptarget:$dst, cc:$cc),
                   "bra $cc\t#$dst",
                   [(Gucbrcc bb:$dst, imm:$cc)]>;
} // isBranch, isTerminator

//===----------------------------------------------------------------------===//
//  Call Instructions...
//
let isCall = 1 in
  // All calls clobber the non-callee saved registers. SP is marked as
  // a use to prevent stack-pointer assignments that appear immediately
  // before calls from potentially appearing dead. Uses for argument
  // registers are added manually.
  let Defs = [R12, R13, R14, R15, FLG],
      Uses = [SP] in {
    def CALLi     : Pseudo<(outs), (ins i32imm:$dst, variable_ops),
                           "call\t#$dst", [(Guccall imm:$dst)]>;
    def CALLr     : Pseudo<(outs), (ins GR32:$dst, variable_ops),
                           "call\t#$dst", [(Guccall GR32:$dst)]>;
  }


//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions...
//
let Defs = [SP], Uses = [SP], neverHasSideEffects=1 in {
let mayLoad = 1 in
def POPr   : Pseudo<(outs GR32:$reg), (ins), "pop\t$reg", []>;

let mayStore = 1 in
def PUSHr  : Pseudo<(outs), (ins GR32:$reg), "push\t$reg",[]>;
}

//===----------------------------------------------------------------------===//
// Move Instructions

// FIXME: Provide proper encoding!
let neverHasSideEffects = 1 in {
def MOVrr : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                   "sub b32\t{$dst $src} 0 // mov $dst $src",
                   []>;
def MOVrs : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                   "mov\t{$dst $src}",
                   []>;
}

def MOVri: Pseudo<(outs GR32:$dst), (ins sext32imm:$src),
                  "mov\t{$dst $src}",
                  []>;
/*fix*/
def MOV16ri: Pseudo<(outs GR32:$dst), (ins sext16imm:$src),
                    "clear b32 {$dst}\n\tmov\t{$dst $src}",
                    [(set GR32:$dst, SExtLO16imm:$src)]>;

def MOVrz: Pseudo<(outs GR32:$dst), (ins i32imm:$src),
                  "clear b32 {$dst}",
                  [(set GR32:$dst, ZEROimm:$src)]>;

let canFoldAsLoad = 1, isReMaterializable = 1 in {
def MOVrm : Pseudo<(outs GR32:$dst), (ins memsrc:$src),
                   "ld b32\t{$dst $src}",
                   [(set GR32:$dst, (load addr:$src))]>;
}

def MOVZXrm : Pseudo<(outs GR32:$dst), (ins memsrc:$src),
                     "ld b32\t{$dst $src}",
                     [(set GR32:$dst, (zextloadi32i16 addr:$src))]>;

def MOVmr : Pseudo<(outs), (ins memdst:$dst, GR32:$src),
                   "st b32\t{$dst $src}",
                   [(store GR32:$src, addr:$dst)]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, Constraints = "$base = $base_wb" in {
def MOVrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb), (ins GR32:$base),
                        "lb b32\t{$dst @$base+}", []>;
}

//===----------------------------------------------------------------------===//
// Arithmetic Instructions

let Constraints = "$src = $dst" in {

let Defs = [FLG] in {

let isCommutable = 1 in { // X = ADD Y, Z  == X = ADD Z, Y
def ADDrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "add b32\t{$dst $src2}",
                   [(set GR32:$dst, (add GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;
}

def ADDri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "add b32\t{$dst $src2}",
                   [(set GR32:$dst, (add GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

// For stack pointer
def ADDsi : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "add\t{$dst $src2}",
                   []>;

let Uses = [FLG] in {

let isCommutable = 1 in { // X = ADDC Y, Z  == X = ADDC Z, Y
def ADCrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "adc b32\t{$dst $src2}",
                   [(set GR32:$dst, (adde GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;
} // isCommutable

def ADCri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "addc b32\t{$dst $src2}",
                   [(set GR32:$dst, (adde GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

} // Uses = [FLG]

let isCommutable = 1 in { // X = AND Y, Z  == X = AND Z, Y
def ANDrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "and\t{$dst $src2}",
                   [(set GR32:$dst, (and GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;
}

def ANDri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "and\t{$dst $src2}",
                   [(set GR32:$dst, (and GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

let isCommutable = 1 in { // X = OR Y, Z  == X = OR Z, Y
def ORrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                  "or\t{$dst $src2}",
                  [(set GR32:$dst, (or GR32:$src, GR32:$src2))]>;
}

def ORri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                  "or\t{$dst $src2}",
                  [(set GR32:$dst, (or GR32:$src, (LO16imm:$src2)))]>;

/*
// bic does not modify condition codes
def BIC16rr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "bic.w\t{$src2, $dst}",
                    [(set GR32:$dst, (and GR32:$src, (not GR32:$src2)))]>;
*/
let isCommutable = 1 in { // X = XOR Y, Z  == X = XOR Z, Y
def XORrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "xor\t{$dst $src2}",
                   [(set GR32:$dst, (xor GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;
}

def XORri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "xor\t{$dst $src2}",
                   [(set GR32:$dst, (xor GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

def SUBrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "sub b32\t{$dst $src2}",
                   [(set GR32:$dst, (sub GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;

def SUBri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "sub b32\t{$dst $src2}",
                   [(set GR32:$dst, (sub GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

def SUBsi : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "add\t{$dst -$src2}",
                   []>;

let Uses = [FLG] in {
def SBCrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "subc b32\t{$dst $src2}",
                   [(set GR32:$dst, (sube GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;

def SBCri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "subc b32\t{$dst $src2}",
                   [(set GR32:$dst, (sube GR32:$src, (LO16imm:$src2))),
                    (implicit FLG)]>;

} // Uses = [FLG]

let isCommutable = 1 in { // X = SHL Y, Z  == X = SHL Z, Y
// FIXME: I don't know how to distinguish mulu and muls in the "rr" form!
def MULSrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "muls\t{$dst $src2}",
                    [(set GR32:$dst, (mul GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;
}

def MULUri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "mulu\t{$dst $src2}",
                    [(set GR32:$dst, (mul GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

def MULSri : Pseudo<(outs GR32:$dst), (ins GR32:$src, sext16imm:$src2),
                    "muls\t{$dst $src2}",
                    [(set GR32:$dst, (mul GR32:$src, SExtLO16imm:$src2)),
                     (implicit FLG)]>;

def SHLrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "shl b32\t{$dst $src2}",
                   [(set GR32:$dst, (shl GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;

def SHLri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "shl b32\t{$dst $src2}",
                   [(set GR32:$dst, (shl GR32:$src, LO16imm:$src2)),
                    (implicit FLG)]>;

def SHRrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "shr b32\t{$dst $src2}",
                   [(set GR32:$dst, (srl GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;

def SHRri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "shr b32\t{$dst $src2}",
                    [(set GR32:$dst, (srl GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

def SARrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "sar b32\t{$dst $src2}",
                   [(set GR32:$dst, (sra GR32:$src, GR32:$src2)),
                    (implicit FLG)]>;

def SARri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "sar b32\t{$dst $src2}",
                   [(set GR32:$dst, (sra GR32:$src, LO16imm:$src2)),
                    (implicit FLG)]>;
} // Defs = [FLG]

// set high 16 bits
def SETHIri: Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                     "sethi\t{$dst $src2}",
                     []>;

} // Constraints = "$src = $dst"

//let Constraints = "$src != $dst" in {

let Defs = [FLG] in {

let isCommutable = 1 in { // X = ADD Y, Z  == X = ADD Z, Y
def ADDrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "add b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (add GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;
}

def ADDrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "add b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (add GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, 
Constraints = "$base = $base_wb, $src = $dst" in {
def ADDrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb),
                        (ins GR32:$src, GR32:$base),
                        "add b32\t{@$base+, $dst}", []>;
}

let Uses = [FLG] in {

let isCommutable = 1 in { // X = ADDC Y, Z  == X = ADDC Z, Y
def ADCrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "adc b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (adde GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;
} // isCommutable

def ADCrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "addc b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (adde GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

} // Uses = [FLG]

let isCommutable = 1 in { // X = AND Y, Z  == X = AND Z, Y
def ANDrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "and\t{$dst $src $src2}",
                    [(set GR32:$dst, (and GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;
}

def ANDrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "and\t{$dst $src $src2}",
                    [(set GR32:$dst, (and GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, 
Constraints = "$base = $base_wb, $src = $dst" in {
def ANDrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb),
                        (ins GR32:$src, GR32:$base),
                        "and\t{@$base+, $dst}", []>;
}

let isCommutable = 1 in { // X = OR Y, Z  == X = OR Z, Y
def ORrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                   "or\t{$dst $src $src2}",
                   [(set GR32:$dst, (or GR32:$src, GR32:$src2))]>;
}

def ORrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                   "or\t{$dst $src $src2}",
                   [(set GR32:$dst, (or GR32:$src, LO16imm:$src2))]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, 
Constraints = "$base = $base_wb, $src = $dst" in {
def ORrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb),
                       (ins GR32:$src, GR32:$base),
                       "or\t{@$base+, $dst}", []>;
}

let isCommutable = 1 in { // X = XOR Y, Z  == X = XOR Z, Y
def XORrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "xor\t{$dst $src $src2}",
                    [(set GR32:$dst, (xor GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;
}

def XORrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "xor\t{$dst $src $src2}",
                    [(set GR32:$dst, (xor GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, 
Constraints = "$base = $base_wb, $src = $dst" in {
def XORrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb),
                        (ins GR32:$src, GR32:$base),
                        "xor\t{@$base+, $dst}", []>;
}

def SUBrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "sub b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sub GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;

def SUBrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "sub b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sub GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

let mayLoad = 1, hasExtraDefRegAllocReq = 1, 
Constraints = "$base = $base_wb, $src = $dst" in {
def SUBrm_POST : Pseudo<(outs GR32:$dst, GR32:$base_wb),
                        (ins GR32:$src, GR32:$base),
                        "sub b32\t{@$base+, $dst}", []>;
}

let Uses = [FLG] in {
def SBCrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "subc b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sube GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;

def SBCrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "subc b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sube GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

} // Uses = [FLG]

let isCommutable = 1 in { // X = SHL Y, Z  == X = SHL Z, Y
// I don't know how to distinguish mulu and muls in rrr form...
def MULSrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                     "muls\t{$dst $src $src2}",
                     [(set GR32:$dst, (mul GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;
}

def MULUrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                     "mulu\t{$dst $src $src2}",
                     [(set GR32:$dst, (mul GR32:$src, LO16imm:$src2)),
                      (implicit FLG)]>;

def MULSrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, sext16imm:$src2),
                     "muls\t{$dst $src $src2}",
                     [(set GR32:$dst, (mul GR32:$src, SExtLO16imm:$src2)),
                      (implicit FLG)]>;

def SHLrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "shl b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (shl GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;

def SHLrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "shl b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (shl GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

def SHRrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "shr b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (srl GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;

def SHRrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "shr b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (srl GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

def SARrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                    "sar b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sra GR32:$src, GR32:$src2)),
                     (implicit FLG)]>;

def SARrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                    "sar b32\t{$dst $src $src2}",
                    [(set GR32:$dst, (sra GR32:$src, LO16imm:$src2)),
                     (implicit FLG)]>;

} // Defs = [FLG]

let isCommutable = 1 in { // X = SHL Y, Z  == X = SHL Z, Y
def DIVUrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                     "div\t{$dst $src $src2}",
                     [(set GR32:$dst, (udiv GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;
def DIVSrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                     "(signed) div\t{$dst $src $src2}",
                     [(set GR32:$dst, (sdiv GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;
}

def DIVUrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                     "div\t{$dst $src $src2}",
                     [(set GR32:$dst, (udiv GR32:$src, LO16imm:$src2)),
                      (implicit FLG)]>;

def DIVSrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, sext16imm:$src2),
                     "(signed) div\t{$dst $src $src2}",
                     [(set GR32:$dst, (sdiv GR32:$src, SExtLO16imm:$src2)),
                      (implicit FLG)]>;

let isCommutable = 1 in { // X = SHL Y, Z  == X = SHL Z, Y
def REMUrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                     "mod\t{$dst $src $src2}",
                     [(set GR32:$dst, (urem GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;

def REMSrrr : Pseudo<(outs GR32:$dst), (ins GR32:$src, GR32:$src2),
                     "(signed) mod\t{$dst $src $src2}",
                     [(set GR32:$dst, (urem GR32:$src, GR32:$src2)),
                      (implicit FLG)]>;
}

def REMUrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$src2),
                     "mod\t{$dst $src $src2}",
                     [(set GR32:$dst, (urem GR32:$src, LO16imm:$src2)),
                      (implicit FLG)]>;

def REMSrri : Pseudo<(outs GR32:$dst), (ins GR32:$src, sext16imm:$src2),
                     "(signed)mod\t{$dst $src $src2}",
                     [(set GR32:$dst, (urem GR32:$src, SExtLO16imm:$src2)),
                      (implicit FLG)]>;

//} // Constraints = "$src != $dst"

// Sign extension
let Defs = [FLG] in {
def SEXT8rr : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                     "sext\t{$dst $src}",
                     [(set GR32:$dst, (sext_inreg GR32:$src, i8)),
                      (implicit FLG)]>;

def SEXT16rr : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                      "sext\t{$dst $src}",
                      [(set GR32:$dst, (sext_inreg GR32:$src, i16)),
                       (implicit FLG)]>;
}

// Integer comparisons
let Defs = [FLG] in {
def CMPrr : Pseudo<(outs), (ins GR32:$src, GR32:$src2),
                   "cmp b32\t{$src $src2}",
                   [(Guccmp GR32:$src, GR32:$src2), (implicit FLG)]>;

def CMPri : Pseudo<(outs), (ins GR32:$src, sext16imm:$src2),
                   "cmp b32\t{$src $src2}",
                   [(Guccmp GR32:$src, SExtLO16imm:$src2), (implicit FLG)]>;

def XBITflag : Pseudo<(outs GR32:$dst), (ins GR32:$src, flagimm:$src2),
                      "xbit\t{$dst $src2}",
                      [(set GR32:$dst, (Gucxbitflag GR32:$src, imm:$src2)),
                       (implicit FLG)]>;

def XBITri : Pseudo<(outs GR32:$dst), (ins GR32:$src, i32imm:$shamt),
                    "xbit\t{$dst $src $shamt}",
                    [(set GR32:$dst, (Gucxbit GR32:$src, imm:$shamt)),
                     (implicit FLG)]>;

} // Defs = [FLG]

// Not
def NOTrr : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                   "not b32\t$dst $src",
                   [(set GR32:$dst, (not GR32:$src))]>;

// Negate
def NEGrr : Pseudo<(outs GR32:$dst), (ins GR32:$src),
                   "neg b32\t$dst $src",
                   [(set GR32:$dst, (ineg GR32:$src))]>;

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns

// extload
def : Pat<(extloadi32i16 addr:$src), (MOVZXrm addr:$src)>;

// Arbitrary immediates.
def : Pat<(i32 imm:$val),
          (SETHIri (MOVri (LO16 imm:$val)), (HI16 imm:$val))>;

// GlobalAddress, ExternalSymbol, etc.
def : Pat<(i32 (GucWrapper tglobaladdr:$dst)), 
          (SETHIri (MOVri tglobaladdr:$dst), tglobaladdr:$dst)>;
def : Pat<(i32 (GucWrapper texternalsym:$dst)), 
          (SETHIri (MOVri texternalsym:$dst), texternalsym:$dst)>;
def : Pat<(i32 (GucWrapper tblockaddress:$dst)),
          (SETHIri (MOVri tblockaddress:$dst), tblockaddress:$dst)>;

def : Pat<(add GR32:$src, (GucWrapper tglobaladdr:$src2)), 
          (SETHIri (ADDri GR32:$src, tglobaladdr:$src2), tglobaladdr:$src2)>;
def : Pat<(add GR32:$src, (GucWrapper texternalsym:$src2)),
          (SETHIri (ADDri GR32:$src, texternalsym:$src2), tglobaladdr:$src2)>;
def : Pat<(add GR32:$src, (GucWrapper tblockaddress:$src2)),
          (SETHIri (ADDri GR32:$src, tblockaddress:$src2), tglobaladdr:$src2)>;

// calls
def : Pat<(Guccall (i32 tglobaladdr:$dst)),
          (CALLi tglobaladdr:$dst)>;
def : Pat<(Guccall (i32 texternalsym:$dst)),
          (CALLi texternalsym:$dst)>;

// add and sub always produce carry
def : Pat<(addc GR32:$src, GR32:$src2),
          (ADDrr GR32:$src, GR32:$src2)>;
def : Pat<(addc GR32:$src, imm:$src2),
          (ADDri GR32:$src, imm:$src2)>;

def : Pat<(subc GR32:$src, GR32:$src2),
          (SUBrr GR32:$src, GR32:$src2)>;
def : Pat<(subc GR32:$src, imm:$src2),
          (SUBri GR32:$src, imm:$src2)>;

// zextload bool -> zextload byte
def : Pat<(i32 (zextloadi1 GR32:$src)), (MOVrr GR32:$src)>;
def : Pat<(i32 (zextloadi1 imm:$src)), (MOVri imm:$src)>;

// peephole patterns


