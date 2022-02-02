/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// CollectInfo Class: Class useful for the collection of information that will be exploited during the 
//                      later phases of Octantis, such as scheduling and binding
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef COLLECTINFO_H
#define COLLECTINFO_H

#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"

#include "PointerInfoTable.h"
#include "LoopInfoTable.h"


#include <map>
#include <iterator>
#include <string>
#include <list>

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
class CollectInfo
{

public:

    ///Default constructor
    CollectInfo();

    /// Recognized instructions
    enum Instr{
        alloc, //Alloca is a keyword
        load,
        store,
        binary, //Also shift right/left
        ret,
        ptr,
        swi, //Switch instruction
        branch,
        icmp,
        sext, //Sign extension
        unknown
    };

    ///Function devoted to the collection of information regarding loops
    void collectLoopInfo(LoopInfo &LI);

    ///Function useful to parse alloca instructions
    void parseAllocaInstructions(BasicBlock* BB);

    ///Function useful to parse loop preheader in order to detect iterator initialization
    void parseLoopPreheader(BasicBlock* &BB);

    ///Function to parse Loops information
    int* parseLoopHeaderAndLatch(BasicBlock* &LH, BasicBlock* &LL);

    ///Function to parse information contained in a Loop Latch
    void parseLoopLatchInfo(BasicBlock* &BB);

    ///Function to parse useful information inside a loop's body
    void parseLoopBody(BasicBlock &BB);

    ///Function that parse, collecting informations, the given Function in order to pass only the essential info to the scheduler
    void parseFunction(Function &F, LoopInfo &LI);

    ///Function useful to know if the valid BB instruction must be considered for scheduling
    bool isValidInst(Instruction &I);

    ///Function useful to know if the current BB is a loop's body
    bool isLoopBody(const BasicBlock* BB);

    ///Function that return the type of the instruction passed
    Instr identifyInstr(Instruction &I);

    ///Function useful to set useful parameters for the binding phase concerning pointers and their indexes
    void setPointerInfo(int* pointer, Instruction* I);

public:

    ///Function useful to return aliasInfoMap
    std::map<int*, std::list<int*>> * getAliasInfo();

    ///Function useful to check the operand refers to an array
    bool isArray(int* operand);

    ///Function useful to retrieve the list of BB effectively useful for scheduling
    std::list<BasicBlock*> getValidBBs();

    ///Function useful to retrieved the allocated register related to the input alias operand
    int * getAllocatedReg(int *op);

    ///Function useful to get the size of the array/matrix given in input the allocated reg
    int getArraySize(int *reg);

    ///Function useful to get the number of columns of the array
    int getArrayCols(int *reg);

    ///Function useful to get the number of rows of the array
    int getArrayRows(int *reg);

    /*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    void printAliasInfoMap();

    void printArrayInfoMap();

    void printLoopIteratorMap();

    void printIteratorsAliasMap();

    /*--------------------------END DEBUG FUNCTIONS------------------------------*/
    

    PointerInfoTable PIT;
    LoopInfoTable LIT;

private:

    ///Map useful to take note of the iterators' aliases
    std::map<int*, int*> iteratorsAliasMap;

    ///Iterator over iteratorsAliasMap
    std::map<int*, int*>::iterator iteratorsAliasMapIT;

    ///Map useful to keep track of allocated register aliases
    std::map<int*, std::list<int*>> aliasInfoMap;

    ///Iterator over aliasInfoMap
    std::map<int*, std::list<int*>>::iterator aliasInfoMapIT;

    ///Map useful to know info about loop iterators' initial values
    std::map<int*, int> initValuesMap;

    ///Iterator over the iteratorsInfoMap map
    std::map<int*, int>::iterator initValuesMapIT;

    //list of BBs to be passed to the scheduler
    std::list<BasicBlock*> validBBs;

    bool ptrValid = false;
    int* tmpPointerArray;

    ///Map containing information about arrays present in the function
    std::map<int*, std::list<int>> arraysInfoMap;

    ///Iterator over arraysInfoMap
    std::map<int*, std::list<int>>::iterator arraysInfoMapIT;

    ///Map pairing each loop with its iterator operand
    std::map<Loop*, int *> loopIteratorMap;

    ///List containing all the Loop Preheaders
    std::list<int*> loopPreheaderList;

    ///List containing all the Loop Headers
    std::list<int*> loopHeaderList;

    ///List containing all the Loop Latches
    std::list<int*> loopLatchList;

    ///List containing all the Loop Bodies
    std::list<int*> loopBodyList;



    ///Map useful to keep track of operations between iterators
    std::map<int*, PointerInfoTable::indexInfoStruct> combinedIteratorsMap;

    ///Iterator over combinedIteratorsMap
    std::map<int*, PointerInfoTable::indexInfoStruct>::iterator combinedIteratorsMapIT;

    PointerInfoTable::pointerInfoStruct tmpPointerStruct;
    int* tmpVariableInitIterator;


    std::list<int*> combinedIterators;

};

} //End of Octantis' namespace

#endif // COLLECTINFO_H
