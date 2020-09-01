/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// SchedulingASAP Class: Implementation of the ASAP algorithm for the scheduling of the operations
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef SCHEDULINGASAP_H
#define SCHEDULINGASAP_H

#include "llvm/IR/Function.h"

#include "InstructionTable.h"
//#include "Allocator.h" //The allocator will introduce constraints considering the config. files

#include <map>
#include <iterator>

using namespace llvm;

namespace octantis {

/// Class useful for the implementation of the ASAP Scheduling algorithm
class SchedulingASAP
{
public:
    /// Recognized instructions
    enum Instr{
        alloc, //Alloca is a keyword
        load,
        store,
        binary,
        ret,
        unknown
    };

    ///Default constructor
    SchedulingASAP();

    ///Constructor useful to initialize the Instruction Table
    SchedulingASAP(Instruction &I);

    ///Function useful to add a new LLVM IR instruction to the Intruction Table
    void addNewInstruction(Instruction &I);

    ///Function useful to increment the value of the scheduling timer
    void incrementTimer();

    ///Function useful to decrement the value of the scheduling timer
    void decrementTimer();

    ///Function that returns the pointer to the complete Instruction Table
    InstructionTable * getInstructionTable();

    ///Function that return the type of the instruction passed
    Instr identifyInstr(Instruction &I);

    ///Function to get the actual source register from which execute the load
    int * getRealParent(int* &aliasParent);

    ///Function to return the pointer to the Instruction Table
    InstructionTable & getIT();


/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    void printAliasMap();

/*--------------------------END DEBUG FUNCTIONS------------------------------*/


public:

    InstructionTable IT;

private:

    /// Variable useful to store the current execution time
    int Timer=0;

    /// Map for storing the aliases of each loaded register
    std::map<int * const, int * const> aliasMap;

    /// Iterator over aliases map
    std::map<int * const, int * const>::iterator aliasMapIT;
};

} //End of Octantis' namespace

#endif // SCHEDULINGASAP_H
