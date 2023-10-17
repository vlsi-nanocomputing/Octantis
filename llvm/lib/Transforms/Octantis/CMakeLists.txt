if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_library( LLVMOctantis MODULE BUILDTREE_ONLY
        Parsers/PrintConfig.cpp
        Utils/OperationsDelay.cpp
        Utils/InstructionTable.cpp
        Utils/AccessPattern.cpp
        Analysis/DependencyDetector.cpp
        Scheduling/ASAP.cpp
        Binding/LiMArray.cpp
        Binding/FiniteStateMachine.cpp
        Utils/AdditionalLogicPorts.cpp
        Utils/AddressList.cpp
        Code_Generation/PrintDexFile.cpp
        Code_Generation/PrintCadFile.cpp
        Code_Generation/PrintVhdlFile.cpp
        Analysis/PointerInfoTable.cpp
        Analysis/LoopInfoTable.cpp
        Binding/LiMBinder.cpp
        Analysis/CollectInfo.cpp
  OctantisPass.cpp

#  DEPENDS
#  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
