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
#include "Code_Generation/PrintDexFile.h"
#include "Code_Generation/PrintVhdlFile.h"
#include "Code_Generation/PrintCadFile.h"
#include "Scheduling/ASAP.h"
#include "Analysis/CollectInfo.h"
#include "Binding/LiMBinder.h"
#include "Parsers/PrintConfig.h"
#include "Analysis/DependencyDetector.h"
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

            //Parsing
            ConfigurationHandler.ParseConfigFile();

            //Extracting information about loops
            LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

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
            instructionTable.printInstructionMap();
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

            //Construction of DFG
            DependencyDetector dependencyDetector(InfoCollector, debugMode);
            dependencyDetector.detectDependencies(InfoCollector.getValidBBs());

            //Scheduling
            if (ConfigurationHandler.getSchedulingAlgorithm() == "ASAP") {

                //ASAP Scheduling
                ASAP ASAPSched(dependencyDetector.getDependencyGraph(), debugMode);
                ASAPSched.scheduleFunction();
                instructionTable = ASAPSched.getScheduledIT();

            }

            //Binding
            LiMBinder Binder(instructionTable, InfoCollector, debugMode, ConfigurationHandler.getWordLength(), ConfigurationHandler.getOptimizationTarget());

            errs() << "End of Synthesis Process\n";


            //.vhd file generation
            PrintVhdlFile VhdlPrinter(&(Binder.MemArray), &(Binder.FSMLim), debugMode);
            VhdlPrinter.print();


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
        InstructionTable instructionTable;

        PrintConfig ConfigurationHandler;

    };

} // namespace octantis

using namespace octantis;

char OctantisPass::ID = 0;
static RegisterPass<OctantisPass> X("octantisPass",
                                    "Description of OctantisPass",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);