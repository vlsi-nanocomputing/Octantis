/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// InstructionTable Class: It is useful to contain all scheduled instructions to be then mapped onto the LiM structure
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
#include "Analysis/CollectInfo.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
    class InstructionTable {

    public:

        enum operandType {

            singleVariable,
            temporaryVariable,
            array,
            constant,
            fakeArray,
            undefined

        };

        ///Useful data for a scheduled instruction
        struct instructionData {
            int ti;
            int di;

            std::string operation;
            std::list<std::string> specifications;

            int *destinationReg;
            operandType destRegType;
            int *destRegPrt;
            int *sourceReg1;
            operandType srcReg1Type;
            int *srcReg1Ptr;
            int *sourceReg2;
            operandType srcReg2Type;
            int *srcReg2Ptr;

            bool isInLoopBody;
        };

        ///Default constructor
        InstructionTable();


        ///It handles the insertion of load instructions
        void insertLoadInstruction(int *destReg, operandType oT);

        ///It handles the insertion of load instructions
        void modifyLoadInstruction(int *destReg, int *newDestReg, int *newDestRegPtr);


        ///It handles the insertion of new instructions that are not load
        void insertInstruction(int ti, int di, std::string operation, int *destReg, operandType destRegType,
                               int *destRegPrt, int *srcReg1, operandType srcReg1Type, int *srcReg1Ptr, int *srcReg2,
                               operandType srcReg2Type,
                               int *srcReg2Ptr, bool isInLoopBody);

        ///It handles the insertion of new instructions that are not load with specs
        void
        insertInstructionWithSpecs(int ti, int di, std::string operation, std::list<std::string> specs, int *destReg,
                                   operandType destRegType,
                                   int *destRegPrt, int *srcReg1, operandType srcReg1Type, int *srcReg1Ptr,
                                   int *srcReg2, operandType srcReg2Type,
                                   int *srcReg2Ptr, bool isInLoopBody);

        ///It adds specs to a specific instruction identified by destReg
        void addSpecToInstruction(int *destReg, std::string spec);


        ///It inserts info about dest reg related pointer in the related instruction designated by destReg parameter
        void insertDestPtr(int *destReg, int *destRegPtr);

        ///It handles the changing of the destReg of an instruction
        int insertNewInstructionDestReg(std::string operation, int *newDestReg, int *oldDestReg);

        ///It changes the operation of a given instruction
        void changeOperationOfInstruction(int *destReg, std::string newOperation);

        ///Swaps the operands of the instruction
        void swapOperands(int *destReg);

        ///Modify the given field of the instruction
        void modifyGivenField(int *destReg, const std::string &field, int *newField, operandType oT);


        ///It returns the available time of the instructionw whose destReg is srcReg
        int getAvailableTime(int *srcReg);

        ///It sets the init time and delay for the operation
        void setInstructionTimeAndDelay(int *destReg, int time, int delay);

        ///It returns the Instruction Map
        std::map<int *, instructionData> getInstructionMap();

        ///It reorders the vector containing all scheduled instructions
        void reorderInstructionVector();

        /*-----------------------------DEBUG FUNCTIONS-------------------------------*/

        void printInstructionMap();

        /*--------------------------END DEBUG FUNCTIONS------------------------------*/

    public:

        ///Map containing all scheduled instructions
        std::map<int *, instructionData> instructionMap;
        std::map<int *, instructionData>::iterator instructionMapIT;

        //Vector containing pairs of destReg(of scheduled instructions) - related operation
        std::vector<std::pair<int *, std::string>> instructionOrderVector;

    };

} //End of Octantis' namespace

#endif // INSTRUCTIONMAP_H
