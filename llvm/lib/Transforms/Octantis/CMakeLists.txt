if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_library( LLVMOctantis MODULE BUILDTREE_ONLY
  PrintConfig.cpp
  OperationsDelay.cpp
  InstructionTable.cpp
  AccessPattern.cpp
  DependencyDetector.cpp
  ASAP.cpp
  LiMArray.cpp
  FiniteStateMachine.cpp
  AdditionalLogicPorts.cpp
  AddressList.cpp
  PrintDexFile.cpp
  PrintCadFile.cpp
  PrintVhdlFile.cpp
  PointerInfoTable.cpp
  LoopInfoTable.cpp
  LiMBinder.cpp
  CollectInfo.cpp
  OctantisPass.cpp

#  DEPENDS
#  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
