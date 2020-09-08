if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_library( LLVMOctantis MODULE BUILDTREE_ONLY
  InstructionTable.cpp
  SchedulingASAP.cpp
  LiMArray.cpp
  FiniteStateMachine.cpp
  AdditionalLogicPorts.cpp
  LiMCompiler.cpp
  PrintDexFile.cpp
  OctantisPass.cpp

#  DEPENDS
#  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
