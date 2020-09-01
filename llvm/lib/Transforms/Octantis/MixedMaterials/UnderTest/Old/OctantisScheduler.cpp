/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// OctantisScheduler Pass: dedicated to the scheduling of the operations.
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
//#include "InstructionTable.h"

using namespace llvm;
using namespace octantis; //To be used only when it has been defined

namespace octantis{

    //Here the definition of the SCHEDULING ALGORITHM
    SchedulingASAP ASAPScheduler;

  // OctantisScheduler - The first implementation, without getAnalysisUsage.
  struct OctantisScheduler : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    OctantisScheduler() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {    
        for (BasicBlock &BB : F) {
            errs() << "Basic Block: " << BB.getName() << " size " << BB.size() << "\n";

            for (Instruction &I : BB) {

                ASAPScheduler.addNewInstruction(I);

               
                //errs() << "\t" << I << ", opcode " <<I.getOpcode() << " opName "<< I.getOpcodeName() << "\n";
                //errs() << "\t\tIs it unary? A: " << I.isUnaryOp() << "\n";
                //errs() << "\t\tIs it binary? A: " << I.isBinaryOp() << "\n";
                //errs() << "\t\t operand1: " << I.getOperand(0) << "\n";
               // isTerminator(): end of basic block;
            }

        }
      //errs() << "Hello: ";
      //errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char octantis::OctantisScheduler::ID = 0;
static RegisterPass<octantis::OctantisScheduler> X("octantisScheduler", "Description of OctantisScheduler",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

