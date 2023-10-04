/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// ASAP class: Class useful for the implementation of the ASAP scheduling algorithm                     
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "ASAP.h"

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "Analysis/CollectInfo.h"
#include "Utils/InstructionTable.h"
#include "Utils/OperationsDelay.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

void ASAP::scheduleFunction(){

    errs() << "Starting the ASAP scheduling on selected function\n\n\n";

    std::map<int *, InstructionTable::instructionData> instructionMap;
    std::map<int *, InstructionTable::instructionData>::iterator instructionMapIT;

    instructionMap = IT.getInstructionMap();

    for(instructionMapIT = instructionMap.begin(); instructionMapIT != instructionMap.end(); ++instructionMapIT){

        if((instructionMapIT->second).operation == "load"){

            IT.setInstructionTimeAndDelay(instructionMapIT->first, 0, 0);

        }else{

            //Get time init time for this instruction
            ti = getTime((instructionMapIT->second).srcReg1Type, (instructionMapIT->second).srcReg2Type, 
                        (instructionMapIT->second).sourceReg1, (instructionMapIT->second).sourceReg2);

            //Get delay of operation
            di = OD.getOperationDelay((instructionMapIT->second).operation);

            IT.setInstructionTimeAndDelay(instructionMapIT->first, ti, di);

        }
    }
    
    //IT.printInstructionMap();
    
    //reorder the internal instruction vector to IT, preparing for binding phase
    IT.reorderInstructionVector();

}

///Function useful to get the init time for a newly scheduled instruction
int ASAP::getTime(const InstructionTable::operandType &src1Type, const InstructionTable::operandType &src2Type, int* &src1, int* &src2){

    int tSrc1, tSrc2;

    //If operand is constant, its init time is 0, else it must be retrieved from IT

    if(src1Type == InstructionTable::constant){
        tSrc1 = 0;
    }else{
        tSrc1 = IT.getAvailableTime(src1);
    }

    if(src2Type == InstructionTable::constant){
        tSrc2 = 0;
    }else{
        tSrc2 = IT.getAvailableTime(src2);
    }

    //init time for the new instruction is the max between tSrc1 and tSrc2
    int ti = (tSrc1 > tSrc2) ? tSrc1 : tSrc2;

    return ti;
}


///Function useful to get the IT with scheduled instructions
InstructionTable ASAP::getScheduledIT(){
    return IT;
}
