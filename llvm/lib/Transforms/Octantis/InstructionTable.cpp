/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// InstructionTable Class: It is useful to contain all scheduled instructions to be then mapped onto the LiM structure
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "InstructionTable.h"

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "CollectInfo.h"

//STD C++ Libraries
#include <string>
#include <algorithm>
#include <sstream>

using namespace llvm;
using namespace octantis;

///Default constructor
InstructionTable::InstructionTable(){
}

///It changes the operation of a given instruction
void InstructionTable::changeOperationOfInstruction(int* destReg, std::string newOperation){

    //Find destReg
    instructionMapIT = instructionMap.find(destReg);

    //If found, change operation
    if(instructionMapIT != instructionMap.end()){
        (instructionMapIT->second).operation = newOperation;
    } 

}

///It handles the insertion of new instructions that are not load
void InstructionTable::insertInstruction(int ti, int di, std::string operation, int* destReg, operandType destRegType,
                            int * destRegPrt, int* srcReg1, operandType srcReg1Type, int * srcReg1Ptr, int* srcReg2, operandType srcReg2Type,
                            int * srcReg2Ptr, bool isInLoopBody){
    //Temporary empty list
    std::list<std::string> tmpEmptyList;

    //Temporary structure to insert in instructionMap
    InstructionTable::instructionData tmpStruct = {
        ti,
        di,
        operation,
        tmpEmptyList,
        destReg,
        destRegType,
        destRegPrt,
        srcReg1,
        srcReg1Type,
        srcReg1Ptr,
        srcReg2,
        srcReg2Type,
        srcReg2Ptr,
        isInLoopBody,
    };

    //Push the new instruction in instructionMap
    instructionMap.insert(std::pair<int*, InstructionTable::instructionData>(destReg, tmpStruct));

    //Push also in instruction vector
    std::pair<int*, std::string> tmpPair = {destReg, operation};
    instructionOrderVector.push_back(tmpPair);
}

///It inserts info about dest reg related pointer in the related instruction designated by destReg parameter
void InstructionTable::insertDestPtr(int* destReg, int* destRegPtr){

    //Find destReg
    instructionMapIT = instructionMap.find(destReg);

    //If found, insert related pointer
    if(instructionMapIT != instructionMap.end()){
        (instructionMapIT->second).destRegPrt = destRegPtr;
    } 
}

///It handles the insertion of new instructions that are not load with specs
void InstructionTable::insertInstructionWithSpecs(int ti, int di, std::string operation, std::list<std::string> specs, int* destReg, operandType destRegType,
                                     int * destRegPrt, int* srcReg1, operandType srcReg1Type, int * srcReg1Ptr, int* srcReg2, operandType srcReg2Type,
                                     int * srcReg2Ptr, bool isInLoopBody){

    //Temporary structure to insert in instructionMap
    InstructionTable::instructionData tmpStruct = {
        ti,
        di,
        operation,
        specs,
        destReg,
        destRegType,
        destRegPrt,
        srcReg1,
        srcReg1Type,
        srcReg1Ptr,
        srcReg2,
        srcReg2Type,
        srcReg2Ptr,
        isInLoopBody,
    };

    //Push the new instruction in instructionMap
    instructionMap.insert(std::pair<int*, InstructionTable::instructionData>(destReg, tmpStruct));
}

///It handles the insertion of load instructions
void InstructionTable::insertLoadInstruction(int* destReg, operandType oT){
    //Temporary empty list
    std::list<std::string> tmpEmptyList;

    //Temporary structure to insert in instructionMap
    InstructionTable::instructionData tmpStruct = {
        0,
        0,
        "load",
        tmpEmptyList,
        destReg,
        oT,
        nullptr,
        nullptr,
        undefined,
        nullptr,
        nullptr,
        undefined,
        nullptr,
        false,
    };

    
    instructionMapIT = instructionMap.find(destReg);
    //If there is no load with the same operand, it can be scheduled
    if(instructionMapIT == instructionMap.end() ){

        //pushing into instruction map
        instructionMap.insert(std::pair<int*, InstructionTable::instructionData>(destReg, tmpStruct)); 

        //pushing into instruction vector
        std::pair<int*, std::string> tmpPair = {destReg, "load"};
        instructionOrderVector.push_back(tmpPair);

    }

}


///It handles the insertion of load instructions
void InstructionTable::modifyLoadInstruction(int* destReg, int* newDestReg, int* newDestRegPtr){

    instructionMapIT = instructionMap.find(destReg);
    //If there is no load with the same operand, it can be scheduled
    if(instructionMapIT == instructionMap.end() ){

        (instructionMapIT->second).destinationReg = newDestReg;
        (instructionMapIT->second).destRegPrt = newDestRegPtr;

    }

}

///It adds specs to a specific instruction identified by destReg
void InstructionTable::addSpecToInstruction(int* destReg, std::string spec){

    //If destReg is found, push related specs
    instructionMapIT = instructionMap.find(destReg);

    if(instructionMapIT != instructionMap.end()){
        ((instructionMapIT->second).specifications).push_back(spec);
    }
}

void InstructionTable::modifyGivenField(int* destReg, const std::string &field, int* newField, operandType oT){

    instructionMapIT = instructionMap.find(destReg);

    if(instructionMapIT != instructionMap.end()){
        if(field == "srcReg1"){
            (instructionMapIT->second).sourceReg1 = newField;
        }else if (field == "srcReg1Ptr"){
            (instructionMapIT->second).srcReg1Ptr = newField;
        }else if(field == "srcReg2"){
            (instructionMapIT->second).sourceReg2 = newField;
        }else if(field == "srcReg2Ptr"){
            (instructionMapIT->second).srcReg2Ptr = newField;
        }else if(field == "destReg"){
            (instructionMapIT->second).destinationReg = newField;
        }else if(field == "destRegPtr"){
            (instructionMapIT->second).destRegPrt = newField;
        }else if(field == "destRegType"){
            (instructionMapIT->second).destRegType = oT;
        }else if(field == "srcReg1Type"){
            (instructionMapIT->second).srcReg1Type = oT;
        }else if(field == "srcReg2Type"){
            (instructionMapIT->second).srcReg2Type = oT;
        }
    }

}

void InstructionTable::swapOperands(int* destReg){

    instructionMapIT = instructionMap.find(destReg);

    int* tmp1 = ((instructionMapIT)->second).sourceReg1;
    int* tmp2 = ((instructionMapIT)->second).srcReg1Ptr;
    InstructionTable::operandType tmp3 = ((instructionMapIT)->second).srcReg1Type;
    ((instructionMapIT)->second).sourceReg1 = ((instructionMapIT)->second).sourceReg2;
    ((instructionMapIT)->second).srcReg1Ptr = ((instructionMapIT)->second).srcReg2Ptr;
    ((instructionMapIT)->second).srcReg1Type = ((instructionMapIT)->second).srcReg2Type;
    ((instructionMapIT)->second).sourceReg2 = tmp1;
    ((instructionMapIT)->second).srcReg2Ptr = tmp2;
    ((instructionMapIT)->second).srcReg2Type = tmp3;
    ((instructionMapIT)->second).destRegPrt = ((instructionMapIT)->second).srcReg2Ptr;

}


///It returns the Instruction Map
std::map<int *, InstructionTable::instructionData> InstructionTable::getInstructionMap(){
    return instructionMap;
}

///It sets the init time and delay for the operation
void InstructionTable::setInstructionTimeAndDelay(int * destReg, int time, int delay){

    instructionMapIT = instructionMap.find(destReg);

    if(instructionMapIT != instructionMap.end()){
        (instructionMapIT->second).ti = time;
        (instructionMapIT->second).di = delay;
    }

}

///It returns the available time of the instructionw whose destReg is srcReg
int InstructionTable::getAvailableTime(int* srcReg){

    //If srcReg is found, return the sum of its ti and di
    instructionMapIT = instructionMap.find(srcReg);

    return (instructionMapIT->second).ti + (instructionMapIT->second).di;
}

///It handles the changing of the destReg of an instruction
int InstructionTable::insertNewInstructionDestReg(std::string operation, int * newDestReg, int * oldDestReg){

    instructionMapIT = instructionMap.find(oldDestReg);

    int ti = (instructionMapIT->second).ti;

    InstructionTable::instructionData dataStruct = instructionMapIT->second;
    dataStruct.destinationReg = newDestReg;    
    dataStruct.operation = operation;

    instructionMap.insert(instructionMapIT, std::pair<int*, InstructionTable::instructionData>(newDestReg, dataStruct));

    return ti;
}

///It reorders the vector containing all scheduled instructions
void InstructionTable::reorderInstructionVector(){

    std::vector<std::pair<int*, std::string>> orderedVector;
    
    //Reordering the instruction vector putting load instructions at the beginning and after all others

    for(auto vectIT = instructionOrderVector.begin(); vectIT != instructionOrderVector.end(); ++vectIT){
        if(vectIT->second == "load"){
            orderedVector.push_back(*vectIT);
        }
    }

    for(auto vectIT = instructionOrderVector.begin(); vectIT != instructionOrderVector.end(); ++vectIT){
        if(vectIT->second != "load"){
            orderedVector.push_back(*vectIT);
        }
    }

    instructionOrderVector.clear();
    instructionOrderVector = orderedVector;

}

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/
void InstructionTable::printInstructionMap(){
    errs() << "Printing scheduled instructions\n";

    int count = 0;
    std::stringstream ssSrc1, ssSrc2, ssDest, ssSrc1Ptr, ssSrc2Ptr, ssDestPtr;
    std::string src1T, src2T, destT;

    errs() << "|-------------------------------------------------------------------------------------------------------------------------------|\n";
    errs() << "|OP             |Dest Reg       |Dest Reg T     |Src Reg 1      |Src Reg 1 T    |Src Reg 2      |Src Reg 2 T    |Others         |\n";
    errs() << "|-------------------------------------------------------------------------------------------------------------------------------|\n";

    for(instructionMapIT = instructionMap.begin(); instructionMapIT != instructionMap.end(); ++instructionMapIT){

        ssSrc1 << (instructionMapIT->second).sourceReg1;
        ssSrc2 << (instructionMapIT->second).sourceReg2;
        ssDest << (instructionMapIT->second).destinationReg;
        ssSrc1Ptr << (instructionMapIT->second).srcReg1Ptr;
        ssSrc2Ptr << (instructionMapIT->second).srcReg2Ptr;
        ssDestPtr << (instructionMapIT->second).destRegPrt;

        if((instructionMapIT->second).srcReg1Type == InstructionTable::array){
            src1T = "Array";
        }else if ((instructionMapIT->second).srcReg1Type == InstructionTable::singleVariable){
            src1T = "Single Var";
        }else if ((instructionMapIT->second).srcReg1Type == InstructionTable::fakeArray){
            src1T = "Temp Var";
        }else if ((instructionMapIT->second).srcReg1Type == InstructionTable::constant){
            src1T = "Constant";
        }

        if((instructionMapIT->second).srcReg2Type == InstructionTable::array){
            src2T = "Array";
        }else if ((instructionMapIT->second).srcReg2Type == InstructionTable::singleVariable){
            src2T = "Single Var";
        }else if ((instructionMapIT->second).srcReg2Type == InstructionTable::fakeArray){
            src2T = "Temp Var";
        }else if ((instructionMapIT->second).srcReg2Type == InstructionTable::constant){
            src2T = "Constant";
        }

        if((instructionMapIT->second).destRegType == InstructionTable::array){
            destT = "Array";
        }else if ((instructionMapIT->second).destRegType == InstructionTable::singleVariable){
            destT = "Single Var";
        }else if ((instructionMapIT->second).destRegType == InstructionTable::fakeArray){
            destT = "Temp Var";
        }else if ((instructionMapIT->second).destRegType == InstructionTable::constant){
            destT = "Constant";
        }

        errs() << "|" << (instructionMapIT->second).operation; for(int i = (instructionMapIT->second).operation.length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << ssDest.str(); for(int i = ssDest.str().length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << destT; for(int i = destT.length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << ssSrc1.str(); for(int i = ssSrc1.str().length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << src1T; for(int i = src1T.length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << ssSrc2.str(); for(int i = ssSrc2.str().length(); i < 15; ++i){errs() << " ";} errs() << "|";
        errs() << src2T; for(int i = src2T.length(); i < 15; ++i){errs() << " ";} errs() << "|\n";  
        errs() << (instructionMapIT->second).destRegPrt << " " << (instructionMapIT->second).srcReg1Ptr << " " << (instructionMapIT->second).srcReg2Ptr << "\n";
        errs() << "|-------------------------------------------------------------------------------------------------------------------------------|\n";

        ssSrc1.str("");
        ssSrc2.str("");
        ssDest.str("");
        ssSrc1Ptr.str("");
        ssSrc2Ptr.str("");
        ssDestPtr.str("");

    }
}
//--------------------END DEGUB FUNCTION---------------------