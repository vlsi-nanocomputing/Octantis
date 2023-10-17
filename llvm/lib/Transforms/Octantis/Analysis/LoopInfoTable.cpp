/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// LoopInfoTable Class: Class useful to collect information about loop nesting and loop-related variables
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#include "LoopInfoTable.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;

///Default constructor
LoopInfoTable::LoopInfoTable() {

}


///Function useful to insert a new loop (its iterator operand) in nestedLoopMap
void LoopInfoTable::insertNewLoopIt(int *iteratorOp) {
    std::list<int *> emptyList;
    nestedLoopMap.insert(std::pair<int *, std::list<int *>>(iteratorOp, emptyList));
}

///Function useful to insert a new item inside the list associated to a loop iterator in nestedLoopMap
void LoopInfoTable::pushNestedLoop(int *loopIt, int *higherLevelLoop) {

    //Find the loop IT
    nestedLoopMapIT = nestedLoopMap.find(loopIt);
    //If found, push the higher level loop IT in the list of nested loops associated to loopIt
    if (nestedLoopMapIT != nestedLoopMap.end()) {
        (nestedLoopMapIT->second).push_back(higherLevelLoop);
    }
}

///Function useful to insert infos about a loop
void LoopInfoTable::insertLoopInfo(int *iteratorOp, loopInfoStruct loopInfo) {
    loopInfoMap.insert(std::pair<int *, LoopInfoTable::loopInfoStruct>(iteratorOp, loopInfo));
}

///Function useful to retrieve infos about a loop
LoopInfoTable::loopInfoStruct LoopInfoTable::getLoopInfo(int *iteratorOp) {
    loopInfoMapIT = loopInfoMap.find(iteratorOp);
    if (loopInfoMapIT != loopInfoMap.end()) {
        return loopInfoMapIT->second;
    }

    return loopInfoMapIT->second;
}

///Function useful to retrieve the pair <loopIt, list of higher level loops that contain it>
std::pair<int *, std::list<int *>> LoopInfoTable::getNestedLoops(int *iteratorOp) {
    nestedLoopMapIT = nestedLoopMap.find(iteratorOp);

    if (nestedLoopMapIT != nestedLoopMap.end()) {
        return *nestedLoopMapIT;
    }

    return *nestedLoopMapIT;

}

///Function useful to retrieve the loop iterator given the loop body
int *LoopInfoTable::getLoopIteratorFromLoopBody(int *loopBody) {

    for (loopInfoMapIT = loopInfoMap.begin(); loopInfoMapIT != loopInfoMap.end(); ++loopInfoMapIT) {
        for (auto listIT = ((loopInfoMapIT->second).loopBody).begin();
             listIT != ((loopInfoMapIT->second).loopBody).end(); ++listIT) {
            if (*listIT == loopBody) {
                return loopInfoMapIT->first;
            }
        }
    }

    return loopInfoMapIT->first;
}

///It returns the iteration vector, a list for a nested loop structure made up of all loops iterators in 
///depth growing order, given the innermost loop iterator as input
std::list<int *> LoopInfoTable::getIterationVector(int *innermostLoopIterator) {
    std::list<int *> iterationVector;

    nestedLoopMapIT = nestedLoopMap.find(innermostLoopIterator);

    if (nestedLoopMapIT != nestedLoopMap.end()) {
        for (auto listIT = (nestedLoopMapIT->second).begin(); listIT != (nestedLoopMapIT->second).end(); ++listIT) {
            iterationVector.push_front(*listIT);
        }
        iterationVector.push_back(innermostLoopIterator);
    }

    return iterationVector;
}

/*-----------------------------DEBUG FUNCTIONS-------------------------------*/
void LoopInfoTable::printNestedLoopMap() {
    errs() << "Printing NestedLoopMap\n";
    for (nestedLoopMapIT = nestedLoopMap.begin(); nestedLoopMapIT != nestedLoopMap.end(); ++nestedLoopMapIT) {
        errs() << "\t" << nestedLoopMapIT->first << "\n";
        for (auto listIT = (nestedLoopMapIT->second).begin(); listIT != (nestedLoopMapIT->second).end(); ++listIT) {
            errs() << "\t\t" << *listIT << "\n";
        }
    }
}

void LoopInfoTable::printLoopInfoMap() {
    errs() << "Printing LoopInfoMap\n";
    for (loopInfoMapIT = loopInfoMap.begin(); loopInfoMapIT != loopInfoMap.end(); ++loopInfoMapIT) {
        errs() << "\tit " << loopInfoMapIT->first <<
               "\titerations " << (loopInfoMapIT->second).iterations <<
               "\tfv " << (loopInfoMapIT->second).iteratorFinalValue <<
               "\tinc " << (loopInfoMapIT->second).iteratorIncrement <<
               "\tiv " << (loopInfoMapIT->second).iteratorInitValue <<
               "\tviv " << (loopInfoMapIT->second).variableInitValue <<
               "\tvfv " << (loopInfoMapIT->second).variableFinalValue <<
               "\nisF " << (loopInfoMapIT->second).isFinalValueConstant <<
               "\tisI " << (loopInfoMapIT->second).isInitValueConstant <<
               "\tLH " << (loopInfoMapIT->second).loopHeader <<
               "\tLL " << (loopInfoMapIT->second).loopLatch;
        for (auto listIT = ((loopInfoMapIT->second).loopBody).begin();
             listIT != ((loopInfoMapIT->second).loopBody).end(); ++listIT) {
            errs() << "\tLB " << *listIT;
        }
        errs() << "\n\n";
    }
}
/*-----------------------------DEBUG FUNCTIONS-------------------------------*/