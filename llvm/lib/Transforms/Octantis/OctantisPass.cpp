/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// OctantisPass Pass: backend pass for the generation of Dexima's configuration files.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//Test Include files
#include "llvm/IR/User.h"
#include "llvm/Analysis/DependenceAnalysis.h"

//Octantis' classes
#include "SchedulingASAP.h"
#include "LiMCompiler.h"
//#include "InstructionTable.h"

using namespace llvm;

namespace octantis{

  // OctantisPass
  struct OctantisPass : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    OctantisPass() : FunctionPass(ID) {}

    void compileAndPrint(){
          //Here the definition of the LiM COMPILER
          LiMCompiler Compiler(ASAPScheduler.getIT());
      }

    // SCHEDULING
    bool runOnFunction(Function &F) override {
        for (BasicBlock &BB : F) {
            errs() << "Basic Block: " << BB.getName() << " size " << BB.size() << "\n";

            for (Instruction &I : BB) {
                ASAPScheduler.addNewInstruction(I);
            }
        }

      compileAndPrint();
      return false;
    }


private:
    //Here the definition of the SCHEDULING ALGORITHM
    SchedulingASAP ASAPScheduler;

  };


}// End Octantis' namespace


using namespace octantis;

char OctantisPass::ID = 0;
static RegisterPass<OctantisPass> X("octantisPass", "Description of OctantisPass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

