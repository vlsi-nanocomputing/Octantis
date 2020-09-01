/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// Instruction Table Class: useful to store the instructions that will be scheduled on the LiM
//                          architecture.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "InstructionTable.h"

// Standard C++ Include Libraries
#include <algorithm>

using namespace llvm;
using namespace octantis;

//Default constructor
InstructionTable::InstructionTable()
{

}

////Initialization of the instructionList with a Operation
//InstructionTable::InstructionTable(int * exeTime, int * compTime, std::string * op, int * destReg, int * src1Reg, int * src2Reg)
//{
//        AddInstructionToList(exeTime, compTime, op, destReg, src1Reg, src2Reg);

//}

////Initialization of the instructionList with an Allocation
//InstructionTable::InstructionTable(int * exeTime, std::string * op, int * destReg, int * src1Reg)
//{
//        AddAllocaInstructionToList(exeTime, op, destReg, src1Reg);
//}

//Function useful to set the iterator to the beginning of the list
void InstructionTable::InitializeIterator() {
        IListIt = instructionList.begin();
}

//Function useful to return the current value of the iterator
int InstructionTable::GetIteratorValue() {
    //Test
    return 0;
}

//Function useful to put a new operation instruction into the instructionList
void InstructionTable::AddInstructionToList(int &allocTime, int &lastModifTime, std::string op, int * const destReg,
                                            int * const src1Reg, int * const src2Reg) {

        //Temporary structure to link inside the list
        instructionData tmpStruct = {
            allocTime, //allocTime
            lastModifTime, //lastModifTime
            lastModifTime, //lastReadTime
            op, //operation
            destReg, //destinationReg
            src1Reg, //sourceReg1
            src2Reg //sourceReg2
        };

        //Push the new line inside the Instruction Table
        instructionList.push_back(tmpStruct);
}

//Function useful to put a new operation instruction into the instructionList in a specific position (identified another location "refPos")
void InstructionTable::AddInstructionToListAfterRefPos(int* const &refPos, int &allocTime, int &lastModifTime, std::string op,
                                                       int * const destReg, int * const src1Reg, int * const src2Reg){

        //Temporary structure to link inside the list
        instructionData tmpStruct = {
            allocTime, //allocTime
            lastModifTime, //lastModifTime
            lastModifTime, //lastReadTime
            op, //operation
            destReg, //destinationReg
            src1Reg, //sourceReg1
            src2Reg //sourceReg2
        };

        IListIt=getIteratorToElement(refPos);

        //Select the subsequent position (after refPos)
        std::advance(IListIt,1);

        //Push the new line inside the Instruction Table
        instructionList.insert(IListIt,tmpStruct);
}

//Function useful to put a new alloca instruction into the instructionList
void InstructionTable::AddAllocaInstructionToList(int &allocTime, int* const destReg) {

        //Temporary structure to link inside the map.
        //Validity bit initialized to 'true'
        allocatedData tmpStruct = {
            allocTime, //allocTime
            true //valid - Initialization Value
        };

       // errs() << "Data in the structure: " << tmpStruct.executionTime << " " << tmpStruct.operation << " " << tmpStruct.destinationReg << " " << tmpStruct.sourceReg1 << "\n";

        //Push the new line inside the allocMap
        allocMap.insert({destReg, tmpStruct});
}

//Function useful to remove an element from the list
void InstructionTable::RemoveInstructionFromList(int &position) {

        //Check if the position is correct
        if (true/* *position < instructionList.size()*/) {
                InitializeIterator();
                std::advance(IListIt,position);
                IListIt=instructionList.erase(IListIt);
        }
        else {
                //An error occurred
                llvm_unreachable("Error in removing instructions inside the InstructionList.\n");
        }
}

//Funtion to get the parent of an operand: the location of the allocated
//data on the stack. It returns a null pointer if the parent has not
//been modified after the load instruction (RAW conflict).
bool InstructionTable::isParentValid(int* const &srcReg){

    MapIt=allocMap.find(srcReg);
    if (MapIt != allocMap.end())
    {
        return (allocMap[srcReg].valid ) ? true : false;
    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
                         " inside the instruction table");
    }

}


//Function to invalidate the information stored inside the parent location
void InstructionTable::invalidateParent(int* const &parent){

    MapIt=allocMap.find(parent);
    if (MapIt != allocMap.end())
    {
        allocMap[parent].valid=false;
    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
                         " inside the instruction table");
    }

}

//Function to get the time in which the source information is available
int InstructionTable::getAvailableTime(int* const &srcReg){

    auto entryMatching = getIteratorToElement(srcReg);
    return entryMatching->lastModifTime;

//    auto entryMatching = std::find_if(instructionList.cbegin(), instructionList.cend(), [srcReg] (const instructionData& iD) {
//      return iD.destinationReg == srcReg;
//    });

//    if (entryMatching != instructionList.cend()) {

//        return entryMatching->lastModifTime;

//    } else {
//        //An error occurred: the parent register, the allocated one, is not present in IT
//        llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
//                         " inside the instruction table");
//    }
}

//Function to get the iterator of a specific entry of the Instruction Table
std::list<InstructionTable::instructionData>::iterator InstructionTable::getIteratorToElement(int* const &position){

    std::list<InstructionTable::instructionData>::iterator entryMatchingPos;
    entryMatchingPos=std::find_if(instructionList.begin(), instructionList.end(), [position] (const instructionData& iD) {
      return iD.destinationReg == position;
    });

    if (entryMatchingPos != instructionList.cend()) {

        return entryMatchingPos;

    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction is not present"
                         " inside the instruction table");
    }

}


/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

void InstructionTable::printIT(){

    int lineCount=0;

    errs()<< "-----------------------------DEBUG MODE-----------------------------\n";
    errs()<< "An error occurred, the state of the Instruction table will be printed:\n\n";

    for (IListIt = instructionList.begin(); IListIt != instructionList.end(); ++IListIt)
    {
        errs()<< "\t\t Line " << lineCount << ": ";
        errs()<< IListIt->allocTime << " ";
        errs()<< IListIt->lastModifTime << " ";
        errs()<< IListIt->lastReadTime << " ";
        errs()<< IListIt->operation << " ";
        errs()<< IListIt->destinationReg << " ";
        errs()<< IListIt->sourceReg1 << " ";
        errs()<< IListIt->sourceReg2 << "\n";

        lineCount++;
    }

    errs()<< "\n\n";

}

void InstructionTable::printAllocData(){

    int lineCount=0;

    errs()<< "An error occurred, the state of the Allocated Data Map will be printed:\n\n";

    for (MapIt = allocMap.begin(); MapIt != allocMap.end(); ++MapIt)
    {
        errs()<< "\t\t Line " << lineCount << ": ";
        errs()<< "allocReg: " << MapIt->first << ", ";
        errs()<< "allocTime: " << MapIt->second.allocTime << ", ";
        errs()<< "validityBit: " << MapIt->second.valid << "\n";

        lineCount++;
    }

    errs()<< "\n\n";

}
/*--------------------------END DEBUG FUNCTIONS------------------------------*/


//Interesting functions:

//auto entryMatching = std::find_if(instructionList.cbegin(), instructionList.cend(), [srcReg] (const instructionData& iD) {
//  return iD.destinationReg == srcReg;
//});

//if (entryMatching != instructionList.cend()) {
//    if ((entryMatching->lastModifTime) <= *execTime)
//    {
//        //The relation is valid
//        return (int *) entryMatching->destinationReg;
//    } else {
//        return nullptr;
//    }

//} else {
//    //An error occurred: the parent register, the allocated one, is not present in IT
//    llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
//                     " inside the instruction table");
//}
