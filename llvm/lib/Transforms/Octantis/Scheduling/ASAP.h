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
#include "Utils/AdditionalLogicPorts.h"

using namespace llvm;

#include "Analysis/DependencyDetector.h"
#include "Utils/InstructionTable.h"
#include "Utils/OperationsDelay.h"

namespace octantis {

/// Class useful for the collection of information
class ASAP
{

public:

    ///Constructor needing a CollectInfo object
    ASAP(InstructionTable instructionTable, bool debugMode)
        :IT(instructionTable), debugMode(debugMode){};

    ///Function useful for scheduling
    void scheduleFunction();

    ///Function useful to get the init time for a newly scheduled instruction
    int getTime(const InstructionTable::operandType &src1Type, const InstructionTable::operandType &src2Type, int* &src1, int* &src2);

    ///Function useful to get the IT with scheduled instructions
    InstructionTable getScheduledIT();

private:   

    ///Scheduling support structure
    InstructionTable IT;

    OperationsDelay OD;

    ///init time for src1 and src2 operands
    int tSrcReg1;
    int tSrcReg2;

    ///init time of an operation and its "delay"
    int ti;
    int di;

    ///operation string
    std::string operation;

    bool debugMode;
};

} //End of Octantis' namespace

#endif // ASAP_H
