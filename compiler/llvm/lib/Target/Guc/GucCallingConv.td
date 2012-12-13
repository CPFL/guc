//==- GucCallingConv.td - Calling Conventions for Guc -*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for Guc architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Guc Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_NVFUC : CallingConv<[
  // i32 are returned in registers R15, R14, R13, R12
  CCIfType<[i32], CCAssignToReg<[R15, R14, R13, R12]>>
]>;

//===----------------------------------------------------------------------===//
// Guc Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_NVFUC : CallingConv<[
  // Promote i16 arguments to i32.
  CCIfType<[i16], CCPromoteToType<i32>>,

  // The first 4 integer arguments of non-varargs functions are passed in
  // integer registers.
  CCIfNotVarArg<CCIfType<[i32], CCAssignToReg<[R15, R14, R13, R12]>>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;
