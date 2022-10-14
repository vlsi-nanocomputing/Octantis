/*-------------------------------------- The Octantis Project --------------------------------------*/
/// 
/// OctantisPass Pass: backend pass for the generation of Dexima's configuration
/// files.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico
// di Torino © Alessio Nicola 2021 (alessio.nicola@studenti.polito.it) for
// Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/


#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/CommandLine.h"

// Include files for Loop Analysis
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/CodeGen/MachineLoopInfo.h"

// Octantis' classes
//#include "LiMCompiler.h"
#include "PrintDexFile.h"
#include "PrintVhdlFile.h"
#include "PrintCadFile.h"
#include "ASAP.h"
#include "CollectInfo.h"
#include "LiMBinder.h"
#include "PrintConfig.h"
#include "DependencyDetector.h"
//#include "InstructionTable.h"

#include <list>

using namespace llvm;

namespace octantis {

  cl::opt<bool> debugMode("debugMode", cl::desc("Specify debug mode"));

// OctantisPass
struct OctantisPass : public FunctionPass {

  static char ID; // Pass identification, replacement for typeid
  OctantisPass() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
  }

  bool runOnFunction(Function &F) override {

    
    // Get the parameters of the function -> TO DO
    /*for (auto args = F.arg_begin(); args != F.arg_end(); ++args) {
      errs() << "A new input parameter found: " << (int *)args << "\n";
      ASAPScheduler.addFuncInputParameter((int *)args);
    }*/

    ConfigurationHandler.ParseConfigFile();
    ConfigurationHandler.printConfigParameters();

    //Extracting information about loops
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();


    bool ptr = false;
    for (BasicBlock &B : F){
      for (Instruction &I : B){
        if(GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I)){

          errs() << "NumIndices: " << GEP->getNumIndices() << " NumUses: " << GEP->getNumUses() << " pointerOpIndex: " << GEP->getPointerOperandIndex() << "\n";

          errs() << ": " <<  << "\n";

        }
      }
    }


    CollectInfo InfoCollector(debugMode);

    //Start InfoCollector pass on the current function
    InfoCollector.parseFunction(F, LI);



    /////DEBUG/////
    /*
    if(debugMode){
    InfoCollector.printAliasInfoMap();
    errs() << "\n\n\n\n";
    InfoCollector.printArrayInfoMap();
    errs() << "\n\n\n\n";
    InfoCollector.printIteratorsAliasMap();
    errs() << "\n\n\n\n";
    IM.printInstructionMap();
    errs() << "\n\n\n\n";
    (InfoCollector.PIT).printPointerInfoTable();
    errs() << "\n\n\n\n";
    (InfoCollector.LIT).printNestedLoopMap();
    errs() << "\n\n\n\n";
    (InfoCollector.LIT).printLoopInfoMap();
    errs() << "\n\n\n\n";
    InfoCollector.printAliasInfoMap();
    errs() << "\n\n\n\n";
    }
    */
    /////DEBUG/////



    DependencyDetector DD(InfoCollector, debugMode);

    DD.detectDependencies(InfoCollector.getValidBBs());

    ASAP ASAPSched(DD.getDependencyGraph(), debugMode);

    ASAPSched.scheduleFunction();

    IM = ASAPSched.getScheduledIT();




    LiMBinder Binder(IM, InfoCollector, debugMode);

    /*
    //.vhd file generation
    PrintVhdlFile VhdlPrinter(&(Binder.MemArray), &(Binder.FSMLim), debugMode);
    VhdlPrinter.print();
    */

    /* DISABLED UNTIL THE INPUT INTERFACES WITH DEXIMA ARE NOT STABLE! */
    //PrintDexFile Printer(&(Compiler.MemArray), &(Compiler.FSMLim));
    //Printer.print();
    /* --------------------------------------------------------------- */

    /*
    //Dexima cad files generation
    PrintCadFile CadFilePrinter(&(Binder.MemArray), &(Binder.FSMLim));
    CadFilePrinter.print();
    */

    return false;
    
  }

private:
  InstructionTable IM;

  PrintConfig ConfigurationHandler;

};

} // namespace octantis

using namespace octantis;

char OctantisPass::ID = 0;
static RegisterPass<OctantisPass> X("octantisPass",
                                    "Description of OctantisPass",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);