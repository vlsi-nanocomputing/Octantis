/*-------------------------------------- The Octantis Project --------------------------------------*/
/// \file
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

// Include files for Loop Analysis
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/MachineLoopInfo.h"

// Octantis' classes
//#include "LiMCompiler.h"
#include "PrintDexFile.h"
#include "PrintVhdlFile.h"
#include "PrintCadFile.h"
#include "SchedulingASAP.h"
#include "ASAP.h"
#include "CollectInfo.h"
#include "LimCompilerNew.h"
#include "PrintConfig.h"
//#include "InstructionTable.h"

#include <list>

using namespace llvm;

namespace octantis {

// OctantisPass
struct OctantisPass : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  OctantisPass() : FunctionPass(ID) {}

  void compileAndPrint() {
    LimCompilerNew Compiler(IM, InfoCollector);

    //.vhd file generation
    PrintVhdlFile VhdlPrinter(&(Compiler.MemArray), &(Compiler.FSMLim));
    VhdlPrinter.print();

    /* DISABLED UNTIL THE INPUT INTERFACES WITH DEXIMA ARE NOT STABLE! */
    //PrintDexFile Printer(&(Compiler.MemArray), &(Compiler.FSMLim));
    //Printer.print();
    /* --------------------------------------------------------------- */

    //Dexima cad files generation
    //PrintCadFile CadFilePrinter(&(Compiler.MemArray), &(Compiler.FSMLim));
    //CadFilePrinter.print();
  }

  void scheduleASAPFunction(){
    ASAP ASAPSched(InfoCollector);

    ASAPSched.scheduleFunction(InfoCollector.getValidBBs());

    IM = ASAPSched.getInstructionMap();
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  // SCHEDULING
  bool runOnFunction(Function &F) override {

    
    // Get the parameters of the function
    /*for (auto args = F.arg_begin(); args != F.arg_end(); ++args) {
      errs() << "A new input parameter found: " << (int *)args << "\n";
      ASAPScheduler.addFuncInputParameter((int *)args);
    }*/

    ConfigurationHandler.ParseConfigFile();
    ConfigurationHandler.printConfigParameters();


    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    InfoCollector.parseFunction(F, LI);

    /////DEBUG/////
    InfoCollector.printAliasInfoMap();
    errs() << "\n\n\n\n";
    InfoCollector.printArrayInfoMap();
    errs() << "\n\n\n\n";
    InfoCollector.printIteratorsAliasMap();
    errs() << "\n\n\n\n";
    /////DEBUG/////

    scheduleASAPFunction();

    /////DEBUG/////
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
    /////DEBUG/////

    compileAndPrint();

    return false;
    
  }

private:
  InstructionMap IM;

  CollectInfo InfoCollector;

  PrintConfig ConfigurationHandler;

};

} // namespace octantis

using namespace octantis;

char OctantisPass::ID = 0;
static RegisterPass<OctantisPass> X("octantisPass",
                                    "Description of OctantisPass",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);