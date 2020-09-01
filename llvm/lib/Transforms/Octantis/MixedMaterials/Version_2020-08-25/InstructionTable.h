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
#ifndef INSTRUCTIONTABLE_H
#define INSTRUCTIONTABLE_H

#include <string>
#include <list>
#include <map>
#include <cstdlib>

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace octantis{

class InstructionTable
{
public:
    //Initialization of the instructionList with a Operation
    //InstructionTable(int * exeTime, int * compTime, std::string * op, int * destReg, int * src1Reg, int * src2Reg);

    //Initialization of the instructionList with an Allocation
    //InstructionTable(int * exeTime, std::string * op, int * destReg, int * src1Reg);

    //Default constructor
    InstructionTable();

    //Function useful to set the iterator to the beginning of the list
    void InitializeIterator();

    //Function useful to return the current value of the iterator
    int GetIteratorValue();

    //Function useful to put a new operation instruction into the instructionList
    void AddInstructionToList(int * allocTime, int * lastModifTime, std::string op, int * destReg, int * src1Reg, int * src2Reg);

    //Function useful to put a new operation instruction into the instructionList in a specific position (identified another location "refPos")
    void AddInstructionToListAfterRefPos(int *refPos, int * allocTime, int * lastModifTime, std::string op, int * destReg, int * src1Reg, int * src2Reg);

    //Function useful to put a new alloca instruction into the instructionList
    //Here the src1Reg is the name of the Alias of the allocated data: the load
    //  instruction is performed copying the allocated data into a new SSA register.
    void AddAllocaInstructionToList(int * allocTime, int * destReg);

    //Function useful to remove an element from the list
    void RemoveInstructionFromList(int * position);

    //Funtion to get the parent of an operand: the location of the allocated
    //data on the stack.
    bool isParentValid(int * srcReg);

    //Function to invalidate the information stored inside the parent location
    void invalidateParent(int * parent);

    //Function to get the time in which the source information is available
    int getAvailableTime(int * srcReg);

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    void printIT();

    void printAllocData();


/*--------------------------END DEBUG FUNCTIONS------------------------------*/


    //Destructor
    ~InstructionTable();

//This has to be transformed into protected!
public:
    //Structure useful to store the information related to each LLVM instruction
    struct instructionData{
        int allocTime;
        int lastModifTime;
        int lastReadTime;
        std::string operation;
        int * destinationReg;
        int * sourceReg1;
        int * sourceReg2;
    };

    //Allocated data: they are typically inside the stack, so outside the memory.
    //The struct is useful to support the identification if one of these data has
    //been modified.
    struct allocatedData{
        int allocTime;
        bool valid;
    };


public:
    //Function to get the iterator of a specific entry of the Instruction Table
    std::list<instructionData>::iterator getIteratorToElement(int * position);

//This has to be transformed into protected!
public:
    //List containing all the instructions that have to be scheduled
    std::list<instructionData> instructionList;

    //Map containing the info about the alloc data
    std::map<int *, allocatedData> allocMap;

private:
    //Iterator to access the list
    std::list<instructionData>::iterator IListIt;

    //Iterator to access the map
    std::map<int *, allocatedData>::iterator MapIt;

};

} // End of Octantis namespace

#endif // INSTRUCTIONTABLE_H
