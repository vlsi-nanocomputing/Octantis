/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LoopInfoTable Class: Class useful to collect information about loop nesting and loop-related variables
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef LOOPINFOTABLE_H
#define LOOPINFOTABLE_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"

using namespace llvm;

namespace octantis {

/// Class useful for the collection of information about loops
    class LoopInfoTable {

    public:

        ///Default constructor
        LoopInfoTable();

        ///Structure for storing the information of loops
        struct loopInfoStruct {
            int *loopHeader;
            std::list<int *> loopBody;
            int *loopLatch;

            int iteratorInitValue;
            int iteratorFinalValue;
            int iteratorIncrement;
            int iterations;

            int *variableInitValue;
            int *variableFinalValue;

            bool isInitValueConstant;
            bool isFinalValueConstant;

        };

        ///Function useful to insert a new loop (its iterator operand) in nestedLoopMap
        void insertNewLoopIt(int *iteratorOp);

        ///Function useful to insert a new item inside the list associated to a loop iterator in nestedLoopMap
        void pushNestedLoop(int *loopIt, int *higherLevelLoop);

        ///Function useful to insert infos about a loop
        void insertLoopInfo(int *iteratorOp, loopInfoStruct loopInfo);

        ///Function useful to retrieve infos about a loop
        loopInfoStruct getLoopInfo(int *iteratorOp);

        ///Function useful to retrieve the loop iterator given the loop body
        int *getLoopIteratorFromLoopBody(int *loopBody);

        ///Function useful to retrieve the pair <loopIt, list of higher level loops that contain it>
        std::pair<int *, std::list<int *>> getNestedLoops(int *iteratorOp);

        ///It returns the iteration vector, a list for a nested loop structure made up of all loops iterators in
        ///depth growing order, given the innermost loop iterator as input
        std::list<int *> getIterationVector(int *innermostLoopIterator);

        /*-----------------------------DEBUG FUNCTIONS-------------------------------*/
        void printNestedLoopMap();

        void printLoopInfoMap();
        /*-----------------------------DEBUG FUNCTIONS-------------------------------*/


    private:

        ///Map storing the infos about nested loops, with the first field being the operand of the iterator associated to a loop
        ///and the second operand the list of higher level loops that contain it
        /*
            loopIt1
            |
            |__ higherLevelLoop1, ..., topLevelLoop

            loopIt2
            |
            |__ higherLevelLoop1, ..., topLevelLoop

        */
        std::map<int *, std::list<int *>> nestedLoopMap;

        ///Iterator over nestedLoopMap
        std::map<int *, std::list<int *>>::iterator nestedLoopMapIT;

        ///Map in which the first field is the iterator operand and the second field
        ///contains info about the loop whose iterator operand is the one in the first field
        std::map<int *, loopInfoStruct> loopInfoMap;

        ///Iterator over loopInfoMap
        std::map<int *, loopInfoStruct>::iterator loopInfoMapIT;

    };

} //End of Octantis' namespace

#endif // LOOPINFOTABLE_H
