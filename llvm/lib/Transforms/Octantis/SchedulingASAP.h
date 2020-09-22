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
#include "AdditionalLogicPorts.h"
//#include "Allocator.h" //The allocator will introduce constraints considering the config. files

#include <map>
#include <iterator>
#include <string>

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
        ptr,
        swi, //Switch instruction
        branch,
        icmp,
        sext, //Sign extension
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

    ///Function to parse Loops information
    void parseLoopInfo(BasicBlock &BB);

    ///Function to know if a basic block is the last one belonging to a loop
    bool isTheLastBBInLoop(BasicBlock &BB);

    ///Function to terminate the scheduling of loop instructions
    void endOfCurrentLoop();


/*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    void printAliasMap();

/*--------------------------END DEBUG FUNCTIONS------------------------------*/

private:
    ///Function to identify the not instruction
    bool isThisNot(Instruction &I);

    ///Fuction to change the parent type in negative logic and to update the correct destReg
    void changeParentInNOT(int * const parentName, std::string operation, int * const newParentName);

private:
    ///Object useful for defining the modified version of a
    /// specific logic operator
    AdditionalLogicPorts changeLogic;

private:
    /// Structure useful to store tmp information
    /// about the index of the array parsed
    struct indexStruct{
        int * ptrName;
        int * srcReg;
        int index;
        bool valid=false; //A getElementPtr instruction is parsed
    } infoAboutPtr;

    /// Structure useful to store tmp information
    /// about switch statements
    struct switchStruct{
        int * destReg=nullptr;
        int * srcReg1=nullptr;
        int * srcReg2=nullptr;
        int numOfCases;
        std::list<std::string> operators; //The order is important for the
                                          //ident. of MUX position.
        bool valid=false; //Useful to check if it is the first case of a switch
    };

private:

    ///Structure for storing the information of loops (if any)
    struct loopInfoStruct{
        int * loopHeader;
        int * loopBody;
        int * loopTerm;
        int iterations;
        bool valid=false;
    }loopInfo;

private:
    /// List of the invalid basic block (useful after analyzing the
    /// switch cases)
    std::list<int *> invalidBB;

    /// Iterator over the invalidBB list
    std::list<int *>::iterator invalidBBIT;

public:
    /// Fuction useful to define if a basic block is valid (switch
    /// cases)
    bool isBBValid(BasicBlock &bb);

    /// Function useful to set a basic block as not valid (loop
    /// case)
    void setBBAsNotValid(BasicBlock &bb);


public:

    InstructionTable IT;

private:

    /// Variable useful to store the current execution time
    int Timer=0;

    /// Map for storing the aliases of each loaded register
    std::map<int * const, int * const> aliasMap;

    /// Iterator over aliases map
    std::map<int * const, int * const>::iterator aliasMapIT;

    /// Map useful for keeping track of the variables usefulf
    /// to implement iterators (like in loops)
    std::map<int * const, int> itVariablesMap;

    /// Iterator over the itVariablesMap
    std::map<int * const, int>::iterator itVariablesMapIT;



    /// Map for storing the temporary index for accessing
    /// an array
    //std::map<int * const, int> arrayIndexList;

    /// Iterator over arrayIndexList
    //std::map<int * const, int>::iterator arrayIndexListIT;

};

} //End of Octantis' namespace

#endif // SCHEDULINGASAP_H
