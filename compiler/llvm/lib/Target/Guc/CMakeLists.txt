set(LLVM_TARGET_DEFINITIONS Guc.td)

tablegen(GucGenRegisterInfo.h.inc -gen-register-desc-header)
tablegen(GucGenRegisterNames.inc -gen-register-enums)
tablegen(GucGenRegisterInfo.inc -gen-register-desc)
tablegen(GucGenInstrNames.inc -gen-instr-enums)
tablegen(GucGenInstrInfo.inc -gen-instr-desc)
tablegen(GucGenAsmWriter.inc -gen-asm-writer)
tablegen(GucGenDAGISel.inc -gen-dag-isel)
tablegen(GucGenCallingConv.inc -gen-callingconv)
tablegen(GucGenSubtarget.inc -gen-subtarget)

add_llvm_target(GucCodeGen
  GucBranchSelector.cpp
  GucISelDAGToDAG.cpp
  GucISelLowering.cpp
  GucInstrInfo.cpp
  GucMCAsmInfo.cpp
  GucRegisterInfo.cpp
  GucSubtarget.cpp
  GucTargetMachine.cpp
  GucSelectionDAGInfo.cpp
  )

target_link_libraries (LLVMGucCodeGen LLVMSelectionDAG)
