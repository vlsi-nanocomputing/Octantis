/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LiM Compiler Class: class useful for the generation of a LiM object.
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef LIMCOMPILER_H
#define LIMCOMPILER_H

#include "OperationsImplemented.h"

//Octantis Include Files
#include "InstructionTable.h"
#include "LiMArray.h"
#include "FiniteStateMachine.h"

//C++ Include files
#include <list>
#include <iterator>

//using namespace llvm;

namespace octantis{

class LiMCompiler
{
public:
    //Default constructor: it initializes the pointer to IT
    LiMCompiler(InstructionTable &ptrIT);

private:
    //Pointer to the Instruction List: backup value
    std::list<InstructionTable::instructionData> * ptrInstrList;

    //Iterator over the Instruction List
    std::list<InstructionTable::instructionData>::iterator instrListIT;

    //Variable for storing the first index of the addresses: this is useful
    //for the generation of the lable for the new rows introduced inside the array.
    int * zeroAddr;

    //Function to find the position of an element inside the InstructionList
    //(It may be moved inside the InstructionTable class!)
  //  std::list<InstructionTable::instructionData>::iterator getIteratorToElement(int * position, std::list<InstructionTable::instructionData> * ptrIL);


protected:
    LiMArray MemArray;
    FiniteStateMachine FSMLim;

//-------------------------DEBUG FUNCTIONS----------------------
private:
    void printLiMArray();
    void printFSM();

//---------------------END DEBUG FUNCTIONS----------------------

};

} //End Octantis namespace

#endif // LIMCOMPILER_H
