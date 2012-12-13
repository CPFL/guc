##===- lib/Target/Guc/Makefile --------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source 
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMGucCodeGen
TARGET = Guc

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = GucGenRegisterInfo.h.inc GucGenRegisterNames.inc \
		GucGenRegisterInfo.inc GucGenInstrNames.inc \
		GucGenInstrInfo.inc GucGenAsmWriter.inc \
		GucGenDAGISel.inc GucGenCallingConv.inc \
		GucGenSubtarget.inc

DIRS = AsmPrinter TargetInfo

include $(LEVEL)/Makefile.common

