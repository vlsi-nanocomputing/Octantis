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

///Default constructor
InstructionTable::InstructionTable()
{

}

//Initialization of the instructionList with a Operation
//InstructionTable::InstructionTable(int * exeTime, int * compTime, std::string * op, int * destReg, int * src1Reg, int * src2Reg)
//{
//        AddInstructionToList(exeTime, compTime, op, destReg, src1Reg, src2Reg);

//}

//Initialization of the instructionList with an Allocation
//InstructionTable::InstructionTable(int * exeTime, std::string * op, int * destReg, int * src1Reg)
//{
//        AddAllocaInstructionToList(exeTime, op, destReg, src1Reg);
//}

///Function useful to set the iterator to the beginning of the list
void InstructionTable::InitializeIterator() {
        IListIt = instructionList.begin();
}

///Function useful to return the current value of the iterator
int InstructionTable::GetIteratorValue() {
    //Test
    return 0;
}

///Function useful to put a new operation instruction into the instructionList
void InstructionTable::AddInstructionToList(int &allocTime, int &lastModifTime, std::string op, int * const destReg,
                                            int * const src1Reg, int * const src2Reg, int &loopFactor) {

        //Temporary empty list
        std::list<std::string> tmpEmptyList;
        //Temporary structure to link inside the list
        instructionData tmpStruct = {
            allocTime, //allocTime
            lastModifTime, //lastModifTime
            lastModifTime, //lastReadTime
            op, //operation
            tmpEmptyList, //No switch operation
            destReg, //destinationReg
            src1Reg, //sourceReg1
            src2Reg, //sourceReg2
            loopFactor //Number of loop iterations
        };

        //Push the new line inside the Instruction Table
        instructionList.push_back(tmpStruct);
}

///Function useful to put a new operation instruction into the instructionList with specifications
/// (e.g. input/output line and switch statement)
void InstructionTable::AddInstructionToListWithSpecs(int &allocTime, int &lastModifTime, std::string op,
                                                  std::list<std::string> &switchList, int* const destReg,
                                                  int* const src1Reg, int * const src2Reg, int &loopFactor){

    //Temporary structure to link inside the list
    instructionData tmpStruct = {
        allocTime, //allocTime
        lastModifTime, //lastModifTime
        lastModifTime, //lastReadTime
        op, //operation
        switchList, //Switch operators
        destReg, //destinationReg
        src1Reg, //sourceReg1
        src2Reg, //sourceReg2
        loopFactor //Number of loop iterations
    };

    //Push the new line inside the Instruction Table
    instructionList.push_back(tmpStruct);



}

///Function useful to put a new operation instruction into the instructionList in a specific position (identified another location "refPos")
void InstructionTable::AddInstructionToListAfterRefPos(int* const &refPos, int &allocTime, int &lastModifTime, std::string op,
                                                       int * const destReg, int * const src1Reg, int * const src2Reg, int &loopFactor){

        //Temporary empty list
        std::list<std::string> tmpEmptyList;
        //Temporary structure to link inside the list
        instructionData tmpStruct = {
            allocTime, //allocTime
            lastModifTime, //lastModifTime
            lastModifTime, //lastReadTime
            op, //operation
            tmpEmptyList,
            destReg, //destinationReg
            src1Reg, //sourceReg1
            src2Reg, //sourceReg2
            loopFactor //Number of loop iterations
        };

        IListIt=getIteratorToElement(refPos);

        //Select the subsequent position (after refPos)
        std::advance(IListIt,1);

        //Push the new line inside the Instruction Table
        instructionList.insert(IListIt,tmpStruct);
}

///Function useful to put a new alloca instruction into the instructionList
void InstructionTable::AddAllocaInstructionToList(int &allocTime, int* const destReg, int &arrayDim) {

        //Temporary vector to store the validity bit
        std::vector<bool> tmpValidityVector;

        for(int i=0; i<arrayDim;++i)
            tmpValidityVector.push_back(true); //valid - Initialization Value


        //Temporary structure to link inside the map.
        //Validity bit initialized to 'true'
        allocatedData tmpStruct = {
            allocTime, //allocTime
            tmpValidityVector
        };

       // errs() << "Data in the structure: " << tmpStruct.executionTime << " " << tmpStruct.operation << " " << tmpStruct.destinationReg << " " << tmpStruct.sourceReg1 << "\n";

        //Push the new line inside the allocMap
        allocMap.insert({destReg, tmpStruct});
}

///Function to add shift blocks inside an existing row
/// NOTEs: Warning, here we lose important timing information!
///        Problem to solve in future updates!
void InstructionTable::AddSpecToList(int * const &refPos, std::string op){

    //Check if the source row exists
    std::list<instructionData>::iterator internalIT;

    internalIT=getIteratorToElement(refPos);

    if(internalIT!=instructionList.end())
    {
        //Check if the shift operand is already present
        std::list<std::string>::iterator specsIT=find((internalIT->specifications).begin(),
                                                      (internalIT->specifications).end(),
                                                      op);
        if(specsIT==(internalIT->specifications).end())
        {
            (internalIT->specifications).push_back(op);
        }

    } else {
        //An error occurred
        llvm_unreachable("Error in InstructionTable: the row that has to be modified does not exist!");
    }
}


///Function to change the kind of operation of an instruction and
/// change the destination register of an operation
void InstructionTable::ChangeOperatorAndDestReg(int * const srcLocation, std::string newOperator, int * const newSrcLocation){

    std::list<instructionData>::iterator internalIT;

    //Typically the instruction is modified immediately after its introduction
    internalIT=instructionList.end();
    --internalIT;

    if((internalIT->destinationReg)==srcLocation)
    {

        (internalIT->operation)=newOperator;
        (internalIT->destinationReg)=newSrcLocation;

    } else {
        //Advanced research of the corret location
        for(internalIT=instructionList.begin();internalIT!=instructionList.end();++internalIT)
        {
            if((internalIT->destinationReg)==srcLocation)
            {
                (internalIT->operation)=newOperator;
                (internalIT->destinationReg)=newSrcLocation;
                break;
            }
        }
        if(internalIT==instructionList.end())
        {
            //An error occurred
            llvm_unreachable("Error in finding an instruction inside the InstructionList.\n");
        }

    }
}


///Function useful to remove an element from the list
void InstructionTable::RemoveInstructionFromList(int * const &rowName) {

    //Check if the source row exists
    std::list<instructionData>::iterator internalIT;

    internalIT=getIteratorToElement(rowName);

    if(internalIT!=instructionList.end())
    {
        instructionList.erase(internalIT);

    } else {
        //An error occurred
        llvm_unreachable("Error in InstructionTable: the row that has to be deleted does not exist!");
    }
}

///Funtion useful to add a new control signal to the list
void InstructionTable::addControlSignal(int* & controlSig){
    controlSignals.push_back(controlSig);
}

///Funtion to get the parent of an operand: the location of the allocated
///data on the stack. It returns a null pointer if the parent has not
///been modified after the load instruction (RAW conflict).
bool InstructionTable::isParentValid(int* const &srcReg, int &index){

    MapIt=allocMap.find(srcReg);
    if (MapIt != allocMap.end())
    {
        return ((allocMap[srcReg]).valid[index]) ? true : false;
    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
                         " inside the instruction table");
    }

}


///Function to invalidate the information stored inside the parent location
void InstructionTable::invalidateParent(int* const &parent, int &index){

    MapIt=allocMap.find(parent);
    if (MapIt != allocMap.end())
    {
        allocMap[parent].valid[index]=false;
    } else {
        //An error occurred: the parent register, the allocated one, is not present in IT
        llvm_unreachable("InstructionTable error: current instruction refers to a source register not present"
                         " inside the instruction table");
    }

}

///Function to get the time in which the source information is available
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

///Function to get the iterator of a specific entry of the Instruction Table
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

///Function to get the first operand of an instruction
int * InstructionTable::getFirstOperand(int * const &srcReg){

    //Check if the source row exists
    std::list<instructionData>::iterator internalIT;

    internalIT=getIteratorToElement(srcReg);

    return(internalIT->sourceReg1);
}

///Function to get the type of an intruction
std::string InstructionTable::getType(int * const &srcReg){

    auto entryMatching = getIteratorToElement(srcReg);
    return entryMatching->operation;

}


/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

void InstructionTable::printIT(){

    int lineCount=0;

    errs()<< "-----------------------------DEBUG MODE-----------------------------\n";

    for (IListIt = instructionList.begin(); IListIt != instructionList.end(); ++IListIt)
    {
        errs()<< "\t\t Line " << lineCount << ": ";
        errs()<< IListIt->allocTime << " ";
        errs()<< IListIt->lastModifTime << " ";
        errs()<< IListIt->lastReadTime << " ";
        errs()<< IListIt->operation;
        errs()<< "; Associated operators: ";
        if(!(IListIt->specifications).empty()){
            errs() << "NOT EMPTY. ";
        } else{
            errs() << "EMPTY. ";
        }
        errs()<< IListIt->destinationReg << " ";
        errs()<< IListIt->sourceReg1 << " ";
        errs()<< IListIt->sourceReg2 << " ";
        errs()<< IListIt->loopFactor << "\n";

        lineCount++;
    }

    errs()<< "\n\n";

}

void InstructionTable::printAllocData(){

    int lineCount=0;
    int validCount=0;

    errs()<< "An error occurred, the state of the Allocated Data Map will be printed:\n\n";

    for (MapIt = allocMap.begin(); MapIt != allocMap.end(); ++MapIt)
    {
        errs()<< "\t Line " << lineCount << ": ";
        errs()<< "allocReg: " << MapIt->first << ", ";
        errs()<< "allocTime: " << MapIt->second.allocTime << ". Validity:\n";
        for (auto i = (MapIt->second.valid).begin(); i != (MapIt->second.valid).end(); ++i)
        {
            errs()<< "\t\tvalidityBit" << validCount << ": " << *i << "\n";
            ++validCount;
        }

        validCount=0;
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
