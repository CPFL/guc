//===- GucRegisterInfo.td - Guc Register defs ----------*- tblgen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Guc register file
//===----------------------------------------------------------------------===//

class GucReg<bits<5> num, string n> : Register<n> {
  field bits<5> Num = num;
  let Namespace = "Guc";
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

def FP  : GucReg<0,  "$r0">;
def R1  : GucReg<1,  "$r1">;
def R2  : GucReg<2,  "$r2">;
def R3  : GucReg<3,  "$r3">;
def R4  : GucReg<4,  "$r4">;
def R5  : GucReg<5,  "$r5">;
def R6  : GucReg<6,  "$r6">;
def R7  : GucReg<7,  "$r7">;
def R8  : GucReg<8,  "$r8">;
def R9  : GucReg<9,  "$r9">;
def R10 : GucReg<10, "$r10">;
def R11 : GucReg<11, "$r11">;
def R12 : GucReg<12, "$r12">;
def R13 : GucReg<13, "$r13">;
def R14 : GucReg<14, "$r14">;
def R15 : GucReg<15, "$r15">;

def IV0 : GucReg<16,  "$iv0">;
def IV1 : GucReg<17,  "$iv1">;
def S2  : GucReg<18,  "$s2">; // reserved
def TV  : GucReg<19,  "$tv">;
def SP  : GucReg<20,  "$sp">;
def PC  : GucReg<21,  "$pc">;
def XCB : GucReg<22,  "$xcbase">;
def XDB : GucReg<23,  "$xdbase">;
def FLG : GucReg<24,  "$flags">;
def CX  : GucReg<25,  "$cx">;
def CAU : GucReg<26, "$cauth">;
def XTG : GucReg<27, "$xtargets">;
def TS  : GucReg<28, "$tstatus">;
def S13 : GucReg<29, "$s13">; // reserved
def S14 : GucReg<30, "$s14">; // reserved
def S15 : GucReg<31, "$s15">; // reserved

def GR32 : RegisterClass<"Guc", [i32], 32,
   // Volatile registers
  [R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15,
   // Pseudo frame register
   FP,
   // Not allocable
   S15, S14, S13, TS, XTG, CAU, CX, FLG, XDB, XCB, PC, SP, TV, S2, IV1, IV0]>
{
  let MethodProtos = [{
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  let MethodBodies = [{
    GR32Class::iterator
    GR32Class::allocation_order_end(const MachineFunction &MF) const {
      // last 17 register on the list above is reserved
      return end()-17;
    }
  }];
}
