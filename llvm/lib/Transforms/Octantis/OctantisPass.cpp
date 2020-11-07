/*-------------------------------------- The Octantis Project --------------------------------------*/
/// \file
/// OctantisPass Pass: backend pass for the generation of Dexima's configuration files.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//Include files for Loop Analysis
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/MachineLoopInfo.h"

//Octantis' classes
#include "SchedulingASAP.h"
#include "LiMCompiler.h"
#include "PrintDexFile.h"
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

          /* DISABLED UNTIL THE INPUT INTERFACES WITH DEXIMA ARE NOT STABLE! */
          //PrintDexFile Printer(&(Compiler.MemArray), &(Compiler.FSMLim));
          //Printer.print();
          /* --------------------------------------------------------------- */
      }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
          AU.setPreservesCFG();
          AU.addRequired<LoopInfoWrapperPass>();
        }

    // SCHEDULING
    bool runOnFunction(Function &F) override {

        //Get the parameters of the function
        for(auto args=F.arg_begin(); args!=F.arg_end(); ++args){
            errs() << "A new input parameter found: "<< (int *) args << "\n";
            ASAPScheduler.addFuncInputParameter((int *) args);
        }

        LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

        if(!LI.empty())
        {
            isThereALoop=true;
            errs() << "A loop is present!\n";
        }

        for (BasicBlock &BB : F) {
            errs() << "Basic Block: " << (int *) &BB << " size " << BB.size() << "\n";

            //Check if the BB is a loop header
            if(isThereALoop){

                //Check if the successor is the last block of the loop
                BasicBlock * successor = BB.getSingleSuccessor();
                errs() << "The successor is: " << (int*) &(*successor) << "\n";


                if(LI.isLoopHeader(&BB)){
                    errs() << "\tThis is a Loop header!\n";

                    //Invalidate the Header Basic Block
                    ASAPScheduler.setBBAsNotValid(BB);

                    //Pass the basic block to the scheduler for
                    //the correct loop info extraction
                    ASAPScheduler.parseLoopInfo(BB);
                }

                if(ASAPScheduler.isTheLastBBInLoop(*successor)){
                    errs() << "The next BB is the last one!\n";

                    //The next basic block will be escaped
                    //  NOTE: there is the assumption that
                    //        the loop proceeds traditionally
                    //        (i.e. i=i+1). During the next
                    //        updates impose chacks!
                    ASAPScheduler.setBBAsNotValid(BB);

                    //Terminate the scheduling of loop instructions
                    ASAPScheduler.endOfCurrentLoop();


                }
            }

            //Verification if the basic block is valid.
            if(ASAPScheduler.isBBValid(BB))
            {
                errs() << "This BB is valid!\n";

                // Parsing the internal instructions
                for (Instruction &I : BB) {
                    errs()<< "Instruction passed: " << I << "\n";

                    ASAPScheduler.addNewInstruction(I);
                }
            }

            errs() << "This BB is NOT valid and it won't be parsed!\n";

        }

      compileAndPrint();
      return false;
    }


private:
    //Here the definition of the SCHEDULING ALGORITHM
    SchedulingASAP ASAPScheduler;

    //Identification of loops
    bool isThereALoop=false;

  };


}// End Octantis' namespace


using namespace octantis;

char OctantisPass::ID = 0;
static RegisterPass<OctantisPass> X("octantisPass", "Description of OctantisPass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

