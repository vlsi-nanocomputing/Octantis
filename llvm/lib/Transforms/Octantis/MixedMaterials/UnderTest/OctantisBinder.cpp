//===- OctantisBinder.cpp - Binder pass from Octantis Project -------------===//
//
// Description of the Pass
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "octantisBinder"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // OctantisPass - The first implementation, without getAnalysisUsage.
  struct OctantisBinder : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    OctantisBinder() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char OctantisBinder::ID = 0;
static RegisterPass<OctantisBinder> X("octantisBinder", "Description of OctantisBinder",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

