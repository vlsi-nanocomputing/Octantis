/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// OctantisLegalizer PASS: Pass useful to remove unwanted llvm-IR instructions (i.e. memset,
//                         memcpy, memmove)
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "octantisLegalizer"

namespace octantis {
  // OctantisPass - The first implementation, without getAnalysisUsage.
  struct OctantisLegalizer : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    OctantisLegalizer() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {

        // Analysis of all the instructions to detect the ones that have to be substituted
        for (Function::iterator BB = F.begin(), EE = F.end(); BB != EE; ++BB) {
            for(BasicBlock::iterator I = BB->begin, E = BB->end(); I != E; ++I){


            }

        }







      return false;
    }
  };
}

char OctantisLegalizer::ID = 0;
static RegisterPass<OctantisLegalizer> X("octantisLegalizer", "Description of OctantisLegalizer",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

