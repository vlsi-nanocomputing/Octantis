/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// InstructionMap Class: It is useful to contain all scheduled instructions to be then mapped onto the LiM structure
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef INSTRUCTIONMAP_H
#define INSTRUCTIONMAP_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"
#include "CollectInfo.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
class InstructionMap
{

public:

    ///Useful data for a scheduled instruction
    struct instructionData{
        int ti;
        int di;

        std::string operation;        
        std::list<std::string> specifications;

        int * destinationReg;
        int * destRegPrt;
        int * sourceReg1;
        int * srcReg1Ptr;
        int * sourceReg2;
        int * srcReg2Ptr;

        bool isInLoopBody;
    };

    ///Default constructor
    InstructionMap();

    ///It handles the insertion of load instructions
    void insertLoadInstruction(int* destReg, CollectInfo CI);

    ///It inserts info about dest reg related pointer in the related instruction designated by destReg parameter
    void insertDestPtr(int* destReg, int* destRegPtr);

    ///It handles the insertion of new instructions that are not load
    void insertInstruction(int ti, int di, std::string operation, int* destReg, int * destRegPrt, int* srcReg1, int * srcReg1Ptr, int* srcReg2, int * srcReg2Ptr, bool isInLoopBody);

    ///It handles the insertion of new instructions that are not load with specs    
    void insertInstructionWithSpecs(int ti, int di, std::string operation, std::list<std::string> specs, int* destReg, int * destRegPrt, int* srcReg1, int * srcReg1Ptr, int* srcReg2, int * srcReg2Ptr, bool isInLoopBody);

    ///It adds specs to a specific instruction identified by destReg
    void addSpecToInstruction(int* destReg, std::string spec);

    ///It returns the available time of the instructionw whose destReg is srcReg
    int getAvailableTime(int* srcReg);

    ///It handles the changing of the destReg of an instruction
    int insertNewInstructionDestReg(std::string operation, int * newDestReg, int * oldDestReg);

    ///It reorders the vector containing all scheduled instructions
    void reorderInstructionVector();

    /*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    void printInstructionMap();

    /*--------------------------END DEBUG FUNCTIONS------------------------------*/

public:

    ///Map containing all scheduled instructions
    std::map<int*, instructionData> instructionMap;
    std::map<int*, instructionData>::iterator instructionMapIT;

    //Vector containing pairs of destReg(of scheduled instructions) - related operation
    std::vector<std::pair<int*, std::string>> instructionOrderVector;
    
};

} //End of Octantis' namespace

#endif // INSTRUCTIONMAP_H
