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

#include "AccessPattern.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information
class PointerInfoTable
{

public:

    ///Default constructor
    PointerInfoTable();

    ///enum type for array access pattern
    enum arrayAccessPattern{
        rowMajorPosColPos,
        rowMajorNegColPos,
        rowMajorPosColNeg,
        rowMajorNegColNeg,

        colMajorPosRowPos,
        colMajorNegRowPos,
        colMajorPosRowNeg,
        colMajorNegRowNeg,

        rowMajorPosColPosDiag,
        rowMajorNegColPosDiag,
        rowMajorPosColNegDiag,
        rowMajorNegColNegDiag,

        colMajorPosRowPosDiag,
        colMajorNegRowPosDiag,
        colMajorPosRowNegDiag,
        colMajorNegRowNegDiag,

        undefined,
    };

    ///Structure useful for storing info about how an array index is calculated
    struct indexInfoStruct{
        std::list<int*> iterators;
        int constant;
        int operation;

        int operationConstant;
    };

    ///Structure for storing indexes and info about array being used as src or dest operand
    struct pointerInfoStruct{
        int * loopIterator;

        int numberOfSets;
        int setRows;
        int setCols;

        bool rowFirst;
        bool colFirst;
        bool rowFirstSet;
        bool colFirstSet;

        AccessPattern pointerAccessPattern;
        AccessPattern pointerAccessPatternConstant;
        
        indexInfoStruct firstIdxInfo;
        indexInfoStruct secondIdxInfo;
    };

    ///Function useful to return the infos about a specific pointer
    pointerInfoStruct getPointerInfo(int * pointer);

    ///Function useful to insert in PointerInfoMap a new element
    void insertPointerInfo(int * pointer, pointerInfoStruct PointerInfo);

    ///Function useful to modify the PointerInfo field related to an element in PointerInfoMap
    void modifyPointerInfo(int * pointer, pointerInfoStruct PointerInfo);

    ///It detects the array access pattern
    arrayAccessPattern detectPointerAccessPattern(AccessPattern pointerAccessPattern);


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
