/*-------------------------------------- The Octantis Project --------------------------------------*/
//
//  ASAP class: Class useful for the implementation of the ASAP scheduling algorithm
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef ASAP_H
#define ASAP_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"
#include "AdditionalLogicPorts.h"

using namespace llvm;

#include "CollectInfo.h"
#include "InstructionMap.h"

namespace octantis {

/// Class useful for the collection of information
class ASAP
{

public:

    ///Constructor needing a CollectInfo object
    ASAP(CollectInfo CI);

    ///Function useful to retrieve the right src register name (which has to correspond to the dest reg name of a previous instruction)
    ///to assign to the temp src variable in scheduling function
    int * getRealSrcReg(int * allocatedReg);

    ///Function useful for scheduling
    void scheduleFunction(std::list<BasicBlock*> basicBlocksToSchedule);

    ///It handles the insertion in IM of instructions that are not load instructions
    void handleInstructionInserting(const Instruction &I);

    ///It handles store instructions. It means that it updates lastUsedRegMap, detects if dest reg is an array and update infos related to it (tmpDestPtr)
    void handleStoreInstruction(const Instruction &I);

    ///Function useful to get the init time for a newly scheduled instruction
    int getTime(const bool &isSrc1Const, const bool &isSrc2Const, int* &src1, int* &src2);

    ///Function useful to get the entire instructionMap built by the scheduling operation
    InstructionMap getInstructionMap();

private:   

    ///Scheduling support structure
    InstructionMap IM;

    ///Map useful for rembering the last name associated to a register in order to properly build the dest-src name connections
    /// (allocatedReg - lastAssociatedUse)
    std::map<int*, int*> lastUsedRegMap;

    ///Iterator over lastUsedRegMap
    std::map<int*, int*>::iterator lastUsedRegMapIT;

    std::map<int*, int*>::iterator aliasMapIT;

    ///Object containing information collected before scheduling
    CollectInfo infoCollection;

    ///Variables useful to implement the scheduling, they are temp variables containing important infos about
    ///the next instruction to be scheduled

    ///accumulation related variables
    int* tmpAccumulationOp;
    int accumulationValidityCount;
    ///variable useful for negated operation detection
    int* tmpLogicOperatorDestReg;

    ///src and dest operands
    int* srcReg1;
    int* srcReg2;
    int* destReg;

    ///ptr associated to an operand
    int* tmpPtrSrc1;
    int* tmpPtrSrc2;
    int* tmpPtrDest;

    ///init time for src1 and src2 operands
    int tSrcReg1;
    int tSrcReg2;

    ///temp variable for counting how many load operations have been performed before a store
    int loadCount;
    int loadBeforeStore;

    ///init time of an operation and its "delay"
    int ti;
    int di;

    ///operation string
    std::string operation;

    bool isSrc1Constant = false;
    bool isSrc2Constant = false;
    

    ///Object useful for defining the modified version of a
    /// specific logic operator
    AdditionalLogicPorts changeLogic;

};

} //End of Octantis' namespace

#endif // ASAP_H
