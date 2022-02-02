/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// InstructionMap Class: It is useful to contain all scheduled instructions to be then mapped onto the LiM structure
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "InstructionMap.h"

//LLVM Include Files
#include "llvm/IR/Function.h"

#include "CollectInfo.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Default constructor
InstructionMap::InstructionMap(){
}

///It handles the insertion of new instructions that are not load
void InstructionMap::insertInstruction(int ti, int di, std::string operation, int* destReg, int * destRegPrt, int* srcReg1,
                                         int * srcReg1Ptr, int* srcReg2, int * srcReg2Ptr, bool isInLoopBody){
    //Temporary empty list
    std::list<std::string> tmpEmptyList;

    //Temporary structure to insert in instructionMap
    InstructionMap::instructionData tmpStruct = {
        ti,
        di,
        operation,
        tmpEmptyList,
        destReg,
        destRegPrt,
        srcReg1,
        srcReg1Ptr,
        srcReg2,
        srcReg2Ptr,
        isInLoopBody,
    };

    //Push the new instruction in instructionMap
    instructionMap.insert(std::pair<int*, InstructionMap::instructionData>(destReg, tmpStruct));

    //Push also in instruction vector
    std::pair<int*, std::string> tmpPair = {destReg, operation};
    instructionOrderVector.push_back(tmpPair);
}

///It inserts info about dest reg related pointer in the related instruction designated by destReg parameter
void InstructionMap::insertDestPtr(int* destReg, int* destRegPtr){

    //Find destReg
    instructionMapIT = instructionMap.find(destReg);

    //If found, insert related pointer
    if(instructionMapIT != instructionMap.end()){
        (instructionMapIT->second).destRegPrt = destRegPtr;
    } 
}

///It handles the insertion of new instructions that are not load with specs
void InstructionMap::insertInstructionWithSpecs(int ti, int di, std::string operation, std::list<std::string> specs, int * destReg,
                                                int * destRegPrt, int* srcReg1, int * srcReg1Ptr, int* srcReg2, int * srcReg2Ptr, bool isInLoopBody){

    //Temporary structure to insert in instructionMap
    InstructionMap::instructionData tmpStruct = {
        ti,
        di,
        operation,
        specs,
        destReg,
        destRegPrt,
        srcReg1,
        srcReg1Ptr,
        srcReg2,
        srcReg2Ptr,
        isInLoopBody,
    };

    //Push the new instruction in instructionMap
    instructionMap.insert(std::pair<int*, InstructionMap::instructionData>(destReg, tmpStruct));
}

///It handles the insertion of load instructions
void InstructionMap::insertLoadInstruction(int* destReg, CollectInfo CI){
    //Temporary empty list
    std::list<std::string> tmpEmptyList;

    //Temporary structure to insert in instructionMap
    InstructionMap::instructionData tmpStruct = {
        0,
        0,
        "load",
        tmpEmptyList,
        destReg,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        false,
    };

    

    /*bool found = false;
    std::map<int*, InstructionMap::instructionData>::iterator instructionMapIT2;
    for(instructionMapIT2 = instructionMap.begin(); instructionMapIT2 != instructionMap.end(); ++instructionMapIT2){

        if(CI.getAllocatedReg(instructionMapIT2->first) == destReg ||
         CI.getAllocatedReg((instructionMapIT2->second).destRegPrt) == destReg){

            found = true;
            
        }

    }*/

//&& found == false

    instructionMapIT = instructionMap.find(destReg);
    //If there is no load with the same operand, it can be scheduled
    if(instructionMapIT == instructionMap.end() ){

        //Push the new instruction in instructionMap
        errs() << "Inserting LOAD instruction " << destReg << "\n";

        //pushing into instruction map
        instructionMap.insert(std::pair<int*, InstructionMap::instructionData>(destReg, tmpStruct)); 

        //pushing into instruction vector
        std::pair<int*, std::string> tmpPair = {destReg, "load"};
        instructionOrderVector.push_back(tmpPair);

    }

    
}

///It adds specs to a specific instruction identified by destReg
void InstructionMap::addSpecToInstruction(int* destReg, std::string spec){

    //If destReg is found, push related specs
    instructionMapIT = instructionMap.find(destReg);

    if(instructionMapIT != instructionMap.end()){
        ((instructionMapIT->second).specifications).push_back(spec);
    }
}

///It returns the available time of the instructionw whose destReg is srcReg
int InstructionMap::getAvailableTime(int* srcReg){

    //If srcReg is found, return the sum of its ti and di
    instructionMapIT = instructionMap.find(srcReg);

    return (instructionMapIT->second).ti + (instructionMapIT->second).di;
}

///It handles the changing of the destReg of an instruction
int InstructionMap::insertNewInstructionDestReg(std::string operation, int * newDestReg, int * oldDestReg){

    instructionMapIT = instructionMap.find(oldDestReg);

    int ti = (instructionMapIT->second).ti;

    InstructionMap::instructionData dataStruct = instructionMapIT->second;
    dataStruct.destinationReg = newDestReg;    
    dataStruct.operation = operation;

    instructionMap.insert(instructionMapIT, std::pair<int*, InstructionMap::instructionData>(newDestReg, dataStruct));

    return ti;
}

///It reorders the vector containing all scheduled instructions
void InstructionMap::reorderInstructionVector(){

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
void InstructionMap::printInstructionMap(){
    errs() << "Printing scheduled instructions\n";

    int count = 0;

    for(instructionMapIT = instructionMap.begin(); instructionMapIT != instructionMap.end(); ++instructionMapIT){
        errs() << count << "\n";
        errs() << "Instruction with the following specs:\n";
        errs() << "\t operation: " << (instructionMapIT->second).operation;
        errs() << "\t ti: " << (instructionMapIT->second).ti;
        errs() << "\t di: " << (instructionMapIT->second).di;
        errs() << "\t destReg: " << (instructionMapIT->second).destinationReg;
        errs() << "\t ptrDestReg: " << (instructionMapIT->second).destRegPrt;
        errs() << "\t srcReg1: " << (instructionMapIT->second).sourceReg1;
        errs() << "\t ptrSrcReg1: " << (instructionMapIT->second).srcReg1Ptr;
        errs() << "\t srcReg2: " << (instructionMapIT->second).sourceReg2;
        errs() << "\t ptrSrcReg2: " << (instructionMapIT->second).srcReg2Ptr;
        errs() << "\t specifications: " << ((instructionMapIT->second).specifications).front();
        errs() << "\t is in a loop: " << (instructionMapIT->second).isInLoopBody;
        errs() << "\n\n";
        count++;
    }
}
//--------------------END DEGUB FUNCTION---------------------