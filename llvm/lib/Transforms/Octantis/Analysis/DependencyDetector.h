/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//  DependencyDetector class: Class useful for the detection of dependencies among schedulable instructions
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef DEPENDENCYDETECTOR_H
#define DEPENDENCYDETECTOR_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"
#include "Utils/AdditionalLogicPorts.h"

using namespace llvm;

#include "CollectInfo.h"
#include "Utils/InstructionTable.h"

namespace octantis {

/// Class useful for the collection of information
    class DependencyDetector {

    public:

        ///Constructor needing a CollectInfo object
        DependencyDetector(CollectInfo CI, bool debugMode)
                : infoCollection(CI), accumulationValidityCount(0), debugMode(debugMode) {};


        ///Function useful for scheduling
        void detectDependencies(std::list<BasicBlock *> basicBlocksToSchedule);


        ///It handles the insertion in IM of instructions that are not load instructions
        void handleInstructionInserting(const Instruction &I);

        ///It handles store instructions. It means that it updates lastUsedRegMap, detects if dest reg is an array and update infos related to it (tmpDestPtr)
        void handleStoreInstruction(const Instruction &I);


        void setRegType(bool isArray, int *reg, std::string operandType);

        ///Function useful to retrieve the right src register name (which has to correspond to the dest reg name of a previous instruction)
        ///to assign to the temp src variable in scheduling function
        int *getRealSrcReg(int *allocatedReg);


        ///Function useful to get the entire instructionMap built by the scheduling operation
        InstructionTable getDependencyGraph();

    private:

        ///Scheduling support structure
        InstructionTable IM;

        ///Map useful for rembering the last name associated to a register in order to properly build the dest-src name connections
        /// (allocatedReg - lastAssociatedUse)
        std::map<int *, int *> lastUsedRegMap;

        ///Iterator over lastUsedRegMap
        std::map<int *, int *>::iterator lastUsedRegMapIT;

        std::map<int *, int *>::iterator aliasMapIT;

        ///Object containing information collected before scheduling
        CollectInfo infoCollection;

        ///Variables useful to implement the scheduling, they are temp variables containing important infos about
        ///the next instruction to be scheduled

        ///accumulation related variables
        int *tmpAccumulationOp;
        int *accDestReg;
        int accumulationValidityCount;
        ///variable useful for negated operation detection
        int *tmpLogicOperatorDestReg;

        ///src and dest operands
        int *srcReg1;
        int *srcReg2;
        int *tmpSrcReg2;
        int *destReg;

        bool isSrc1Array = false;
        bool isSrc2Array = false;

        ///Operands type
        InstructionTable::operandType srcReg1Type = InstructionTable::undefined;
        InstructionTable::operandType srcReg2Type = InstructionTable::undefined;
        InstructionTable::operandType destRegType = InstructionTable::undefined;
        InstructionTable::operandType backUpSrcReg2Type = InstructionTable::undefined;
        InstructionTable::operandType backUpSrcReg1Type = InstructionTable::undefined;

        ///ptr associated to an operand
        int *tmpPtrSrc1;
        int *tmpPtrSrc2;
        int *tmpPtrDest;

        ///init time for src1 and src2 operands
        int tSrcReg1;
        int tSrcReg2;

        ///temp variable for counting how many load operations have been performed before a store
        int loadCount;

        ///operation string
        std::string operation;

        bool isSrc1Constant = false;
        bool isSrc2Constant = false;


        ///Object useful for defining the modified version of a
        /// specific logic operator
        AdditionalLogicPorts changeLogic;

        std::list<int *> tmpVariableList;
        std::list<int *>::iterator tmpVariableListIT;

        bool debugMode;
    };

} //End of Octantis' namespace

#endif // DependencyDetector_H
