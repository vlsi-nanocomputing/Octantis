/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// PointerInfoTable Class: class useful to collect info about array-related pointers in the code
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef POINTERINFOTABLE_H
#define POINTERINFOTABLE_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"

#include "Utils/AccessPattern.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
    class PointerInfoTable {

    public:

        ///Default constructor
        PointerInfoTable();

        ///Structure useful for storing info about how an array index is calculated
        struct indexInfoStruct {
            std::list<int *> iterators;
            int constant;
            int operation;

            int operationConstant;
        };

        ///Structure for storing indexes and info about array being used as src or dest operand
        struct pointerInfoStruct {
            int *loopIterator;

            //Number of rows of the pointer array
            int arrayRows;
            //Number of cols of the pointer array
            int arrayCols;

            //Number of rows of a subset
            int setRows;
            //Number of rows of a subset
            int setCols;

            //Horizontal spacing between two subset elements
            int spacingInSubsetX;
            //Vertical spacing between two subset elements
            int spacingInSubsetY;
            //Horizontal spacing between two subsets
            int spacingX;
            //Vertical spacing between two subsets
            int spacingY;

            //Horizontal offset
            int offsetX;
            //Vertical offset
            int offsetY;
            //Horizontal stop
            int stopX;
            //Vertical stop
            int stopY;

            //Number of subsets in the pointer array
            int numberOfSubsets;
            //Number of elements in a subset
            int numberOfSubsetsElements;

            AccessPattern pointerAccessPattern;
            AccessPattern pointerAccessPatternConstant;

            indexInfoStruct firstIdxInfo;
            indexInfoStruct secondIdxInfo;
        };

        ///Function useful to return the infos about a specific pointer
        pointerInfoStruct getPointerInfo(int *pointer);

        ///Function useful to insert in PointerInfoMap a new element
        void insertPointerInfo(int *pointer, pointerInfoStruct PointerInfo);

        ///Function useful to modify the PointerInfo field related to an element in PointerInfoMap
        void modifyPointerInfo(int *pointer, pointerInfoStruct PointerInfo);

        ///It detects the array access pattern
        //std::pair<arrayAccessPattern, std::vector<int>> detectPointerAccessPattern(AccessPattern ptrAP);


        /*-----------------------------DEBUG FUNCTIONS-------------------------------*/
        void printPointerInfoTable();
        /*-----------------------------DEBUG FUNCTIONS-------------------------------*/

    private:

        ///Map where the key field refers to a pointer, the value field refers to the
        ///pointer-related information needed for the binding phase
        std::map<int *, pointerInfoStruct> pointerInfoMap;

        ///Iterator over PointerInfoMap
        std::map<int *, pointerInfoStruct>::iterator pointerInfoMapIT;

    };

} //End of Octantis' namespace

#endif // POINTERINFOTABLE_H
